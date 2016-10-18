#include "ViewEvent.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

const string ViewEvent::Type::UPDATED = "updated";
const string ViewEvent::Type::GENERIC = "generic";

void ViewEvent::stopPropagation() {
	shouldPropagate = false;
}

ViewEvent::ViewEvent(const std::string & type, BaseView * target) :
	type(type),
	target(target) {
}

}
}
