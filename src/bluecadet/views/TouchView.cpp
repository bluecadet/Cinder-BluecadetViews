#ifndef NO_TOUCH

#include "TouchView.h"
#include "../touch/TouchManager.h"

using namespace std;
using namespace ci;
using namespace ci::app;

namespace bluecadet {
namespace views {

//==================================================
// Defaults
// 

float TouchView::sDefaultMinAlphaForTouches = 0.0f;


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
	mHasReachedDragThreshold(false),
	mDragEnabledX(false),
	mDragEnabledY(false),
	mMinAlphaForTouches(sDefaultMinAlphaForTouches),
	mDragThreshold(20.0f),
	mMaxTapDuration(2.0),
	mLocalTouchPos(0, 0),
	mPrevGlobalTouchPos(0, 0),
	mInitialLocalTouchPos(0, 0),
	mInitialGlobalTouchPos(0, 0),
	mInitialGlobalPosWhenTouched(0, 0),
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
	mTouchIds.push_back(touchEvent.touchId);

	const bool isFirstTouch = mTouchIds.size() == 1;

	if (isFirstTouch) {
		mPrevLocalTouchPos = touchEvent.localPosition;
		mPrevGlobalTouchPos = touchEvent.globalPosition;
		mLocalTouchPos = touchEvent.localPosition;
		mGlobalTouchPos = touchEvent.globalPosition;
		mInitialLocalTouchPos = touchEvent.localPosition;
		mInitialGlobalTouchPos = touchEvent.globalPosition;
		mInitialLocalPosWhenTouched = getPosition();
		mInitialGlobalPosWhenTouched = getGlobalPosition();
		mInitialTouchTime = getElapsedSeconds();
	}

	mHasReachedDragThreshold = isFirstTouch ? false : mHasReachedDragThreshold;
	mHasMovingTouches = isFirstTouch ? false : mHasMovingTouches;

	handleTouchBegan(touchEvent);
	mSignalTouchBegan.emit(touchEvent);
}

void TouchView::processTouchMoved(const touch::TouchEvent& touchEvent) {
	if (mTouchIds.empty()) {
		return;
	}

	mHasMovingTouches = true;

	if (!mMovingTouchesEnabled) {
		return;
	}

	if (touchEvent.touchId == mTouchIds.front()) {
		mPrevLocalTouchPos = mLocalTouchPos;
		mPrevGlobalTouchPos = mGlobalTouchPos;
		mLocalTouchPos = touchEvent.localPosition;
		mGlobalTouchPos = touchEvent.globalPosition;

		if (!mHasReachedDragThreshold) {
			const float dragDistance2 = glm::distance2(mInitialGlobalTouchPos, mGlobalTouchPos);
			mHasReachedDragThreshold = dragDistance2 > mDragThreshold * mDragThreshold;
		}
	}

	if (mHasReachedDragThreshold && (mDragEnabledX || mDragEnabledY)) {
		auto globalPos = touchEvent.globalPosition - mInitialGlobalTouchPos + mInitialGlobalPosWhenTouched;
		auto localPos = getParent() ? getParent()->convertGlobalToLocal(globalPos) : globalPos;

		if (!mDragEnabledX) localPos.x = getPosition().value().x;
		if (!mDragEnabledY) localPos.y = getPosition().value().y;

		setPosition(localPos);
	}

	handleTouchMoved(touchEvent);
	mSignalTouchMoved.emit(touchEvent);
}

void TouchView::processTouchEnded(const touch::TouchEvent& touchEvent) {
	handleTouchEnded(touchEvent);
	mSignalTouchEnded.emit(touchEvent);

	bool didTap = (mAllowsTapReleaseOutside || containsPoint(touchEvent.localPosition)) && !touchEvent.isCanceled;

	// Only allow taps within a certain time
	if (didTap) {
		double touchDuration = getElapsedSeconds() - mInitialTouchTime;
		didTap = touchDuration <= mMaxTapDuration;
	}

	// Only allow taps within a certain drag distance
	if (didTap && mHasReachedDragThreshold) {
		didTap = false;
	}

	// Trigger tap if we had one
	if (didTap) {
		mSignalTapped.emit(touchEvent);
		handleTouchTapped(touchEvent);
	}

	// Remove id from list
	auto idIt = std::find(mTouchIds.cbegin(), mTouchIds.cend(), touchEvent.touchId);
	if (idIt != mTouchIds.end()) {
		mTouchIds.erase(idIt);
	}

	if (mTouchIds.empty()) {
		resetTouchState();
	}
}

void TouchView::cancelTouches() {
	std::shared_ptr<touch::TouchManager> touchManager = touch::TouchManager::getInstance();
	touchManager->cancelTouch(getSharedTouchViewPtr());
	resetTouchState();
}

void TouchView::resetTouchState() {
	mHasReachedDragThreshold = false;
	mHasMovingTouches = false;

	mPrevLocalTouchPos = vec2(0);
	mPrevGlobalTouchPos = vec2(0);
	mLocalTouchPos = vec2(0);
	mGlobalTouchPos = vec2(0);
	mInitialLocalTouchPos = vec2(0);
	mInitialGlobalTouchPos = vec2(0);
	mInitialLocalPosWhenTouched = vec2(0);
	mInitialGlobalPosWhenTouched = vec2(0);
	mInitialTouchTime = 0;

	mTouchIds.clear();
}

bool TouchView::containsPoint(const vec2 &point) {

	if (mTouchPath.empty()) {
		// simply check if within size when no path defined
		const vec2& size = BaseView::getSize();
		return
			point.x >= 0 && point.x <= size.x &&
			point.y >= 0 && point.y <= size.y;
	}

	return mTouchPath.contains(point);
}

bool TouchView::canAcceptTouch(const bluecadet::touch::Touch & touch) const {
	return (mMultiTouchEnabled || mTouchIds.empty()) && (getAlphaConst() > mMinAlphaForTouches);
}

bool TouchView::isHandlingTouch(const int touchId) const {
	auto idIt = std::find(mTouchIds.cbegin(), mTouchIds.cend(), touchId);
	return (idIt != mTouchIds.end());
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

#endif
