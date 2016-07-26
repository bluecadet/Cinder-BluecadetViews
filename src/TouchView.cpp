
#include "TouchView.h"

#include "cinder/gl/gl.h"
#include "boost/lexical_cast.hpp"
#include <math.h>

#include "TouchManager.h"

using namespace std;
using namespace ci;
using namespace ci::app;

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
	mIsDragging(false),
	mDragThreshold(20.0f),
	mMaxTapDuration(2.0),
	mCurTouchPos(0, 0),
	mPrevTouchPos(0, 0),
	mInitialRelTouchPos(0, 0),
	mInitialAbsTouchPos(0, 0),
	mInitialPosWhenTouched(0, 0),
	mInitialTouchTime(0)
{
}

TouchView::~TouchView() {
	cancelTouches();
}

void TouchView::reset() {
	BaseView::reset();
	cancelTouches();
}

void TouchView::setup(float radius) {

	// Touchable area of each circle is created from 50 points. This number may be adjusted.
	int points = 30;
	double slice = 2.0 * M_PI / points;
	vector<cinder::vec2> coordinates;
	for (int i = 0; i < points; i++) {
		double angle = slice * i;
		int newX = (int)(radius *  cos(angle));
		int newY = (int)(radius * sin(angle));
		coordinates.push_back(vec2(newX, newY));
	}

	createShape(coordinates);
	coordinates.clear();
}

void TouchView::setup(const std::vector<cinder::vec2> &coordinates, const cinder::vec2 &pos) {
	createShape(coordinates);
}

void TouchView::createShape(const std::vector<cinder::vec2> &coordinates) {
	mTouchablePath = cinder::Path2d();
	mTouchablePath.moveTo(coordinates[0]);

	for (int i = 1; i < coordinates.size(); ++i) {
		mTouchablePath.lineTo(coordinates[i]);
	}

	mTouchablePath.close();

	setSize(mTouchablePath.calcBoundingBox().getSize());
}

//==================================================
// Touch Management
//

void TouchView::processTouchBegan(const touch::TouchEvent& touchEvent) {
	mObjectTouchIDs.push_back(touchEvent.id);

	mPrevTouchPos = touchEvent.position; // Set to current touchPnt, otherwise prevtouch pos may be anywhere
	mCurTouchPos = touchEvent.position;
	mInitialRelTouchPos = touchEvent.position;
	mInitialAbsTouchPos = convertLocalToGlobal(touchEvent.position);
	mInitialPosWhenTouched = getGlobalPosition();
	mInitialTouchTime = getElapsedSeconds();
	mIsDragging = getNumTouches() == 1 ? false : mIsDragging;
	mHasMovingTouches = getNumTouches() == 1 ? false : mHasMovingTouches;

	handleTouchBegan(touchEvent);
	mDidBeginTouch(touchEvent);
}

void TouchView::processTouchMoved(const touch::TouchEvent& touchEvent) {
	if (mObjectTouchIDs.empty() || mObjectTouchIDs.front() != touchEvent.id) {
		return;
	}

	mHasMovingTouches = true;

	if (!mMovingTouchesEnabled) {
		return;
	}

	mPrevTouchPos = mCurTouchPos;
	mCurTouchPos = touchEvent.position;

	if (!mIsDragging) {
		const float dragDistance2 = glm::distance2(mInitialAbsTouchPos, convertLocalToGlobal(mCurTouchPos));
		mIsDragging = dragDistance2 > mDragThreshold * mDragThreshold;
	}

	handleTouchMoved(touchEvent);
	mDidMoveTouch(touchEvent);
}

void TouchView::processTouchCanceled(const bluecadet::touch::TouchEvent& touchEvent) {
	handleTouchCanceled(touchEvent);
	mDidCancelTouch(touchEvent);
}

void TouchView::processTouchEnded(const touch::TouchEvent& touchEvent) {
	handleTouchEnded(touchEvent);
	mDidEndTouch(touchEvent);

	bool didTap = mAllowsTapReleaseOutside || containsPoint(touchEvent.localPosition);

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
	}

	resetTouchState();
}

void TouchView::cancelTouches() {
	// TODO: Move this logic to the touch manager

	/*std::shared_ptr<touch::TouchManager> manager = touch::TouchManager::getInstance();

	const auto sharedPtr = shared_from_this();
	const auto touchIds = vector<int>(mObjectTouchIDs.begin(), mObjectTouchIDs.end());

	for (const auto i : touchIds) {
		manager->endTouch(i);
		mDidCancelTouch(sharedPtr);
	}

	mObjectTouchIDs.clear();*/
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

	if (mTouchablePath.getNumPoints() <= 0) {
		// simply check if within size when no path defined
		return 
			point.x >= 0 && point.x <= size.x &&
			point.y >= 0 && point.y <= size.y;
	}

	return mTouchablePath.contains(point);
}

bool TouchView::canAcceptTouch() const {
	return mMultiTouchEnabled || mObjectTouchIDs.empty();
}

//==================================================
// Debugging
//

//void TouchView::drawDebugShape() const {
//	gl::lineWidth(2.0f);
//	gl::draw(mPath);
//}

}
}