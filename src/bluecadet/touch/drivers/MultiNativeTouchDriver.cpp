#include "MultiNativeTouchDriver.h"

using namespace ci::app;

namespace bluecadet {
namespace touch {
namespace drivers {

// <Windows 8 touch API>

#define WM_POINTERENTER 0x0249
#define WM_POINTERLEAVE 0x024A
#define WM_POINTERUPDATE 0x0245
#define WM_POINTERDOWN 0x0246
#define WM_POINTERUP 0x0247
#define WM_POINTERCAPTURECHANGED 0x024C
#define POINTER_CANCELLED 0x1000

#define GET_POINTERID_WPARAM(wParam) (LOWORD(wParam))

typedef enum {
	PT_POINTER = 0x00000001,
	PT_TOUCH = 0x00000002,
	PT_PEN = 0x00000003,
	PT_MOUSE = 0x00000004,
	PT_TOUCHPAD = 0x00000005
} POINTER_INPUT_TYPE;

typedef enum {
	POINTER_FLAG_NONE = 0x00000000,
	POINTER_FLAG_NEW = 0x00000001,
	POINTER_FLAG_INRANGE = 0x00000002,
	POINTER_FLAG_INCONTACT = 0x00000004,
	POINTER_FLAG_FIRSTBUTTON = 0x00000010,
	POINTER_FLAG_SECONDBUTTON = 0x00000020,
	POINTER_FLAG_THIRDBUTTON = 0x00000040,
	POINTER_FLAG_FOURTHBUTTON = 0x00000080,
	POINTER_FLAG_FIFTHBUTTON = 0x00000100,
	POINTER_FLAG_PRIMARY = 0x00002000,
	POINTER_FLAG_CONFIDENCE = 0x00004000,
	POINTER_FLAG_CANCELED = 0x00008000,
	POINTER_FLAG_DOWN = 0x00010000,
	POINTER_FLAG_UPDATE = 0x00020000,
	POINTER_FLAG_UP = 0x00040000,
	POINTER_FLAG_WHEEL = 0x00080000,
	POINTER_FLAG_HWHEEL = 0x00100000,
	POINTER_FLAG_CAPTURECHANGED = 0x00200000,
	POINTER_FLAG_HASTRANSFORM = 0x00400000
} POINTER_FLAGS;

typedef enum {
	POINTER_CHANGE_NONE,
	POINTER_CHANGE_FIRSTBUTTON_DOWN,
	POINTER_CHANGE_FIRSTBUTTON_UP,
	POINTER_CHANGE_SECONDBUTTON_DOWN,
	POINTER_CHANGE_SECONDBUTTON_UP,
	POINTER_CHANGE_THIRDBUTTON_DOWN,
	POINTER_CHANGE_THIRDBUTTON_UP,
	POINTER_CHANGE_FOURTHBUTTON_DOWN,
	POINTER_CHANGE_FOURTHBUTTON_UP,
	POINTER_CHANGE_FIFTHBUTTON_DOWN,
	POINTER_CHANGE_FIFTHBUTTON_UP,
} POINTER_BUTTON_CHANGE_TYPE;

typedef enum { TOUCH_FLAG_NONE = 0x00000000 } TOUCH_FLAGS;

typedef enum {
	TOUCH_MASK_NONE = 0x00000000,
	TOUCH_MASK_CONTACTAREA = 0x00000001,
	TOUCH_MASK_ORIENTATION = 0x00000002,
	TOUCH_MASK_PRESSURE = 0x00000004
} TOUCH_MASK;

typedef enum {
	PEN_FLAG_NONE = 0x00000000,
	PEN_FLAG_BARREL = 0x00000001,
	PEN_FLAG_INVERTED = 0x00000002,
	PEN_FLAG_ERASER = 0x00000004
} PEN_FLAGS;

typedef enum {
	PEN_MASK_NONE = 0x00000000,
	PEN_MASK_PRESSURE = 0x00000001,
	PEN_MASK_ROTATION = 0x00000002,
	PEN_MASK_TILT_X = 0x00000004,
	PEN_MASK_TILT_Y = 0x00000008
} PEN_MASK;

typedef struct {
	POINTER_INPUT_TYPE pointerType;
	UINT32 pointerId;
	UINT32 frameId;
	POINTER_FLAGS pointerFlags;
	HANDLE sourceDevice;
	HWND hwndTarget;
	POINT ptPixelLocation;
	POINT ptHimetricLocation;
	POINT ptPixelLocationRaw;
	POINT ptHimetricLocationRaw;
	DWORD dwTime;
	UINT32 historyCount;
	INT32 InputData;
	DWORD dwKeyStates;
	UINT64 PerformanceCount;
	POINTER_BUTTON_CHANGE_TYPE ButtonChangeType;
} POINTER_INFO;

typedef struct {
	POINTER_INFO pointerInfo;
	TOUCH_FLAGS touchFlags;
	TOUCH_MASK touchMask;
	RECT rcContact;
	RECT rcContactRaw;
	UINT32 orientation;
	UINT32 pressure;
} POINTER_TOUCH_INFO;

typedef struct {
	POINTER_INFO pointerInfo;
	PEN_FLAGS penFlags;
	PEN_MASK penMask;
	UINT32 pressure;
	UINT32 rotation;
	INT32 tiltX;
	INT32 tiltY;
} POINTER_PEN_INFO;

typedef BOOL(WINAPI * GET_POINTER_INFO)(UINT32 pointerId, POINTER_INFO * pointerInfo);
typedef BOOL(WINAPI * GET_POINTER_TOUCH_INFO)(UINT32 pointerId, POINTER_TOUCH_INFO * pointerInfo);
typedef BOOL(WINAPI * GET_POINTER_PEN_INFO)(UINT32 pointerId, POINTER_PEN_INFO * pointerInfo);

// </Windows 8 touch API>

#define EXPORT_API __declspec(dllexport)

extern "C" {
EXPORT_API void __stdcall initTouch();
}

LRESULT CALLBACK wndProc8(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void decodeWin8Touches(UINT msg, WPARAM wParam, LPARAM lParam);

ci::signals::Signal<void(ci::vec2, int)> signalOnTouchAdded;
ci::signals::Signal<void(ci::vec2, int)> signalOnTouchUpdated;
ci::signals::Signal<void(ci::vec2, int)> signalOnTouchRemoved;

GET_POINTER_INFO GetPointerInfo;
GET_POINTER_TOUCH_INFO GetPointerTouchInfo;
GET_POINTER_PEN_INFO GetPointerPenInfo;

HWND _currentWindow;
LONG_PTR _oldWindowProc;

extern "C" {
void __stdcall initTouch() {
	static bool initialized = false;
	if (initialized) {
		return;
	}
	initialized = true;

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
		case WM_TOUCH: CloseTouchInputHandle((HTOUCHINPUT)lParam); break;
		case WM_POINTERENTER:
		case WM_POINTERLEAVE:
		case WM_POINTERDOWN:
		case WM_POINTERUP:
		case WM_POINTERUPDATE:
		case WM_POINTERCAPTURECHANGED: decodeWin8Touches(msg, wParam, lParam); break;
		default: return CallWindowProc((WNDPROC)_oldWindowProc, hwnd, msg, wParam, lParam);
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
			// CI_LOG_I("WM_TOUCH");
			// not used
			break;
		case WM_POINTERENTER:
			// CI_LOG_I("WM_POINTERENTER");
			// redundant as followed by WM_POINTERDOWN with same coords
			break;
		case WM_POINTERLEAVE:
			// CI_LOG_I("WM_POINTERLEAVE");
			// redundant as follows WM_POINTERUP with same coords
			break;
		case WM_POINTERDOWN:
			// CI_LOG_I("WM_POINTERDOWN");
			signalOnTouchAdded.emit(position, pointerId);
			break;
		case WM_POINTERUP:
			// CI_LOG_I("WM_POINTERUP");
			signalOnTouchRemoved.emit(position, pointerId);
			break;
		case WM_POINTERUPDATE:
			// CI_LOG_I("WM_POINTERUPDATE");
			signalOnTouchUpdated.emit(position, pointerId);
			break;
		case WM_POINTERCAPTURECHANGED:
			// CI_LOG_I("WM_POINTERCAPTURECHANGED");
			// not used
			break;
	}
}

MultiNativeTouchDriver::MultiNativeTouchDriver() {
	mTouchManager = nullptr;
}

void MultiNativeTouchDriver::connect() {
	// set up touches from win 8 api
	initTouch();

	// Connect to the application window touch event signals
	mTouchBeganConnection = signalOnTouchAdded.connect(
		std::bind(&MultiNativeTouchDriver::nativeTouchBegan, this, std::placeholders::_1, std::placeholders::_2));
	mTouchMovedConnection = signalOnTouchUpdated.connect(
		std::bind(&MultiNativeTouchDriver::nativeTouchMoved, this, std::placeholders::_1, std::placeholders::_2));
	mTouchEndConnection = signalOnTouchRemoved.connect(
		std::bind(&MultiNativeTouchDriver::nativeTouchEnded, this, std::placeholders::_1, std::placeholders::_2));

	// Shared pointer to the Touch Manager
	mTouchManager = TouchManager::getInstance();
}

MultiNativeTouchDriver::~MultiNativeTouchDriver() {
	disconnect();
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

}  // namespace drivers
}  // namespace touch
}  // namespace bluecadet