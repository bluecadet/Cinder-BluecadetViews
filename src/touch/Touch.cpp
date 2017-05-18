#include "Touch.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace touch {

Touch::Touch(const int id, const ci::ivec2 windowPosition, const TouchType type, const TouchPhase phase) :
	id(id),
	windowPosition(windowPosition),
	appPosition(windowPosition),
	type(type),
	phase(phase) {
}


const string TouchEvent::Type::TOUCH = "touch";

TouchEvent::TouchEvent(const Touch & touch)	:
	TouchEvent(touch.id, touch.windowPosition, touch.appPosition, touch.type, touch.phase)
{
}

TouchEvent::TouchEvent(const int touchId, const ci::ivec2 & windowPosition, const ci::vec2 & globalPosition, const TouchType touchType, const TouchPhase touchPhase) :
	views::ViewEvent("touch", nullptr),
	touchId(touchId),
	windowPosition(windowPosition),
	globalPosition(globalPosition),
	localPosition(globalPosition),
	touchPhase(touchPhase),
	touchType(touchType) {
}

}
}
