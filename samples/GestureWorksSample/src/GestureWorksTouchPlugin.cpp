#include "GestureWorksTouchPlugin.h"

#include "gwc/GestureWorksCore.h"
#include <views/TouchView.h>

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::views;

namespace bluecadet {
namespace touch {

GestureWorksTouchPlugin::GestureWorksTouchPlugin() :
	mIsInitialized(false),
	mAppSize(0)
{
}

GestureWorksTouchPlugin::~GestureWorksTouchPlugin() {
}

void GestureWorksTouchPlugin::initialize() {
	if (mAppSize.x == 0 || mAppSize.y == 0) {
		// Auto-detect window size if app size is not set yet
		mAppSize = getWindowSize();
	}

	if (mIsInitialized) {
		// Only update the app size if we already initialized before
		gwc::GestureWorks::getInstance()->resizeScreen((int)mAppSize.x, (int)mAppSize.y);
		return;
	}

	const string dllPath = getAssetPath("GestureworksCore32.dll").string();
	const string gmlPath = getAssetPath("basic_manipulation.gml").string();

	if (gwc::GestureWorks::getInstance()->loadGestureWorks(dllPath) != 0) {
		console() << "TouchManager Error: Could not load GestureWorks DLL at: '" << dllPath << "'" << endl;
		return;
	}

	// Usually GW works in screen space, but since we're passing in our own events, we want to stick to our app space
	gwc::GestureWorks::getInstance()->initializeGestureWorks((int)mAppSize.x, (int)mAppSize.y);

	if (!gwc::GestureWorks::getInstance()->loadGML(gmlPath)) {
		console() << "TouchManager Error: Could not find gml file for gestures at: '" << gmlPath << "'" << endl;
	}

	mIsInitialized = true;
}

void GestureWorksTouchPlugin::wasAddedTo(TouchManager * manager) {
	mAppSize = manager->getAppSize();
	initialize();
}

void GestureWorksTouchPlugin::willBeRemovedFrom(TouchManager * manager) {
}

void GestureWorksTouchPlugin::preUpdate(TouchManager * manager) {
	const vec2 appSize = manager->getAppSize();
	if (appSize != mAppSize) {
		gwc::GestureWorks::getInstance()->resizeScreen((int)appSize.x, (int)appSize.y);
	}
}

void GestureWorksTouchPlugin::processEvent(TouchManager * manager, const TouchEvent & event) {
	// GW expects touch coordinates to be normalized. Normally this would be in screen coords,
	// but we're emulating events in app space, so we use app size instead of window size.
	vec2 touchPos = toPixels(event.position) / mAppSize;
	gwc::touchpoint touchPoint;
	touchPoint.init(event.touchId, touchPos.x, touchPos.y, 0, 1, 1);

	switch (event.touchPhase) {
		case TouchPhase::Began: touchPoint.status = gwc::touchStatus::TOUCHADDED; break;
		case TouchPhase::Moved: touchPoint.status = gwc::touchStatus::TOUCHUPDATE; break;
		case TouchPhase::Ended: touchPoint.status = gwc::touchStatus::TOUCHREMOVED; break;
	}

	gwc::GestureWorks::getInstance()->addEvent(touchPoint);

	if (event.touchTarget) {
		const auto touchViewId = event.touchTarget->getTouchViewId();
		const auto gestureCountIt = mNumGesturesPerViewId.find(touchViewId);

		if (gestureCountIt == mNumGesturesPerViewId.end()) {
			// if this is our first gesture on this view: register it
			mNumGesturesPerViewId[touchViewId] = 0;
			mViewsById[touchViewId] = TouchViewWeakRef(event.touchTarget);
			gwc::GestureWorks::getInstance()->registerTouchObject(touchViewId);
			gwc::GestureWorks::getInstance()->addGesture(touchViewId, "n-rotate-and-scale");
		}

		gwc::GestureWorks::getInstance()->assignTouchPoint(touchViewId, touchPoint.id);
	}
}

void GestureWorksTouchPlugin::postUpdate(TouchManager * manager) {
	gwc::GestureWorks::getInstance()->processFrame();

	const auto pointEvents = gwc::GestureWorks::getInstance()->consumePointEvents();
	const auto gestureEvents = gwc::GestureWorks::getInstance()->consumeGestureEvents();

	for (const auto & gesture : gestureEvents) {
		const string touchViewId = gesture.target;
		const auto gestureCountIt = mNumGesturesPerViewId.find(touchViewId);
		const auto viewIt = mViewsById.find(touchViewId);
		TouchViewRef view = nullptr;

		if (viewIt != mViewsById.end()) {
			view = viewIt->second.lock();
		}

		if (view) {
			// process gesture on view
			view->processGesture(gesture);
		}

		if (gestureCountIt != mNumGesturesPerViewId.end()) {

			if (gesture.phase == 0) {
				// incr gesture count when it begins
				gestureCountIt->second += 1;

			} else if (gesture.phase == 2) {
				// decr gesture count when it ends
				gestureCountIt->second -= 1;
			}

			// remove view if this is the last gesture and it ended
			if (gestureCountIt->second == 0) {
				mNumGesturesPerViewId.erase(gestureCountIt);
				mViewsById.erase(viewIt);
				gwc::GestureWorks::getInstance()->deregisterTouchObject(touchViewId);
			}
		}
	}
}

} // touch
} // bluecadet

