#pragma once

#include "Touch.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace touch {

const string TouchEvent::Type::TOUCH = "touch";

TouchEvent::TouchEvent(const int touchId, const ci::vec2& position, const TouchType touchType, const TouchPhase touchPhase) :
	views::Event("touch", nullptr),
	touchId(touchId),
	position(position),
	windowPosition(position),
	localPosition(position),
	touchPhase(touchPhase)
{}

}
}
