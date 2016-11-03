
#include "TuioDriver.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace touch {
namespace drivers {

TuioDriver::TuioDriver() :
    mOscReceiver(tuio::Receiver::DEFAULT_TUIO_PORT),
	mWindowPos(getWindowPos()),
	mDisplaySize(getWindow()->getDisplay()->getSize()),
	mTuio(getWindow(), &mOscReceiver),
    mTouchManager(touch::TouchManager::getInstance())
{
}

TuioDriver::~TuioDriver() {
	disconnect();
}

void TuioDriver::connect() {
	mSignalConnections.push_back(getWindow()->getSignalDisplayChange().connect(bind(&TuioDriver::updateViewport, this)));
	mSignalConnections.push_back(getWindow()->getSignalMove().connect(bind(&TuioDriver::updateViewport, this)));
	mSignalConnections.push_back(getWindow()->getSignalResize().connect(bind(&TuioDriver::updateViewport, this)));
	
	updateViewport();
	
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
	
	// Remove existing tuio listeners from ci::App
	mTuio.clear<tuio::Cursor2d>();
	mTuio.clear<tuio::Object2d>();
	
	// Add custom tuio listeners
	mTuio.setAddedFn<tuio::Cursor2d>(bind(&TuioDriver::touchBegan, this, placeholders::_1));
	mTuio.setUpdatedFn<tuio::Cursor2d>(bind(&TuioDriver::touchMoved, this, placeholders::_1));
	mTuio.setRemovedFn<tuio::Cursor2d>(bind(&TuioDriver::touchEnded, this, placeholders::_1));
	
	mTuio.setAddedFn<tuio::Object2d>(bind(&TuioDriver::objectBegan, this, placeholders::_1));
	mTuio.setUpdatedFn<tuio::Object2d>(bind(&TuioDriver::objectMoved, this, placeholders::_1));
	mTuio.setRemovedFn<tuio::Object2d>(bind(&TuioDriver::objectEnded, this, placeholders::_1));
}

void TuioDriver::disconnect() {
	mSignalConnections.clear();
	
	try {
        mOscReceiver.close();
	} catch (...) {
		cout << "TuioDriver: Couldn't disconnect TuioDriver From the TouchManager. TouchManager may have already been deleted." << endl;
	}
	mTouchManager = nullptr;
}

void TuioDriver::updateViewport() {
	auto display = getWindow()->getDisplay();
	mWindowPos = vec2(toPixels(getWindowPos() - display->getBounds().getUL()));
	mDisplaySize = vec2(toPixels(display->getSize()));
}

void TuioDriver::touchBegan(const tuio::Cursor2d &cursor) {
	if (mTouchManager) {
		vec2 pos = vec2(cursor.getPosition()) * mDisplaySize - mWindowPos;
		mTouchManager->addTouch(cursor.getSessionId(), pos, TouchType::Touch, TouchPhase::Began);
	}
}

void TuioDriver::touchMoved(const tuio::Cursor2d &cursor) {
	if (mTouchManager) {
		vec2 pos = vec2(cursor.getPosition()) * mDisplaySize - mWindowPos;
		mTouchManager->addTouch(cursor.getSessionId(), pos, TouchType::Touch, TouchPhase::Moved);
	}
}

void TuioDriver::touchEnded(const tuio::Cursor2d &cursor) {
	if (mTouchManager) {
		vec2 pos = vec2(cursor.getPosition()) * mDisplaySize - mWindowPos;
		mTouchManager->addTouch(cursor.getSessionId(), pos, TouchType::Touch, TouchPhase::Ended);
	}
}

void TuioDriver::objectBegan(const tuio::Object2d &object) {
	if (mTouchManager) {
		vec2 pos = vec2(object.getPosition()) * mDisplaySize - mWindowPos;
		mTouchManager->addTouch(object.getSessionId(), pos, TouchType::Fiducial, TouchPhase::Began);
	}
}

void TuioDriver::objectMoved(const tuio::Object2d &object) {
	if (mTouchManager) {
		vec2 pos = vec2(object.getPosition()) * mDisplaySize - mWindowPos;
		mTouchManager->addTouch(object.getSessionId(), pos, TouchType::Fiducial, TouchPhase::Moved);
	}
}

void TuioDriver::objectEnded(const tuio::Object2d &object) {
	if (mTouchManager) {
		vec2 pos = vec2(object.getPosition()) * mDisplaySize - mWindowPos;
		mTouchManager->addTouch(object.getSessionId(), pos, TouchType::Fiducial, TouchPhase::Ended);
	}
}



}
}
}
