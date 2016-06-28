#include "cinder/gl/gl.h"
#include "TouchManager.h"
#include "BaseTouchView.h"
#include "boost/lexical_cast.hpp"
#include <math.h>

using namespace std;
using namespace ci;
using namespace ci::app;

namespace touch {

	int	BaseTouchView::TotalObjectCount = 0;
	int	BaseTouchView::ObjectID = 0;

	//==================================================
	// Setup/Destruction
	//

	BaseTouchView::BaseTouchView(bool dragEnabled) :
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

	BaseTouchView::~BaseTouchView() {
		//The call to unregister should be made BEFORE you destroy your touchable object
		mPath.clear();
		TotalObjectCount--;
	}

	void BaseTouchView::reset() {
		BaseView::reset();
		cancelTouches();
	}

	void BaseTouchView::setupBaseTouchObj(const cinder::vec2 &size, bool registerWithTouchManager) {
		//create shape from this position and size
		vector<cinder::vec2> coordinates;
		vec2 startCoord = vec2(0);
		coordinates.push_back(startCoord);
		coordinates.push_back(vec2(startCoord.x + size.x, startCoord.y));
		coordinates.push_back(vec2(startCoord.x + size.x, startCoord.y + size.y));
		coordinates.push_back(vec2(startCoord.x, startCoord.y + size.y));
		createShape(coordinates);
		//clean up
		coordinates.clear();
	}

	void BaseTouchView::setupBaseTouchObj(float radius, bool registerWithTouchManager) {
		//! Touchable area of each circle is created from 50 points. This number may be adjusted.
		int points = 30;
		double slice = 2 * M_PI / points;
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

	void BaseTouchView::setupBaseTouchObj(const std::vector<cinder::vec2> &coordinates, const cinder::vec2 &pos, bool registerWithTouchManager) {
		createShape(coordinates);
	}

	void BaseTouchView::createShape(const std::vector<cinder::vec2> &coordinates) {
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

	void BaseTouchView::touchesBeganHandler(int touchID, const ci::vec2 &touchPnt, touch::TouchType touchType) {
		mObjectTouchIDs.push_back(touchID);

		mPrevTouchPos = touchPnt; // Set to current touch pt, otherwise prevtouch pos may be anywhere
		mCurTouchPos = touchPnt;
		mInitialRelTouchPos = touchPnt;
		mInitialAbsTouchPos = convertLocalToGlobal(touchPnt);
		mInitialPosWhenTouched = getGlobalPosition();
		mInitialTouchTime = getElapsedSeconds();
		mIsDragging = getNumTouches() == 1 ? false : mIsDragging;
		mHasMovingTouches = getNumTouches() == 1 ? false : mHasMovingTouches;

		mDidBeginTouch(shared_from_this());
	}

	void BaseTouchView::touchesMovedHandler(int touchID, const ci::vec2 &touchPnt, touch::TouchType touchType) {
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

	void BaseTouchView::touchesEndedHandler(int touchID, const ci::vec2 &touchPnt, touch::TouchType touchType) {
		mDidEndTouch(shared_from_this());

		bool didTap = mAllowsTapReleaseOutside || hasTouchPoint(touchPnt);

		// only allow taps within a certain time
		if (didTap) {
			double touchDuration = getElapsedSeconds() - mInitialTouchTime;
			didTap = touchDuration <= mMaxTapDuration;
		}

		// only allow taps within a certain drag distance
		if (didTap && mIsDragging) {
			didTap = false;
		}

		// trigger tap if we had one
		if (didTap) {
			mDidTap(shared_from_this());
		}

		resetTouchState();
	}

	void BaseTouchView::cancelTouches() {
		std::shared_ptr<TouchManager> manager = TouchManager::getInstance();

		const auto sharedPtr = shared_from_this();
		const auto touchIds = vector<int>(mObjectTouchIDs.begin(), mObjectTouchIDs.end());

		for (const auto i : touchIds) {
			manager->endTouch(i);
			mDidCancelTouch(sharedPtr);
		}

		mObjectTouchIDs.clear();
	}

	void BaseTouchView::resetTouchState() {
		mIsDragging = false;
		mHasMovingTouches = false;

		mInitialRelTouchPos = vec2(0.0f, 0.0f);
		mInitialAbsTouchPos = vec2(0.0f, 0.0f);
		mInitialPosWhenTouched = vec2(0.0f, 0.0f);
		mInitialTouchTime = 0.0;

		mObjectTouchIDs.clear();
	};

	bool BaseTouchView::hasTouchPoint(const vec2 &pnt) {
		vec2 localPoint = convertGlobalToLocal(pnt);
		return mPath.contains(localPoint);
	}

	bool BaseTouchView::canAcceptTouch() const {
		return mMultiTouchEnabled || mObjectTouchIDs.empty();
	}

	//==================================================
	// Debugging
	//

	void BaseTouchView::drawDebugShape() const {
		gl::lineWidth(2.0f);
		gl::draw(mPath);
	}

};