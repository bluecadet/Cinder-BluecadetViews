
#include "TuioDriver.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace touch {
namespace drivers {

TuioDriver::TuioDriver() :
    mOscReceiver(tuio::Receiver::DEFAULT_TUIO_PORT),
    mTuio(app::getWindow(), &mOscReceiver),
    mTouchManager(touch::TouchManager::getInstance()),
    mWindow(getWindow())
{
}

TuioDriver::~TuioDriver() {
	disconnect();
}

void TuioDriver::connect() {
    try {
        mOscReceiver.bind();
    } catch( const ci::Exception & e ) {
        CI_LOG_EXCEPTION("OscReceiver bind", e);
        return;
    }
    
    mOscReceiver.listen([] (asio::error_code ec, asio::ip::udp::endpoint ep) -> bool {
        if (ec) {
            CI_LOG_E("Error on listener: " << ec.message() << " Error Value: " << ec.value());
            return false;
        } else {
            return true;
        }
    });
    
	// Callbacks for touches
	mTuio.setAddedFn<tuio::Cursor2d>(bind(&TuioDriver::touchBegan, this, placeholders::_1));
	mTuio.setUpdatedFn<tuio::Cursor2d>(bind(&TuioDriver::touchMoved, this, placeholders::_1));
	mTuio.setRemovedFn<tuio::Cursor2d>(bind(&TuioDriver::touchEnded, this, placeholders::_1));

	// Callbacks for marker objects
	/*
	//! TODO: @SM Complete when we have objects for testing

	mTuioReceiver->setAddedFn<tuio::Object2d>(bind(&TuioDriver::objectBegan, this, placeholders::_1));
	mTuioReceiver->setUpdatedFn<tuio::Object2d>(bind(&TuioDriver::objectMoved, this, placeholders::_1));
	mTuioReceiver->setRemovedFn<tuio::Object2d>(bind(&TuioDriver::objectEnded, this, placeholders::_1));
	*/
}

void TuioDriver::disconnect() {
	try {
        mOscReceiver.close();
	} catch (...) {
		cout << "TuioDriver: Couldn't disconnect TuioDriver From the TouchManager. TouchManager may have already been deleted." << endl;
	}
	mTouchManager = nullptr;
}


void TuioDriver::touchBegan(const tuio::Cursor2d &cursor) {
	if (mTouchManager) {
		ci::app::TouchEvent::Touch touch = cursor.convertToTouch(mWindow);
		mTouchManager->addTouch(touch.getId(), touch.getPos(), TouchType::Touch, TouchPhase::Began);
	}
}

void TuioDriver::touchMoved(const tuio::Cursor2d &cursor) {
	if (mTouchManager) {
		ci::app::TouchEvent::Touch touch = cursor.convertToTouch(mWindow);
		mTouchManager->addTouch(touch.getId(), touch.getPos(), TouchType::Touch, TouchPhase::Moved);
	}
}

void TuioDriver::touchEnded(const tuio::Cursor2d &cursor) {
	if (mTouchManager) {
		ci::app::TouchEvent::Touch touch = cursor.convertToTouch(mWindow);
		mTouchManager->addTouch(touch.getId(), touch.getPos(), TouchType::Touch, TouchPhase::Ended);
	}
}



/*
//! TODO: @SM Complete when we have objects for testing

void TuioDriver::objectBegan(const tuio::Object2d &object) {
	if (mTouchManagerPntr) {
	}
}

void TuioDriver::objectMoved(const tuio::Object2d &object) {
	if (mTouchManagerPntr) {
	}
}

void TuioDriver::objectEnded(const tuio::Object2d &object) {
	if (mTouchManagerPntr) {
	}
}
*/


}
}
}
