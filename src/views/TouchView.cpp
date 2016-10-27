
#include "TouchView.h"
#include "../touch/TouchManager.h"

using namespace std;
using namespace ci;
using namespace ci::app;

// forward declaration for GWC -- temporary solution
namespace gwc {
class GestureEvent;
}

namespace bluecadet {
namespace views {

//==================================================
// Setup/Destruction
//

TouchView::TouchView() :
	BaseView(),
	mTouchEnabled(true),
	mMultiTouchEnabled(false),
	mMovingTouchesEnabled(true),
	mHasMovingTouches(false),
	mAllowsTapReleaseOutside(false),
	mDebugDrawTouchPath(false),
	mIsDragging(false),
	mDragThreshold(20.0f),
	mMaxTapDuration(2.0),
	mCurTouchPos(0, 0),
	mPrevTouchPos(0, 0),
	mInitialRelTouchPos(0, 0),
	mInitialAbsTouchPos(0, 0),
	mInitialPosWhenTouched(0, 0),
	mInitialTouchTime(0) {
}

TouchView::~TouchView() {
}

void TouchView::reset() {
	BaseView::reset();
	cancelTouches();
}

void TouchView::setup(const ci::vec2 size) {
	mTouchPath.clear();
	setSize(size);
}

void TouchView::setup(const float radius, const ci::vec2& offset, const int numSegments) {
	setTouchPath(radius, offset, numSegments);
}

void TouchView::setup(const ci::Path2d& path) {
	mTouchPath = path;
}

//==================================================
// Rendering
// 

void TouchView::draw() {
	BaseView::draw();

	if (!mDebugDrawTouchPath) {
		return;
	}

	gl::ScopedColor color(ColorA(1.0f, 0, 0, 0.5f));
	gl::ScopedLineWidth lineWidth(2.0f);

	if (mTouchPath.empty()) {
		gl::drawStrokedRect(Rectf(vec2(), getSize()));
	} else {
		gl::draw(mTouchPath);
	}
}

//==================================================
// Touch Management
//

void TouchView::processTouchBegan(const touch::TouchEvent& touchEvent) {
	mObjectTouchIDs.push_back(touchEvent.touchId);

	if (mObjectTouchIDs.size() <= 1) {
		mPrevTouchPos = touchEvent.position; // Set to current touchPnt, otherwise prevtouch pos may be anywhere
		mCurTouchPos = touchEvent.position;
		mInitialRelTouchPos = touchEvent.position;
		mInitialAbsTouchPos = convertLocalToGlobal(touchEvent.position);
		mInitialPosWhenTouched = getGlobalPosition();
		mInitialTouchTime = getElapsedSeconds();
	}

	mIsDragging = getNumTouches() == 1 ? false : mIsDragging;
	mHasMovingTouches = getNumTouches() == 1 ? false : mHasMovingTouches;

	handleTouchBegan(touchEvent);
	mDidBeginTouch(touchEvent);
}

void TouchView::processTouchMoved(const touch::TouchEvent& touchEvent) {
	if (mObjectTouchIDs.empty()) {
		return;
	}

	mHasMovingTouches = true;

	if (!mMovingTouchesEnabled) {
		return;
	}

	if (touchEvent.touchId == mObjectTouchIDs.front()) {
		mPrevTouchPos = mCurTouchPos;
		mCurTouchPos = touchEvent.position;

		if (!mIsDragging) {
			const float dragDistance2 = glm::distance2(mInitialAbsTouchPos, convertLocalToGlobal(mCurTouchPos));
			mIsDragging = dragDistance2 > mDragThreshold * mDragThreshold;
		}
	}

	handleTouchMoved(touchEvent);
	mDidMoveTouch(touchEvent);
}

void TouchView::processTouchEnded(const touch::TouchEvent& touchEvent) {
	handleTouchEnded(touchEvent);
	mDidEndTouch(touchEvent);

	bool didTap = (mAllowsTapReleaseOutside || containsPoint(touchEvent.localPosition)) && !touchEvent.isCanceled;

	// Only allow taps within a certain time
	if (didTap) {
		double touchDuration = getElapsedSeconds() - mInitialTouchTime;
		didTap = touchDuration <= mMaxTapDuration;
	}

	// Only allow taps within a certain drag distance
	if (didTap && mIsDragging) {
		didTap = false;
	}

	// Trigger tap if we had one
	if (didTap) {
		mDidTap(touchEvent);
		handleTouchTapped(touchEvent);
	}

	// Remove id from list
	auto idIt = std::find(mObjectTouchIDs.cbegin(), mObjectTouchIDs.cend(), touchEvent.touchId);
	if (idIt != mObjectTouchIDs.end()) {
		mObjectTouchIDs.erase(idIt);
	}

	if (mObjectTouchIDs.empty()) {
		resetTouchState();
	}
}

void TouchView::processGesture(const gwc::GestureEvent & gestureEvent) {
	handleGesture(gestureEvent);
	mDidReceiveGesture(gestureEvent);
}

void TouchView::cancelTouches() {
	std::shared_ptr<touch::TouchManager> touchManager = touch::TouchManager::getInstance();
	touchManager->cancelTouch(getSharedTouchViewPtr());
}

void TouchView::resetTouchState() {
	mIsDragging = false;
	mHasMovingTouches = false;

	mInitialRelTouchPos = vec2(0.0f, 0.0f);
	mInitialAbsTouchPos = vec2(0.0f, 0.0f);
	mInitialPosWhenTouched = vec2(0.0f, 0.0f);
	mInitialTouchTime = 0.0;

	mObjectTouchIDs.clear();
}

bool TouchView::containsPoint(const vec2 &point) {
	const vec2& size = BaseView::getSize();

	if (mTouchPath.empty()) {
		// simply check if within size when no path defined
		return
			point.x >= 0 && point.x <= size.x &&
			point.y >= 0 && point.y <= size.y;
	}

	return mTouchPath.contains(point);
}

bool TouchView::canAcceptTouch() const {
	return mMultiTouchEnabled || mObjectTouchIDs.empty();
}

void TouchView::setTouchPath(const float radius, const ci::vec2& offset, const int numSegments) {
	mTouchPath.clear();

	static const float twoPi = 2.0f * (float)M_PI;
	static const float defSegmentLength = 12.0f;
	static const float defMinNumSegments = 12.0f;
	const float n = numSegments >= 0 ? numSegments : max(defMinNumSegments, twoPi * radius / defSegmentLength);
	const float deltaAngle = twoPi / n;

	mTouchPath.moveTo(offset.x + radius, offset.y);
	for (float angle = deltaAngle; angle < twoPi; angle += deltaAngle) {
		mTouchPath.lineTo(offset.x + radius * cosf(angle), offset.y + radius * sinf(angle));
	}
	mTouchPath.close();
}

}
}
