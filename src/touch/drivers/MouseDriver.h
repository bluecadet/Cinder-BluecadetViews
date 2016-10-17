//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2014
//	Developers: Paul Rudolph & Stacey Martens
//  Contents: Code for mouse driver to be used with the touch manager class
//  Comments: This mouse driver was based off of Cinder's listenerBasic sample 
//----------------------------------------------------------------------------

#pragma once
#include "cinder/app/App.h"
#include "../TouchManager.h"

namespace bluecadet {
namespace touch {
namespace drivers {

class MouseDriver {

public:
	MouseDriver();
	~MouseDriver();

	void connect();
	void disconnect();

	//! Simulate an additional touche by pressing Alt before clicking.
	//! Move the cursor to change the offset of both touches and hold
	//! Shift to move the center position. Defaults to true.
	bool getVirualMultiTouchEnabled() const { return mVirtualMultiTouchEnabled; }
	void setVirtualMultiTouchEnabled(const bool value) { mVirtualMultiTouchEnabled = value; }

	const ci::ivec2 & getMousePos() const { return mMousePos; };
	const ci::ivec2 & getPrevMousePos() const { return mPrevMousePos; };

protected:
	//! Signals
	ci::signals::Connection mMouseBeganConnection;
	ci::signals::Connection mMouseDraggedConnection;
	ci::signals::Connection mMouseMovedConnection;
	ci::signals::Connection mMouseEndConnection;
	ci::signals::Connection mKeyDownConnection;
	ci::signals::Connection mKeyUpConnection;
	ci::signals::Connection mUpdateConnection;

	void handleMouseBegan(const ci::app::MouseEvent &event);
	void handleMouseMoved(const ci::app::MouseEvent &event);
	void handleMouseDragged(const ci::app::MouseEvent &event);
	void handleMouseEnded(const ci::app::MouseEvent &event);

	void handleKeyDown(const ci::app::KeyEvent &event);
	void handleKeyUp(const ci::app::KeyEvent &event);
	void handleUpdate();

	void updateSimulatedTouches();

	void showVirtualTouches();
	void hideVirtualTouches();
	void updateVirtualTouches();
	void commitVirtualTouches();

	TouchManagerRef	mTouchManager;
	int				mTouchId;
	bool			mVirtualMultiTouchEnabled;
	bool			mIsSimulatingMultiTouch;
	ci::ivec2		mVirtualMultiTouchInitial;
	ci::ivec2		mVirtualMultiTouchCenter;
	ci::ivec2		mVirtualMultiTouchOffset;

	bool			mIsControlDown;
	bool			mIsShiftDown;
	bool			mIsMouseDown;
	bool			mDidMoveMouse;
	ci::ivec2		mPrevMousePos;
	ci::ivec2		mMousePos;

	bool			mShowVirtualTouches;
	Touch			mVirtualTouchA;
	Touch			mVirtualTouchB;

};

}
}
}