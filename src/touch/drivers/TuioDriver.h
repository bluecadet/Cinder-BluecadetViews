//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016  
//	Developers: Stacey Martens
//  Contents: 
//  Comments:
//----------------------------------------------------------------------------

#pragma once
#include "cinder/app/App.h"
#include "Tuio.h"
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

private:

	//! Callback functions
	void touchBegan(const ci::tuio::Cursor2d &cursor);
	void touchMoved(const ci::tuio::Cursor2d &cursor);
	void touchEnded(const ci::tuio::Cursor2d &cursor);

	/*
	//! TODO: @SM Complete when we have objects for testing
	void objectBegan(const tuio::Object2d &object);
	void objectMoved(const tuio::Object2d &object);
	void objectEnded(const tuio::Object2d &object);
	*/

	std::shared_ptr<ci::tuio::Receiver> mTuioReceiver;
	ci::app::WindowRef mWindow;
	TouchManagerRef	mTouchManager;
};

}
}
}
