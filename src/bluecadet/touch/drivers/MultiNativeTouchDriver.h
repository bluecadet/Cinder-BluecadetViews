// Thanks to code from TouchScript by Valentin Simonov / http://va.lent.in/
// https://github.com/TouchScript/TouchScript/tree/master/External/WindowsTouch

#pragma once

#include <Windows.h>

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"
#include "cinder/Signals.h"
#include "cinder/Log.h"

#include "../TouchManager.h"

namespace bluecadet {
namespace touch {
namespace drivers {

class MultiNativeTouchDriver {
public:
	MultiNativeTouchDriver();
	~MultiNativeTouchDriver();
	void connect();
	void disconnect();

private:
	void nativeTouchBegan(const ci::vec2 pos, const int id);
	void nativeTouchMoved(const ci::vec2 pos, const int id);
	void nativeTouchEnded(const ci::vec2 pos, const int id);

	bluecadet::touch::TouchManagerRef	mTouchManager;

	// Native touch event signals
	ci::signals::Connection				mTouchBeganConnection;
	ci::signals::Connection				mTouchMovedConnection;
	ci::signals::Connection				mTouchEndConnection;
};

}
}
}