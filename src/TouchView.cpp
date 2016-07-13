
#include "TouchView.h"

#include "cinder/gl/gl.h"
#include "boost/lexical_cast.hpp"
#include <math.h>

using namespace std;
using namespace ci;
using namespace ci::app;

namespace bluecadet {
namespace views {

int	TouchView::TotalObjectCount = 0;
int	TouchView::ObjectID = 0;

//==================================================
// Setup/Destruction
//

TouchView::TouchView(bool dragEnabled) :
	BaseView(),
	mSize(0.0f),
	mUniqueID(ObjectID),
	mDragEnabled(dragEnabled),
	mTouchEnabled(true),
	mMultiTouchEnabled(false),
	mHasMovingTouches(false),
	mAllowsTapReleaseOutside(false),
	mIsDragging(false),
	mIsActive(false),
	mDragThreshold(20.0f),
	mMaxTapDuration(2.0),
	mCurTouchPos(0, 0),
	mPrevTouchPos(0, 0),
	mInitialRelTouchPos(0, 0),
	mInitialAbsTouchPos(0, 0),
	mInitialPosWhenTouched(0, 0),
	mInitialTouchTime(0)
{
	TotalObjectCount++;
	ObjectID++;
}

TouchView::~TouchView() {
	// Call to unregister should be made BEFORE you destroy your touchable object
	mPath.clear();
	TotalObjectCount--;
}

void TouchView::reset() {
	BaseView::reset();
	cancelTouches();
}

void TouchView::setup(const cinder::vec2 &size) {
	// Create shape from this position and size
	vector<cinder::vec2> coordinates;
	vec2 startCoord = vec2(0);
	coordinates.push_back(startCoord);
	coordinates.push_back(vec2(startCoord.x + size.x, startCoord.y));
	coordinates.push_back(vec2(startCoord.x + size.x, startCoord.y + size.y));
	coordinates.push_back(vec2(startCoord.x, startCoord.y + size.y));
	createShape(coordinates);
	// Clean up
	coordinates.clear();
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
	mPath = cinder::Path2d();

	for (int i = 0; i <= coordinates.size() - 1; i++) {
		if (i == 0) mPath.moveTo(coordinates[0]);
		else mPath.lineTo(coordinates[i]);
	}

	mPath.close();

	mSize = mPath.calcBoundingBox().getSize();
}

//==================================================
// Touch Management
//

void TouchView::touchesBeganHandler(int touchID, const ci::vec2 &touchPnt, TouchType touchType) {
	mObjectTouchIDs.push_back(touchID);

	mPrevTouchPos = touchPnt; // Set to current touchPnt, otherwise prevtouch pos may be anywhere
	mCurTouchPos = touchPnt;
	mInitialRelTouchPos = touchPnt;
	mInitialAbsTouchPos = convertLocalToGlobal(touchPnt);
	mInitialPosWhenTouched = getGlobalPosition();
	mInitialTouchTime = getElapsedSeconds();
	mIsDragging = getNumTouches() == 1 ? false : mIsDragging;
	mHasMovingTouches = getNumTouches() == 1 ? false : mHasMovingTouches;

	mDidBeginTouch(shared_from_this());
}

void TouchView::touchesMovedHandler(int touchID, const ci::vec2 &touchPnt, TouchType touchType) {
	if (mObjectTouchIDs.empty() || mObjectTouchIDs.front() != touchID) {
		return;
	}

	mHasMovingTouches = true;

	if (!mDragEnabled) {
		return;
	}

	mPrevTouchPos = mCurTouchPos;
	mCurTouchPos = touchPnt;

	if (!mIsDragging) {
		const float dragDistance2 = glm::distance2(mInitialAbsTouchPos, convertLocalToGlobal(touchPnt));
		mIsDragging = dragDistance2 > mDragThreshold * mDragThreshold;
	}

	mDidMoveTouch(shared_from_this());
}

void TouchView::touchesEndedHandler(int touchID, const ci::vec2 &touchPnt, TouchType touchType) {
	mDidEndTouch(shared_from_this());

	bool didTap = mAllowsTapReleaseOutside || hasTouchPoint(touchPnt);

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
		mDidTap(shared_from_this());
	}

	resetTouchState();
}

void TouchView::cancelTouches() {
	/*
	std::shared_ptr<TouchManager> manager = TouchManager::getInstance();

	const auto sharedPtr = shared_from_this();
	const auto touchIds = vector<int>(mObjectTouchIDs.begin(), mObjectTouchIDs.end());

	for (const auto i : touchIds) {
		manager->endTouch(i);
		mDidCancelTouch(sharedPtr);
	}

	mObjectTouchIDs.clear();
	*/
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

bool TouchView::hasTouchPoint(const vec2 &pnt) {
	vec2 localPoint = convertGlobalToLocal(pnt);
	return mPath.contains(localPoint);
}

bool TouchView::canAcceptTouch() const {
	return mMultiTouchEnabled || mObjectTouchIDs.empty();
}

//==================================================
// Debugging
//

void TouchView::drawDebugShape() const {
	gl::lineWidth(2.0f);
	gl::draw(mPath);
}

}
}