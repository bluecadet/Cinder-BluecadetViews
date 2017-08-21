//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//	Developers: Paul Rudolph, Stacey Martens & Ben Bojko
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------

#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Signals.h"

#include <mutex>

#include "Touch.h"
#include "TouchManagerPlugin.h"
#include "../views/TouchView.h"

namespace bluecadet {
namespace touch {

// Forward type definitions
typedef std::shared_ptr<class TouchManager> TouchManagerRef;

class TouchManager {

public:

	//==================================================
	// Broadcast signals
	//
	TouchSignal	& getSignalTouchBegan()	{ return mSignalTouchBegan; };	//! Global touch event. Fired before any touch objects receive this event
	TouchSignal	& getSignalTouchMoved()	{ return mSignalTouchMoved; };	//! Global touch event. Fired before any touch objects receive this event
	TouchSignal & getSignalTouchEnded()	{ return mSignalTouchEnded; };	//! Global touch event. Fired before any touch objects receive this event

	//==================================================
	// Lifecycle
	//

	//! Shared singleton instance
	static TouchManagerRef getInstance();
	virtual ~TouchManager();

	void					update(views::BaseViewRef rootView, const ci::vec2 & appSize, const ci::mat4 & appTransform = ci::mat4());


	//==================================================
	// Touch Management
	//
	
	//! Adds a touch event to the event queue which will be processed on the main thread and forwarded to views
	void					addTouch(const int id, const ci::vec2& relPosition, const TouchType type, const TouchPhase phase);
	void					addTouch(Touch & touch);

	//! Removes a touch if it exists. Views associated to this event will be notifed with touchEnded.
	void					cancelTouch(views::TouchViewRef touchView);


	//==================================================
	// Accessors
	//

	//! Time in seconds of the most recent touch event regardless of touch type or phase
	float                   getLatestTouchTime() { return mLatestTouchTime; };

	//! Immediately discard touches that begin outside of any active touch view if enabled
	bool					getDiscardMissedTouches() const { return mDiscardMissedTouches; }
	void					setDiscardMissedTouches(const bool value) { mDiscardMissedTouches = value; }

	//! Accessors for plugins
	const ci::vec2 &		getAppSize() const { return mAppSize; }
	const ci::mat4 &		getAppTransform() const { return mAppTransform; }


	//==================================================
	// Plug-ins
	// 
	void					addPlugin(TouchManagerPluginRef plugin);
	void					removePlugin(TouchManagerPluginRef plugin);
	bool					hasPlugin(TouchManagerPluginRef plugin) const;


	//==================================================
	// Debugging
	// 
	void					debugDrawTouch(const Touch & touch, const bool isVirtual);
	void					debugDrawTouches();


protected:
	//! Private to make the TouchManager a singleton
	TouchManager();


	//==================================================
	// Internal Helpers
	//

	//! TUIO touches aren't always on the main thread. So when we receive touches through TUIO, we want to hold onto the functions in the mRenderFunctionQueue, 
	//and on each update we will call all of the functions in that deque
	void					mainThreadTouchesBegan(const Touch & touch, views::BaseViewRef rootView);
	void					mainThreadTouchesMoved(const Touch & touch, views::BaseViewRef rootView);
	void					mainThreadTouchesEnded(const Touch & touch, views::BaseViewRef rootView, const bool canceled = false);

	views::TouchViewRef		getViewForTouchId(const int touchId);

	//! Find the object that the current touch is hitting by navigating up the view hierarchy
	views::TouchView*		getTopViewForTouch(const Touch & touch, views::BaseViewRef rootView);
	
	//! This enables/disables MultiTouch on a global app level where setMultiTouchEnabled in BaseView is on a view by view basis.
	void					setMultiTouchEnabled(bool enabled) { mMultiTouchEnabled = enabled; };
	bool					isMultiTouchEnabled(){ return mMultiTouchEnabled; };

	//==================================================

	TouchSignal									mSignalTouchBegan;	//! Fired before any touch objects receive this event
	TouchSignal									mSignalTouchMoved;	//! Fired before any touch objects receive this event
	TouchSignal									mSignalTouchEnded;	//! Fired before any touch objects receive this event

	std::recursive_mutex						mTouchIdMutex;
	std::map<int, views::TouchViewWeakRef>		mViewsByTouchId;	// Stores views that are currently being touched
	std::map<int, Touch>						mTouchesById;		// Whatever the latest event is for a touch id

	std::recursive_mutex						mQueueMutex;
	std::deque<Touch>							mTouchQueue;		 // Collects all touch events until they're processed on the main thread

	bool										mDiscardMissedTouches;
	bool										mMultiTouchEnabled;
	float                                       mLatestTouchTime;

	ci::mat4									mAppTransform;
	ci::vec2									mAppSize;

	std::vector<TouchManagerPluginRef>			mPlugins;
};

}
}
