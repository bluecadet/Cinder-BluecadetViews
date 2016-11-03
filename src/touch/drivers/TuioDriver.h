//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016  
//	Developers: Stacey Martens
//  Contents: 
//  Comments:
//----------------------------------------------------------------------------

#pragma once
#include "cinder/app/App.h"
#include "cinder/tuio/Tuio.h"
#include "cinder/osc/Osc.h"
#include "cinder/Signals.h"

#include "../TouchManager.h"

namespace bluecadet {
namespace touch {
namespace drivers {

class TuioDriver {
public:
	TuioDriver();
	~TuioDriver();

	void connect();
	void disconnect();

protected:

	//! Callback functions
	void touchBegan(const ci::tuio::Cursor2d &cursor);
	void touchMoved(const ci::tuio::Cursor2d &cursor);
	void touchEnded(const ci::tuio::Cursor2d &cursor);
	
	void objectBegan(const ci::tuio::Object2d &object);
	void objectMoved(const ci::tuio::Object2d &object);
	void objectEnded(const ci::tuio::Object2d &object);
	
	void updateViewport();
	
    ci::osc::ReceiverUdp mOscReceiver;
    ci::tuio::Receiver mTuio;
	TouchManagerRef	mTouchManager;
	
	ci::vec2 mDisplaySize;
	ci::vec2 mWindowPos;
	std::vector<ci::signals::ScopedConnection> mSignalConnections;
};
    
}
}
}

