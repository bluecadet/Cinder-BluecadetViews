/**+---------------------------------------------------------------------------
Bluecadet Interactive 2016
Developers: Paul Rudolph, Stacey Martens & Ben Bojko
Contents:
Comments:
1. In setup, create each Path based on a (0,0) coordinate.
2. Move the Path to the desired starting location on screen by setting mPosition.
3. If you are translating the space, you have to update mTranslationPos by calling setTranslationPos in the draw function.
----------------------------------------------------------------------------*/

#pragma once
#include "cinder/app/App.h"
#include "cinder/Timeline.h"
#include "cinder/Shape2d.h"

#include <boost/signals2.hpp>

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class TouchView>		TouchViewRef;
typedef std::shared_ptr<const class TouchView>	TouchViewConstRef;
typedef std::weak_ptr<class TouchView>			TouchViewWeakRef;

//! TouchType options. Object could be a fiducial.
enum TouchType { TOUCH, OBJECT, MOUSE, SIMULATOR };

class TouchView : public BaseView, public std::enable_shared_from_this<TouchView> {

public:

	boost::signals2::signal<void(TouchViewRef)>	mDidBeginTouch;		//! Triggered for first touch when touch begins and for subsequent touches if multi-touch is enabled
	boost::signals2::signal<void(TouchViewRef)>	mDidMoveTouch;		//! Triggered for moving touches after touch began
	boost::signals2::signal<void(TouchViewRef)>	mDidEndTouch;		//! Triggered when touch ends and when touch is canceled
	boost::signals2::signal<void(TouchViewRef)>	mDidCancelTouch;	//! Triggered after mDidEndTouch when cancelTouches() is called
	boost::signals2::signal<void(TouchViewRef)>	mDidTap;			//! Triggered after mDidEndTouch if the touch fits the parameters for tapping


	//! Setup/Destruction
	TouchView(bool dragEnabled = false);
	virtual ~TouchView();

	//! Calls BaseView::reset() and cancels/ends all existing touches on this view.
	virtual void				reset() override;


	//! Setting up base touch object as a rectangle
	virtual void				setup(const cinder::vec2 &size = cinder::vec2(10.0f, 10.0f));
	//! Setting up base touch object as a circle
	virtual void				setup(float radius = 10.0);
	//! Setting up base touch object as random shape
	virtual void				setup(const std::vector<cinder::vec2> &coordinates, const cinder::vec2 &pos = cinder::vec2(0));


	//==================================================
	// Touch Management
	//

	void						createShape(const std::vector<cinder::vec2> &coordinates);	//! Iterate through coordinates passed in. Create Path2d of the touchable area.
	const cinder::Path2d		getPath() const { return mPath; };

	virtual void				cancelTouches();					//! Remove whatever touches are currently within the object and causes touchesEndedHandler() to be called
	virtual bool                hasTouchPoint(const ci::vec2 &pnt);	//! Returns whether or not this object should accept the touch point
	bool						canAcceptTouch() const;	//! Will return whether this touch object can accept a new touch based on its current state.

	virtual	void				touchesBeganHandler(int touchID, const ci::vec2 &touchPnt, TouchType touchType);
	virtual void				touchesMovedHandler(int touchID, const ci::vec2 &touchPnt, TouchType touchType);
	virtual void				touchesEndedHandler(int touchID, const ci::vec2 &touchPnt, TouchType touchType);


	//! Getters/Setters

	//! Set whether or not to accept new touches. Can be turned on/off.
	virtual void	setTouchEnabled(const bool state) { mTouchEnabled = state; };
	const bool		isTouchEnabled() const { return mTouchEnabled; }

	//! Accepts more than one touch if true, otherwise max of one touch if false. Defaults to false.
	bool			getMultiTouchEnabled() const { return mMultiTouchEnabled; }
	void			setMultiTouchEnabled(const bool value) { mMultiTouchEnabled = value; }

	//! Returns the total number of touches currently within the object
	const int		getNumTouches() const { return (int)mObjectTouchIDs.size(); }

	const ci::vec2&	getSize() const { return mSize; };		//! Returns size
	float			getWidth()	const { return mSize.x; };	//! Returns width as float
	float			getHeight()	const { return mSize.y; };	//! Returns height as float

	//! True once a touch has moved the minimum drag threshold
	bool			isDragging() const { return mIsDragging; };

	//! True if this object has any moving touches, even if they are below the drag threshold
	bool			hasMovingTouches() const { return mHasMovingTouches; };

	void			setCurTouchPosition(ci::vec2 pos) { mCurTouchPos = pos; };
	ci::vec2&		getCurTouchPosition() { return mCurTouchPos; };

	//! Maximum allowed distance to move a touch up to which it's considered a tap
	float			getDragThreshold() const { return mDragThreshold; }
	void			setDragThreshold(const float value) { mDragThreshold = value; }

	//! Maximum time in seconds until which a touch is considered a tap (vs a long press)
	double			getMaxTapDuration() const { return mMaxTapDuration; }
	void			setMaxTapDuration(const double value) { mMaxTapDuration = value; }

	//! When true, will allow a tap to trigger even if the touch up even occurs outside this touch object. Good for fast moving objects. Default is false.
	bool			getAllowsTapReleaseOutside() const { return mAllowsTapReleaseOutside; }
	void			setAllowsTapReleaseOutside(const bool value) { mAllowsTapReleaseOutside = value; }

	bool			isActive() const { return mIsActive; };
	void			setActive(bool activeState) { mIsActive = activeState; };


	//! Debugging

	//! Returns the unique ID tag that is set upon creation of the touch object
	const int		getUniqueID() const { return mUniqueID; }

	//! Draws an the outer box of the object, and the objects to string in the center, helpful for debugging pourposes.
	virtual void	drawDebugShape() const;

protected:
	void			resetTouchState();

	ci::vec2		mCurTouchPos;
	ci::vec2		mPrevTouchPos;
	ci::vec2		mInitialRelTouchPos;
	ci::vec2		mInitialAbsTouchPos;
	ci::vec2		mInitialPosWhenTouched;
	double			mInitialTouchTime;

	std::vector<int>mObjectTouchIDs;		//! Vector containing the touch IDs of the touches within this object

	ci::Path2d		mPath;					//! Coordinates of the object
	ci::vec2		mSize;					//! Calculated from path bounding box

private:
	bool			mTouchEnabled;
	bool			mMultiTouchEnabled;
	int				mUniqueID;				//! Object identification

	bool			mDragEnabled;
	bool			mIsActive;
	bool			mIsDragging;
	bool			mHasMovingTouches;
	bool			mAllowsTapReleaseOutside;

	float			mDragThreshold;
	double			mMaxTapDuration;


	//! Internal static members

	//! TotalObjectCount is used to count the number of Object instances for debugging purposes
	//! Total object count is a value that is incremented when an object is created and decremented when an object is destroyed. This way we always know how many objects exist.
	static int		TotalObjectCount;

	//! ObjectID is used to generate new unique ID's anytime an object is created. 
	static int		ObjectID;
};

}
}