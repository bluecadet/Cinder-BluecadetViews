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
	lock_guard<recursive_mutex> scopedUpdateLock(mQueueMutex);
	lock_guard<recursive_mutex> scopedTouchMapLock(mTouchIdMutex);
	
	mAppSize = appSize;
	mAppTransform = appTransform;

	// pre update plugins
	for (auto plugin : mPlugins) {
		plugin->preUpdate(this);
	}

	// process touches
	for (auto & touch : mTouchQueue) {
		switch (touch.phase) {
			case TouchPhase::Began: mainThreadTouchesBegan(touch, rootView); break;
			case TouchPhase::Moved: mainThreadTouchesMoved(touch, rootView); break;
			case TouchPhase::Ended: mainThreadTouchesEnded(touch, rootView); break;
		}
	}

	mTouchQueue.clear();

	// post update plugins
	for (auto plugin : mPlugins) {
		plugin->postUpdate(this);
	}
}

//==================================================
// Touch Management
//

void TouchManager::addTouch(const int id, const ci::vec2 & position, const TouchType type, const TouchPhase phase) {
	addTouch(Touch(id, position, type, phase));
}

void TouchManager::addTouch(Touch & touch)
{
	// transform position from window into app space
	const vec2 transformedPos = vec2(mAppTransform * vec4(touch.windowPosition, 0, 1));
	touch.appPosition = transformedPos;

	lock_guard<recursive_mutex> scopedUpdateLock(mQueueMutex);
	mTouchQueue.push_back(touch);
	mLatestTouchTime = (float)getElapsedSeconds(); // Update the most recent touch time on the app
}

void TouchManager::mainThreadTouchesBegan(const Touch & touch, views::BaseViewRef rootView) {

	// only store virtual touches, but don't process further
	if (touch.isVirtual) {
		mTouchesById[touch.id] = touch;
		return;
	}

	// checks if MultiTouch is enabled OR it's not enabled AND touch map had no views in it
	if (mMultiTouchEnabled || (!mMultiTouchEnabled && mViewsByTouchId.size() == 0)) {
		TouchEvent touchEvent(touch);
		TouchView* view = getTopViewAtPosition(touch.appPosition, rootView);

		if (view) {
			touchEvent.touchTarget = view->shared_from_this();
			touchEvent.localPosition = view->convertGlobalToLocal(touchEvent.position);
		}

		mDidBeginTouch(touchEvent);

		if (view) {
			mViewsByTouchId[touchEvent.touchId] = views::TouchViewWeakRef(touchEvent.touchTarget);
			view->processTouchBegan(touchEvent);
			view->dispatchEvent(touchEvent); // TODO: move this into processTouch...()
		}

		if (view || !mDiscardMissedTouches) {
			mTouchesById[touchEvent.touchId] = touch;
		}

		// process event in plugins
		for (auto plugin : mPlugins) {
			plugin->processEvent(this, touchEvent);
		}
	}
}

void TouchManager::mainThreadTouchesMoved(const Touch & touch, views::BaseViewRef rootView) {
	// only store virtual touches, but don't process further
	if (touch.isVirtual) {
		mTouchesById[touch.id] = touch;
		return;
	}

	TouchEvent touchEvent(touch);
	auto view = getViewForTouchId(touchEvent.touchId);

	if (view) {
		touchEvent.touchTarget = view;
		touchEvent.localPosition = view->convertGlobalToLocal(touchEvent.position);
	}

	mDidMoveTouch(touchEvent);

	if (view) {
		view->processTouchMoved(touchEvent);
		view->dispatchEvent(touchEvent); // TODO: move this into processTouch...()
	}

	if (view || !mDiscardMissedTouches) {
		mTouchesById[touchEvent.touchId] = touch;
	}

	// process event in plugins
	for (auto plugin : mPlugins) {
		plugin->processEvent(this, touchEvent);
	}
}

void TouchManager::mainThreadTouchesEnded(const Touch & touch, views::BaseViewRef rootView, const bool canceled) {
	TouchViewRef view = nullptr;
	TouchEvent touchEvent(touch);

	touchEvent.isCanceled = canceled;

	{// scoped lock start
		lock_guard<recursive_mutex> scopedTouchMapLock(mTouchIdMutex);

		// remove view
		const auto viewIt = mViewsByTouchId.find(touchEvent.touchId);

		if (viewIt != mViewsByTouchId.end()) {
			view = viewIt->second.lock();
			mViewsByTouchId.erase(viewIt);
		}

		// remove event
		const auto eventIt = mTouchesById.find(touchEvent.touchId);
		if (eventIt != mTouchesById.end()) {
			mTouchesById.erase(eventIt);
		}

	}// scoped lock end

	if (view) {
		touchEvent.touchTarget = view;
		touchEvent.localPosition = view->convertGlobalToLocal(touchEvent.position);
	}

	if (touch.isVirtual) {
		// don't process virtual events any further
		return;
	}

	mDidEndTouch(touchEvent);

	if (view) {
		view->processTouchEnded(touchEvent);
		view->dispatchEvent(touchEvent); // TODO: move this into processTouch...()
	}

	// process event in plugins
	for (auto plugin : mPlugins) {
		plugin->processEvent(this, touchEvent);
	}
}

void TouchManager::cancelTouch(TouchViewRef touchView) {
	vector<Touch> touchesToEnd;

	{// scoped lock start
		lock_guard<recursive_mutex> scopedUpdateLock(mQueueMutex);
		lock_guard<recursive_mutex> scopedTouchMapLock(mTouchIdMutex);

		for (auto & it : mViewsByTouchId) {
			if (it.second.lock() == touchView) {
				const auto & touch = mTouchesById[it.first];
				touchesToEnd.push_back(touch);
			}
		}
	}// scoped lock end

	for (const auto & touch : touchesToEnd) {
		mainThreadTouchesEnded(touch, touchView);
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
	const auto & children = rootView->getChildren();
	for (auto it = children.rbegin(); it != children.rend(); ++it) {
		const auto & touchedChild = getTopViewAtPosition(position, *it);
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

void TouchManager::debugDrawTouch(const Touch & touch, const bool isVirtual) {
	static const ColorA labelColor = ColorA(1, 1, 1, 0.85f);
	static const Font labelFont = Font("Arial", 16.0f);
	static const float innerRadius = 12.0f;
	static const float outerRadius = 16.0f;

	static const float circlePadding = 2.0f;
	static const float circleScale = 2.0f;
	static const ColorA circleColor = ColorA(1, 1, 1, 0.85f);
	static const vec2 circleSize = vec2(2.0f * (outerRadius + circlePadding));
	static const Rectf circleDestRect = Rectf(-circleSize * 0.5f, circleSize * 0.5f);
	static const vec2 labelOffset = vec2(outerRadius, -labelFont.getSize() * 0.5f);

	static gl::FboRef fboNormal = nullptr;
	static gl::FboRef fboVirtual = nullptr;

	const string labelText = to_string(touch.id) +
		" (" + to_string((int)touch.appPosition.x) +
		", " + to_string((int)touch.appPosition.y) + ")";

	// cached buffer for circle texture
	if (!fboNormal) {
		fboNormal = gl::Fbo::create((int)(circleSize.x * circleScale), (int)(circleSize.y * circleScale));
		fboVirtual = gl::Fbo::create((int)(circleSize.x * circleScale), (int)(circleSize.y * circleScale));

		gl::ScopedMatrices scopedMatrices;
		gl::ScopedViewport scopedViewport(fboNormal->getSize());
		gl::ScopedColor scopedColor(circleColor);

		gl::setMatricesWindow(fboNormal->getSize());
		gl::scale(vec2(circleScale));

		fboNormal->bindFramebuffer();
		gl::drawSolidCircle(vec2(outerRadius + circlePadding), innerRadius, 32);
		gl::drawStrokedCircle(vec2(outerRadius + circlePadding), outerRadius, 2, 32);
		fboNormal->unbindFramebuffer();

		fboVirtual->bindFramebuffer();
		gl::drawStrokedCircle(vec2(outerRadius + circlePadding), outerRadius, 2, 32);
		fboVirtual->unbindFramebuffer();
	}

	// draw the event
	gl::ScopedMatrices scopedMatrices;
	gl::translate(touch.appPosition);

	const auto fbo = isVirtual ? fboVirtual : fboNormal;

	gl::draw(fbo->getColorTexture(), circleDestRect);
	gl::drawString(labelText, labelOffset, labelColor, labelFont);
}

void TouchManager::debugDrawTouches() {
	lock_guard<recursive_mutex> scopedTouchMapLock(mTouchIdMutex);
	gl::ScopedBlendPremult scopedBlend;
	for (const auto & it : mTouchesById) {
		const auto & touch = it.second;
		debugDrawTouch(touch, touch.isVirtual);
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