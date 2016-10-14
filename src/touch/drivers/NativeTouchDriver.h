//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//	Developers: Paul Rudolph & Stacey Martens
//  Contents: Code for native touch to be used with the touch manager class
//  Comments: This driver was based off of Cinder's multitouchbasic sample 
//----------------------------------------------------------------------------

#pragma once
#include "cinder/app/App.h"
#include "cinder/System.h"

#include "../TouchManager.h"

namespace bluecadet {
namespace touch {
namespace drivers {

class NativeTouchDriver {

public:
	NativeTouchDriver();
	~NativeTouchDriver();
	void connect();
	void disconnect();

private:
	// Native touch event signals
	ci::signals::Connection			mTouchBeganConnection;
	ci::signals::Connection			mTouchMovedConnection;
	ci::signals::Connection			mTouchEndConnection;

	void nativeTouchBegan(const cinder::app::TouchEvent &event);
	void nativeTouchMoved(const cinder::app::TouchEvent &event);
	void nativeTouchEnded(const cinder::app::TouchEvent &event);

	TouchManagerRef		mTouchManager;
};

}
}
}
