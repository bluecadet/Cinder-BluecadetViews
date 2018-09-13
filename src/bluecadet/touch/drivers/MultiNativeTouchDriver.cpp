#include "MultiNativeTouchDriver.h"

using namespace ci::app;

namespace bluecadet {
namespace touch {
namespace drivers {

ci::signals::Signal<void(ci::vec2, int)>  signalOnTouchAdded;
ci::signals::Signal<void(ci::vec2, int)>& getSignalOnTouchAdded() { return signalOnTouchAdded; }
ci::signals::Signal<void(ci::vec2, int)>  signalOnTouchUpdated;
ci::signals::Signal<void(ci::vec2, int)>& getSignalOnTouchUpdated() { return signalOnTouchUpdated; }
ci::signals::Signal<void(ci::vec2, int)>  signalOnTouchRemoved;
ci::signals::Signal<void(ci::vec2, int)>& getSignalOnTouchRemoved() { return signalOnTouchRemoved; }

GET_POINTER_INFO			GetPointerInfo;
GET_POINTER_TOUCH_INFO		GetPointerTouchInfo;
GET_POINTER_PEN_INFO		GetPointerPenInfo;

HWND						_currentWindow;
LONG_PTR					_oldWindowProc;

extern "C"
{
	void __stdcall initTouch() {
		_currentWindow = (HWND)ci::app::getWindow()->getNative();

		HINSTANCE h = LoadLibrary(TEXT("user32.dll"));
		GetPointerInfo = (GET_POINTER_INFO)GetProcAddress(h, "GetPointerInfo");
		GetPointerTouchInfo = (GET_POINTER_TOUCH_INFO)GetProcAddress(h, "GetPointerTouchInfo");
		GetPointerPenInfo = (GET_POINTER_PEN_INFO)GetProcAddress(h, "GetPointerPenInfo");

		_oldWindowProc = SetWindowLongPtr(_currentWindow, GWLP_WNDPROC, (LONG_PTR)wndProc8);
	}
}

LRESULT CALLBACK wndProc8(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_TOUCH:
		CloseTouchInputHandle((HTOUCHINPUT)lParam);
		break;
	case WM_POINTERENTER:
	case WM_POINTERLEAVE:
	case WM_POINTERDOWN:
	case WM_POINTERUP:
	case WM_POINTERUPDATE:
	case WM_POINTERCAPTURECHANGED:
		decodeWin8Touches(msg, wParam, lParam);
		break;
	default:
		return CallWindowProc((WNDPROC)_oldWindowProc, hwnd, msg, wParam, lParam);
	}
	return 0;
}

void decodeWin8Touches(UINT msg, WPARAM wParam, LPARAM lParam) {
	int pointerId = GET_POINTERID_WPARAM(wParam);

	POINTER_INFO pointerInfo;
	if (!GetPointerInfo(pointerId, &pointerInfo)) return;

	POINT p;
	p.x = pointerInfo.ptPixelLocation.x;
	p.y = pointerInfo.ptPixelLocation.y;
	ScreenToClient(_currentWindow, &p);

	ci::vec2 position(p.x, p.y);

	switch (msg) {
	case WM_TOUCH:
		//CI_LOG_I("WM_TOUCH");
		//not used
		break;
	case WM_POINTERENTER:
		//CI_LOG_I("WM_POINTERENTER");
		//redundant as followed by WM_POINTERDOWN with same coords
		break;
	case WM_POINTERLEAVE:
		//CI_LOG_I("WM_POINTERLEAVE");
		//redundant as follows WM_POINTERUP with same coords
		break;
	case WM_POINTERDOWN:
		//CI_LOG_I("WM_POINTERDOWN");
		signalOnTouchAdded.emit(position, pointerId);
		break;
	case WM_POINTERUP:
		//CI_LOG_I("WM_POINTERUP");
		signalOnTouchRemoved.emit(position, pointerId);
		break;
	case WM_POINTERUPDATE:
		//CI_LOG_I("WM_POINTERUPDATE");
		signalOnTouchUpdated.emit(position, pointerId);
		break;
	case WM_POINTERCAPTURECHANGED:
		//CI_LOG_I("WM_POINTERCAPTURECHANGED");
		//not used
		break;
	}

}

MultiNativeTouchDriver::MultiNativeTouchDriver() {
	mTouchManager = nullptr;
}

void MultiNativeTouchDriver::connect() {
	//set up touches from win 8 api
	initTouch();

	// Connect to the application window touch event signals
	mTouchBeganConnection = getSignalOnTouchAdded().connect(std::bind(&MultiNativeTouchDriver::nativeTouchBegan, this, std::placeholders::_1, std::placeholders::_2));
	mTouchMovedConnection = getSignalOnTouchUpdated().connect(std::bind(&MultiNativeTouchDriver::nativeTouchMoved, this, std::placeholders::_1, std::placeholders::_2));
	mTouchEndConnection = getSignalOnTouchRemoved().connect(std::bind(&MultiNativeTouchDriver::nativeTouchEnded, this, std::placeholders::_1, std::placeholders::_2));

	// Shared pointer to the Touch Manager
	mTouchManager = TouchManager::getInstance();
}

MultiNativeTouchDriver::~MultiNativeTouchDriver() {
	// Disconnect from the mouse signals
	mTouchBeganConnection.disconnect();
	mTouchMovedConnection.disconnect();
	mTouchEndConnection.disconnect();

	// Remove the pointer to the touch manager
	mTouchManager = nullptr;
}

void MultiNativeTouchDriver::disconnect() {
	// Disconnect from the mouse signals
	mTouchBeganConnection.disconnect();
	mTouchMovedConnection.disconnect();
	mTouchEndConnection.disconnect();

	// Remove the pointer to the touch manager
	mTouchManager = nullptr;
}

void MultiNativeTouchDriver::nativeTouchBegan(const ci::vec2 pos, const int id) {
	if (mTouchManager) {
		mTouchManager->addTouch(id, pos, TouchType::Touch, TouchPhase::Began);
	}
}

void MultiNativeTouchDriver::nativeTouchMoved(const ci::vec2 pos, const int id) {
	if (mTouchManager) {
		mTouchManager->addTouch(id, pos, TouchType::Touch, TouchPhase::Moved);
	}
}

void MultiNativeTouchDriver::nativeTouchEnded(const ci::vec2 pos, const int id) {
	if (mTouchManager) {
		mTouchManager->addTouch(id, pos, TouchType::Touch, TouchPhase::Ended);
	}
}

}
}
}