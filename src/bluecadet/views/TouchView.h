#pragma once
#include "cinder/app/App.h"
#include "cinder/Timeline.h"
#include "cinder/Shape2d.h"
#include "cinder/Signals.h"

#include "BaseView.h"
#include "../touch/Touch.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class TouchView>		TouchViewRef;
typedef std::shared_ptr<const class TouchView>	TouchViewConstRef;
typedef std::weak_ptr<class TouchView>			TouchViewWeakRef;

class TouchView : public BaseView {

public:


	//==================================================
	// Global defaults/types
	//

	//! The minimum alpha required for touches to be accepted (alpha has to be greater than the min). Defaults to 0, so that when alpha = 0 touches are not accepted.
	static float			sDefaultMinAlphaForTouches;

	//==================================================
	// Life cycle
	// 
	//! Setup/Destruction
	TouchView();
	virtual ~TouchView();

	//! Calls BaseView::reset() and cancels/ends all existing touches on this view.
	virtual void	reset() override;

	//! Sets the size of this touch view and clears any existing touch path. You also just call setSize().
	virtual void	setup(const ci::vec2 size);

	//! Sets a circular touch path with radius and n segments with an origin at offset.
	//! If segments is below 0, it will be automatically calculated based on radius.
	//! This is a convenience method and can cause complex paths that slow down performance. Use sparingly.
	virtual void	setup(const float radius, const ci::vec2& offset = ci::vec2(0, 0), const int numSegments = -1);

	//! Sets a touch path with a custom shape in local coordinate space. You can also just call setTouchPath().
	virtual void	setup(const ci::Path2d& path);

	//! Returns a shared pointer to this instance. Use this instead of shared_from_this() if you want the shared pointer to be of type TouchViewRef.
	TouchViewRef	getSharedTouchViewPtr() { return std::dynamic_pointer_cast<TouchView>(shared_from_this()); }


	//==================================================
	// Signals
	// 
	touch::TouchSignal & getSignalTouchBegan()	{ return mSignalTouchBegan; }	//! Triggered for first touch when touch begins and for subsequent touches if multi-touch is enabled
	touch::TouchSignal & getSignalTouchMoved()	{ return mSignalTouchMoved; }	//! Triggered for moving touches after touch began
	touch::TouchSignal & getSignalTouchEnded()	{ return mSignalTouchEnded; }	//! Triggered when touch ends and when touch is canceled
	touch::TouchSignal & getSignalTapped()		{ return mSignalTapped; }		//! Triggered after mDidEndTouch if the touch fits the parameters for tapping


	//==================================================
	// Touch Management
	//

	virtual void	cancelTouches();						//! Remove whatever touches are currently within the object and causes touchesEndedHandler() to be called
	virtual bool	containsPoint(const ci::vec2& point);	//! Used for touch detection. Passes in a local point. Override this method to define more complex hit areas.
	virtual bool	canAcceptTouch(const bluecadet::touch::Touch & touch) const;	//! Will return whether this touch object can accept a new touch based on its current state.

	// Used by the touch manager and should not be overriden
	virtual void	processTouchBegan(const bluecadet::touch::TouchEvent& touchEvent) final;
	virtual void	processTouchMoved(const bluecadet::touch::TouchEvent& touchEvent) final;
	virtual void	processTouchEnded(const bluecadet::touch::TouchEvent& touchEvent) final;

	//! Getters/Setters

	//! Set whether or not to accept new touches. Can be turned on/off.
	void			setTouchEnabled(const bool state)		{ mTouchEnabled = state; };
	const bool		isTouchEnabled() const					{ return mTouchEnabled; }

	//! Accepts more than one touch if true, otherwise max of one touch if false. Defaults to false.
	bool			isMultiTouchEnabled() const				{ return mMultiTouchEnabled; }
	void			setMultiTouchEnabled(const bool value)	{ mMultiTouchEnabled = value; }

	//! Returns the total number of touches currently within the object
	const int		getNumTouches() const					{ return (int)mTouchIds.size(); }

	//! Set whether to process move events and whether to process whether this view is being dragged or tapped. Defaults to true.
	bool			getMovingTouchesEnabled() const				{ return mMovingTouchesEnabled; }
	void			setMovingTouchesEnabled(const bool value)	{ mMovingTouchesEnabled = value; }

	//! Sets whether this view should be dragged by moving touches. Defaults to false.
	bool			isDragEnabled() const					{ return mDragEnabledX || mDragEnabledY; };
	bool			isDragEnabledX() const					{ return mDragEnabledX; };
	bool			isDragEnabledY() const					{ return mDragEnabledY; };

	void			setDragEnabled(const bool drag)						{ mDragEnabledX = mDragEnabledY = drag; };
	void			setDragEnabled(const bool dragX, const bool dragY)	{ mDragEnabledX = dragX; mDragEnabledY = dragY; };
	void			setDragEnabledX(const bool drag)					{ mDragEnabledX = drag; };
	void			setDragEnabledY(const bool drag)					{ mDragEnabledY = drag; };

	//! True once a touch has moved the minimum drag threshold
	bool			hasReachedDragThreshold() const			{ return mHasReachedDragThreshold; };

	//! True if this object has any moving touches, even if they are below the drag threshold
	bool			hasMovingTouches() const				{ return mHasMovingTouches; };

	//! Touch position within this view with 0,0 being at this view's origin
	const ci::vec2 &	getLocalTouchPos() const			{ return mLocalTouchPos; };

	//! Touch position in app coordinate space with 0,0 at the app's origin
	const ci::vec2 &	getGlobalTouchPos() const			{ return mGlobalTouchPos; };

	//! Maximum allowed distance to move a touch up to which it's considered a tap
	float			getDragThreshold() const				{ return mDragThreshold; }
	void			setDragThreshold(const float value)		{ mDragThreshold = value; }

	//! Maximum time in seconds until which a touch is considered a tap (vs a long press)
	double			getMaxTapDuration() const				{ return mMaxTapDuration; }
	void			setMaxTapDuration(const double value)	{ mMaxTapDuration = value; }

	//! When true, will allow a tap to trigger even if the touch up even occurs outside this touch object. Good for fast moving objects. Default is false.
	bool			getAllowsTapReleaseOutside() const				{ return mAllowsTapReleaseOutside; }
	void			setAllowsTapReleaseOutside(const bool value)	{ mAllowsTapReleaseOutside = value; }

	//! The minimum alpha required for touches to be accepted (alpha has to be greater than the min). Defaults to sDefaultMinAlphaForTouches.
	float			getMinAlphaForTouches() const					{ return mMinAlphaForTouches; }
	void			setMinAlphaForTouches(const float value)		{ mMinAlphaForTouches = value; }

	//! Custom path that determines touchable area if configured. If no path is set, size will be used for hit detection.
	const ci::Path2d&	getTouchPath() const					{ return mTouchPath; }
	void				setTouchPath(const ci::Path2d value)	{ mTouchPath = value; }
	void				setTouchPath(const float radius, const ci::vec2 & offset = ci::vec2(0), const int numSegments = -1);

	//! If set to true, will draw the touch path with a debug color
	bool			getDebugDrawTouchPath() const				{ return mDebugDrawTouchPath; }
	void			setDebugDrawTouchPath(const bool value)		{ mDebugDrawTouchPath = value; }

	//! Returns true if this view is currently handling and owning the touch with touchId
	bool			isHandlingTouch(const int touchId) const;

protected:

	void			draw() override;
																				
	// Override these boilerplate methods to react to touch events
	virtual	void	handleTouchBegan(const bluecadet::touch::TouchEvent& touchEvent) {};
	virtual void	handleTouchMoved(const bluecadet::touch::TouchEvent& touchEvent) {};
	virtual void	handleTouchEnded(const bluecadet::touch::TouchEvent& touchEvent) {};
	virtual void	handleTouchTapped(const bluecadet::touch::TouchEvent& touchEvent) {};

	//! Resets all touch-state related variables to a non-touched state
	virtual void	resetTouchState();

	ci::vec2		mLocalTouchPos;
	ci::vec2		mGlobalTouchPos;
	ci::vec2		mPrevLocalTouchPos;
	ci::vec2		mPrevGlobalTouchPos;
	ci::vec2		mInitialLocalTouchPos;
	ci::vec2		mInitialGlobalTouchPos;
	ci::vec2		mInitialLocalPosWhenTouched;
	ci::vec2		mInitialGlobalPosWhenTouched;
	double			mInitialTouchTime;

	std::vector<int>	mTouchIds;
	ci::Path2d			mTouchPath;

private:

	touch::TouchSignal		mSignalTouchBegan;
	touch::TouchSignal		mSignalTouchMoved;
	touch::TouchSignal		mSignalTouchEnded;
	touch::TouchSignal		mSignalTapped;

	bool			mTouchEnabled;
	bool			mMultiTouchEnabled;
	bool			mMovingTouchesEnabled;
	bool			mDragEnabledX;
	bool			mDragEnabledY;

	bool			mHasReachedDragThreshold;
	bool			mHasMovingTouches;
	
	float			mDragThreshold;
	bool			mAllowsTapReleaseOutside;
	double			mMaxTapDuration;

	float			mMinAlphaForTouches;
	bool			mDebugDrawTouchPath;
};

}
}
