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

ci::signals::Signal< void(ci::vec2, int) > & getSignalOnTouchAdded();
ci::signals::Signal< void(ci::vec2, int) > & getSignalOnTouchUpdated();
ci::signals::Signal< void(ci::vec2, int) > & getSignalOnTouchRemoved();

// <Windows 8 touch API>

#define WM_POINTERENTER				0x0249
#define WM_POINTERLEAVE				0x024A
#define WM_POINTERUPDATE			0x0245
#define WM_POINTERDOWN				0x0246
#define WM_POINTERUP				0x0247
#define WM_POINTERCAPTURECHANGED    0x024C
#define POINTER_CANCELLED			0x1000

#define GET_POINTERID_WPARAM(wParam)	(LOWORD(wParam))

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

typedef enum {
	TOUCH_FLAG_NONE = 0x00000000
} TOUCH_FLAGS;

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
	POINTER_INPUT_TYPE		pointerType;
	UINT32					pointerId;
	UINT32					frameId;
	POINTER_FLAGS			pointerFlags;
	HANDLE					sourceDevice;
	HWND					hwndTarget;
	POINT					ptPixelLocation;
	POINT					ptHimetricLocation;
	POINT					ptPixelLocationRaw;
	POINT					ptHimetricLocationRaw;
	DWORD					dwTime;
	UINT32					historyCount;
	INT32					InputData;
	DWORD					dwKeyStates;
	UINT64					PerformanceCount;
	POINTER_BUTTON_CHANGE_TYPE ButtonChangeType;
} POINTER_INFO;

typedef struct {
	POINTER_INFO			pointerInfo;
	TOUCH_FLAGS				touchFlags;
	TOUCH_MASK				touchMask;
	RECT					rcContact;
	RECT					rcContactRaw;
	UINT32					orientation;
	UINT32					pressure;
} POINTER_TOUCH_INFO;

typedef struct {
	POINTER_INFO			pointerInfo;
	PEN_FLAGS				penFlags;
	PEN_MASK				penMask;
	UINT32					pressure;
	UINT32					rotation;
	INT32					tiltX;
	INT32					tiltY;
} POINTER_PEN_INFO;

typedef BOOL(WINAPI *GET_POINTER_INFO)(UINT32 pointerId, POINTER_INFO *pointerInfo);
typedef BOOL(WINAPI *GET_POINTER_TOUCH_INFO)(UINT32 pointerId, POINTER_TOUCH_INFO *pointerInfo);
typedef BOOL(WINAPI *GET_POINTER_PEN_INFO)(UINT32 pointerId, POINTER_PEN_INFO *pointerInfo);

// </Windows 8 touch API>

#define EXPORT_API __declspec(dllexport) 

extern "C"
{
	EXPORT_API void __stdcall initTouch();
}

LRESULT CALLBACK wndProc8(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void decodeWin8Touches(UINT msg, WPARAM wParam, LPARAM lParam);

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