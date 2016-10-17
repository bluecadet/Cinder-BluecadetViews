#include "TouchManager.h"
#include "boost/lexical_cast.hpp"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

namespace bluecadet {
namespace touch {

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace bluecadet::views;

//==================================================
// Lifecycle
//

TouchManager::TouchManager() :
	mDiscardMissedTouches(true),
	mMultiTouchEnabled(true)
{
}

TouchManager::~TouchManager() {
}

TouchManagerRef TouchManager::getInstance() {
	static TouchManagerRef instance = nullptr;
	if (!instance) {
		instance = TouchManagerRef(new TouchManager());
	}
	return instance;
}

void TouchManager::update(BaseViewRef rootView, const vec2 & appSize, const mat4 & appTransform) {
	lock_guard<recursive_mutex> scopedUpdateLock(mEventMutex);
	lock_guard<recursive_mutex> scopedTouchMapLock(mTouchIdMutex);
	
	mAppSize = appSize;
	mAppTransform = appTransform;

	// pre update plugins
	for (auto plugin : mPlugins) {
		plugin->preUpdate(this);
	}

	// process touches
	for (auto & event : mEventQueue) {
		switch (event.touchPhase) {
			case TouchPhase::Began: mainThreadTouchesBegan(event, rootView); break;
			case TouchPhase::Moved: mainThreadTouchesMoved(event, rootView); break;
			case TouchPhase::Ended: mainThreadTouchesEnded(event, rootView); break;
		}
	}

	mEventQueue.clear();

	// post update plugins
	for (auto plugin : mPlugins) {
		plugin->postUpdate(this);
	}
}

//==================================================
// Touch Management
//

void TouchManager::addTouchEvent(const int id, const ci::vec2 & position, const TouchType type, const TouchPhase phase) {
	addTouchEvent(TouchEvent(id, position, type, phase));
}

void TouchManager::addTouchEvent(TouchEvent event)
{
	// transform touches into app space
	const vec2 transformedPos = vec2(mAppTransform * vec4(event.position, 0, 1));
	event.position = transformedPos;

	lock_guard<recursive_mutex> scopedUpdateLock(mEventMutex);
	mEventQueue.push_back(event);
	mLatestTouchTime = (float)getElapsedSeconds(); // Update the most recent touch time on the app
}

void TouchManager::mainThreadTouchesBegan(TouchEvent & touchEvent, views::BaseViewRef rootView) {

	// only store virtual touches, but don't process further
	if (touchEvent.isVirtual) {
		mEventsByTouchId[touchEvent.touchId] = touchEvent;
		return;
	}

	// checks if MultiTouch is enabled OR it's not enabled AND touch map had no views in it
	if (mMultiTouchEnabled || (!mMultiTouchEnabled && mViewsByTouchId.size() == 0)) {
		TouchView* view = getTopViewAtPosition(touchEvent.position, rootView);

		if (view) {
			touchEvent.touchTarget = view->shared_from_this();
			touchEvent.localPosition = view->convertGlobalToLocal(touchEvent.position);
		}

		mDidBeginTouch(touchEvent);

		if (view) {
			mViewsByTouchId[touchEvent.touchId] = views::TouchViewWeakRef(touchEvent.touchTarget);
			view->processTouchBegan(touchEvent);
		}

		if (view || !mDiscardMissedTouches) {
			mEventsByTouchId[touchEvent.touchId] = touchEvent;
		}

		// process event in plugins
		for (auto plugin : mPlugins) {
			plugin->processEvent(this, touchEvent);
		}
	}
}

void TouchManager::mainThreadTouchesMoved(TouchEvent& touchEvent, views::BaseViewRef rootView) {
	// only store virtual touches, but don't process further
	if (touchEvent.isVirtual) {
		mEventsByTouchId[touchEvent.touchId] = touchEvent;
		return;
	}

	auto view = getViewForTouchId(touchEvent.touchId);

	if (view) {
		touchEvent.touchTarget = view;
		touchEvent.localPosition = view->convertGlobalToLocal(touchEvent.position);
	}

	mDidMoveTouch(touchEvent);

	if (view) {
		view->processTouchMoved(touchEvent);
	}

	if (view || !mDiscardMissedTouches) {
		mEventsByTouchId[touchEvent.touchId] = touchEvent;
	}

	// process event in plugins
	for (auto plugin : mPlugins) {
		plugin->processEvent(this, touchEvent);
	}
}

void TouchManager::mainThreadTouchesEnded(TouchEvent& touchEvent, views::BaseViewRef rootView) {
	TouchViewRef view = nullptr;

	{// scoped lock start
		lock_guard<recursive_mutex> scopedTouchMapLock(mTouchIdMutex);

		// remove view
		const auto viewIt = mViewsByTouchId.find(touchEvent.touchId);

		if (viewIt != mViewsByTouchId.end()) {
			view = viewIt->second.lock();
			mViewsByTouchId.erase(viewIt);
		}

		// remove event
		const auto eventIt = mEventsByTouchId.find(touchEvent.touchId);
		if (eventIt != mEventsByTouchId.end()) {
			mEventsByTouchId.erase(eventIt);
		}

	}// scoped lock end

	if (view) {
		touchEvent.touchTarget = view;
		touchEvent.localPosition = view->convertGlobalToLocal(touchEvent.position);
	}

	if (touchEvent.isVirtual) {
		// don't process virtual events any further
		return;
	}

	mDidEndTouch(touchEvent);

	if (view) {
		view->processTouchEnded(touchEvent);
	}

	// process event in plugins
	for (auto plugin : mPlugins) {
		plugin->processEvent(this, touchEvent);
	}
}

void TouchManager::cancelTouch(TouchViewRef touchView) {
	vector<TouchEvent> touchesToEnd;

	{// scoped lock start
		lock_guard<recursive_mutex> scopedUpdateLock(mEventMutex);
		lock_guard<recursive_mutex> scopedTouchMapLock(mTouchIdMutex);

		for (auto & it : mViewsByTouchId) {
			if (it.second.lock() == touchView) {
				auto touch = mEventsByTouchId[it.first];
				touch.isCanceled = true;
				touchesToEnd.push_back(touch);
			}
		}
	}// scoped lock end

	for (auto & touchEvent : touchesToEnd) {
		mainThreadTouchesEnded(touchEvent, touchView);
	}
}


//==================================================
// View Helpers
//

TouchViewRef TouchManager::getViewForTouchId(const int touchId) {
	lock_guard<recursive_mutex> scopedTouchMapLock(mTouchIdMutex);
	const auto touchViewIt = mViewsByTouchId.find(touchId);
	const bool containsId = touchViewIt != mViewsByTouchId.end();

	if (!containsId) {
		// Stop if we can't find this object
		return nullptr;
	}

	// Try to get a strong pointer; This could return nullptr
	return touchViewIt->second.lock();
}

TouchView* TouchManager::getTopViewAtPosition(const ci::vec2 &position, BaseViewRef rootView) {
	if (!rootView) {
		return nullptr;
	}

	if (rootView->isHidden()) {
		// Don't check for touches in this view or in children if hidden
		return nullptr;
	}

	TouchView* obj = dynamic_cast<TouchView*>(rootView.get());

	if (obj && (!obj->isTouchEnabled() || !obj->canAcceptTouch())) {
		// Don't check for touches in this view or in children if untouchable
		return nullptr;
	}

	// Go through children first
	const auto& children = rootView->getChildren();
	for (auto it = children.rbegin(); it != children.rend(); ++it) {
		const auto& touchedChild = getTopViewAtPosition(position, *it);
		if (touchedChild) {
			return touchedChild;
		}
	}

	if (obj) {
		// Check if the object itself is touched if none of the children were
		const vec2 localPosition = obj->convertGlobalToLocal(position);

		if (obj->containsPoint(localPosition)) {
			return obj;
		}
	}

	return nullptr;
}


//==================================================
// Debugging
//

void TouchManager::debugDrawTouch(const TouchEvent & touchEvent, const ColorA & color) {
	static const Color labelColor = Color(1, 1, 1);
	static const Font labelFont = Font("Arial", 16.0f);
	static const float innerRadius = 12.0f;
	static const float outerRadius = 16.0f;

	static const float circlePadding = 2.0f;
	static const float circleScale = 2.0f;
	static const vec2 circleSize = vec2(2.0f * (outerRadius + circlePadding));
	static const Rectf circleDestRect = Rectf(-circleSize * 0.5f, circleSize * 0.5f);
	static const vec2 labelOffset = vec2(outerRadius, -labelFont.getSize() * 0.5f);

	static gl::FboRef fbo = nullptr;

	const string labelText = to_string(touchEvent.touchId) +
		" (" + to_string((int)touchEvent.position.x) +
		", " + to_string((int)touchEvent.position.y) + ")";

	// cached buffer for circle texture
	if (!fbo) {
		fbo = gl::Fbo::create((int)(circleSize.x * circleScale), (int)(circleSize.y * circleScale));

		gl::ScopedMatrices scopedMatrices;
		gl::ScopedViewport scopedViewport(fbo->getSize());
		gl::setMatricesWindow(fbo->getSize());

		fbo->bindFramebuffer();

		gl::scale(vec2(circleScale));
		gl::drawSolidCircle(vec2(outerRadius + circlePadding), innerRadius, 32);
		gl::drawStrokedCircle(vec2(outerRadius + circlePadding), outerRadius, 2, 32);

		fbo->unbindFramebuffer();
	}

	// draw the event
	gl::ScopedColor scopedColor(color);
	gl::ScopedMatrices scopedMatrices;
	gl::translate(touchEvent.position);

	gl::draw(fbo->getColorTexture(), circleDestRect);
	gl::drawString(labelText, labelOffset, labelColor, labelFont);
}

void TouchManager::debugDrawTouches() {
	lock_guard<recursive_mutex> scopedTouchMapLock(mTouchIdMutex);

	static const ci::ColorA normalColor(1, 0, 1, 0.75f);
	static const ci::ColorA virtualColor(1, 0, 1, 0.5f);

	for (const auto & it : mEventsByTouchId) {
		const auto & event = it.second;
		debugDrawTouch(event, event.isVirtual ? virtualColor : normalColor);
	}
}

//==================================================
// Plug-Ins
// 

void TouchManager::addPlugin(TouchManagerPluginRef plugin) {
	mPlugins.push_back(plugin);
	plugin->wasAddedTo(this);
}

void TouchManager::removePlugin(TouchManagerPluginRef plugin) {
	auto it = find(mPlugins.cbegin(), mPlugins.cend(), plugin);
	if (it != mPlugins.cend()) {
		mPlugins.erase(it);
		plugin->willBeRemovedFrom(this);
	}
}

bool TouchManager::hasPlugin(TouchManagerPluginRef plugin) const {
	auto it = find(mPlugins.cbegin(), mPlugins.cend(), plugin);
	return it != mPlugins.cend();
}

}
}