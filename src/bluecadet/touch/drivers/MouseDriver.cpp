#include "MouseDriver.h"

using namespace std;
using namespace ci;
using namespace ci::app;

namespace bluecadet {
namespace touch {
namespace drivers {

MouseDriver::MouseDriver() :
	mTouchManager(nullptr),
	mVirtualMultiTouchEnabled(true),
	mShowVirtualTouches(false),
	mIsSimulatingMultiTouch(false),
	mIsControlDown(false),
	mIsShiftDown(false),
	mIsMouseDown(false),
	mDidMoveMouse(false),
	mMousePos(0),
	mPrevMousePos(0),
	mTouchId(-1),
	mVirtualTouchA(-3, vec2(), TouchType::Mouse, TouchPhase::Began),
	mVirtualTouchB(-4, vec2(), TouchType::Mouse, TouchPhase::Began) {
	mVirtualTouchA.isVirtual = true;
	mVirtualTouchB.isVirtual = true;
}

MouseDriver::~MouseDriver() {
	disconnect();
}

void MouseDriver::connect() {
	auto window = getWindow();

	mMouseBeganConnection = window->getSignalMouseDown().connect(bind(&MouseDriver::handleMouseBegan, this, placeholders::_1));
	mMouseMovedConnection = window->getSignalMouseMove().connect(bind(&MouseDriver::handleMouseMoved, this, placeholders::_1));
	mMouseDraggedConnection = window->getSignalMouseDrag().connect(bind(&MouseDriver::handleMouseDragged, this, placeholders::_1));
	mMouseEndConnection = window->getSignalMouseUp().connect(bind(&MouseDriver::handleMouseEnded, this, placeholders::_1));

	mKeyDownConnection = window->getSignalKeyDown().connect(bind(&MouseDriver::handleKeyDown, this, placeholders::_1));
	mKeyUpConnection = window->getSignalKeyUp().connect(bind(&MouseDriver::handleKeyUp, this, placeholders::_1));

	mUpdateConnection = App::get()->getSignalUpdate().connect(-1, bind(&MouseDriver::handleUpdate, this));

	mTouchManager = TouchManager::getInstance();
}

void MouseDriver::disconnect() {
	mMouseBeganConnection.disconnect();
	mMouseMovedConnection.disconnect();
	mMouseEndConnection.disconnect();
	mMouseDraggedConnection.disconnect();
	mTouchManager = nullptr;
}

void MouseDriver::handleKeyDown(const ci::app::KeyEvent & event) {
	const bool controlWasDown = mIsControlDown;
	mIsControlDown = event.isControlDown();
	mIsShiftDown = event.isShiftDown();

	if (!controlWasDown && mIsMouseDown) {
		return;
	}

	if (mIsControlDown && !controlWasDown && !mIsSimulatingMultiTouch && mVirtualMultiTouchEnabled) {
		mVirtualMultiTouchInitial = mMousePos;
		mVirtualMultiTouchCenter = mMousePos;
		mVirtualMultiTouchOffset = ivec2(0);
		showVirtualTouches();
	}
}

void MouseDriver::handleKeyUp(const ci::app::KeyEvent & event) {
	const bool controlWasDown = mIsControlDown;
	mIsControlDown = event.isControlDown();
	mIsShiftDown = event.isShiftDown();

	if (mShowVirtualTouches && !mIsControlDown && controlWasDown) {
		hideVirtualTouches();
	}
}

void MouseDriver::handleUpdate() {
	const bool mouseMoved = mPrevMousePos != mMousePos;

	if (!mIsSimulatingMultiTouch && mIsMouseDown && (mouseMoved || mDidMoveMouse)) {
		mTouchManager->addTouch(mTouchId, mMousePos, TouchType::Mouse, TouchPhase::Moved);

	} else if (mIsSimulatingMultiTouch && (mouseMoved || mDidMoveMouse)) {
		mTouchManager->addTouch(mTouchId - 1, mVirtualMultiTouchCenter + mVirtualMultiTouchOffset, TouchType::Mouse, TouchPhase::Moved);
		mTouchManager->addTouch(mTouchId - 2, mVirtualMultiTouchCenter - mVirtualMultiTouchOffset, TouchType::Mouse, TouchPhase::Moved);

	} else if (mShowVirtualTouches) {
		updateVirtualTouches();
	}

	mPrevMousePos = mMousePos;
	mDidMoveMouse = mouseMoved;
}

void MouseDriver::handleMouseBegan(const cinder::app::MouseEvent &event) {
	mIsMouseDown = true;
	mMousePos = event.getPos();

	if (!mTouchManager) return;

	mShowVirtualTouches = false;
	hideVirtualTouches();

	if (mVirtualMultiTouchEnabled && mIsControlDown) {
		mIsSimulatingMultiTouch = true;
		updateSimulatedTouches();
		mTouchManager->addTouch(mTouchId - 1, mVirtualMultiTouchCenter + mVirtualMultiTouchOffset, TouchType::Mouse, TouchPhase::Began);
		mTouchManager->addTouch(mTouchId - 2, mVirtualMultiTouchCenter - mVirtualMultiTouchOffset, TouchType::Mouse, TouchPhase::Began);
	} else {
		mTouchManager->addTouch(mTouchId, event.getPos(), TouchType::Mouse, TouchPhase::Began);
	}
}

void MouseDriver::handleMouseMoved(const cinder::app::MouseEvent &event) {
	mMousePos = event.getPos();

	if (!mTouchManager) return;

	if (mIsMouseDown || !mIsControlDown) {
		mShowVirtualTouches = false;
	}

	if (mIsSimulatingMultiTouch || mShowVirtualTouches) {
		updateSimulatedTouches();
	}
}

void MouseDriver::handleMouseDragged(const cinder::app::MouseEvent &event) {
	mMousePos = event.getPos();

	if (!mTouchManager) return;

	if (mIsSimulatingMultiTouch || mShowVirtualTouches) {
		updateSimulatedTouches();
	}
}

void MouseDriver::handleMouseEnded(const  cinder::app::MouseEvent &event) {
	mPrevMousePos = mMousePos;
	mMousePos = event.getPos();

	if (!mTouchManager) return;

	if (mIsSimulatingMultiTouch) {
		mTouchManager->addTouch(mTouchId - 1, mVirtualMultiTouchCenter + mVirtualMultiTouchOffset, TouchType::Mouse, TouchPhase::Ended);
		mTouchManager->addTouch(mTouchId - 2, mVirtualMultiTouchCenter - mVirtualMultiTouchOffset, TouchType::Mouse, TouchPhase::Ended);

	} else {
		mTouchManager->addTouch(mTouchId, event.getPos(), TouchType::Mouse, TouchPhase::Ended);
	}

	mIsSimulatingMultiTouch = false;
	mIsMouseDown = false;
}

void MouseDriver::updateSimulatedTouches() {
	if (mIsShiftDown) {
		// move center position
		ivec2 distance = mMousePos - mVirtualMultiTouchInitial;
		mVirtualMultiTouchCenter = mVirtualMultiTouchInitial + distance - mVirtualMultiTouchOffset;

	} else if (mIsControlDown || mIsSimulatingMultiTouch) {
		// change offset from center
		mVirtualMultiTouchOffset = mMousePos - mVirtualMultiTouchCenter;
	}
}

void MouseDriver::showVirtualTouches() {
	mShowVirtualTouches = true;
	mVirtualTouchA.phase = TouchPhase::Began;
	mVirtualTouchB.phase = TouchPhase::Began;
	commitVirtualTouches();
}

void MouseDriver::updateVirtualTouches() {
	mVirtualTouchA.phase = TouchPhase::Moved;
	mVirtualTouchB.phase = TouchPhase::Moved;
	commitVirtualTouches();
}

void MouseDriver::hideVirtualTouches() {
	mShowVirtualTouches = false;
	mVirtualTouchA.phase = TouchPhase::Ended;
	mVirtualTouchB.phase = TouchPhase::Ended;
	commitVirtualTouches();
}

void MouseDriver::commitVirtualTouches() {
	mVirtualTouchA.windowPosition = mVirtualMultiTouchCenter + mVirtualMultiTouchOffset;
	mVirtualTouchB.windowPosition = mVirtualMultiTouchCenter - mVirtualMultiTouchOffset;
	mTouchManager->addTouch(mVirtualTouchA);
	mTouchManager->addTouch(mVirtualTouchB);
}

}
}
}