#include "ViewEvent.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

const string ViewEvent::Type::CONTENT_INVALIDATED = "updated";

void ViewEvent::stopPropagation() {
	shouldPropagate = false;
}

ViewEvent::ViewEvent(const std::string & type, BaseViewRef target) :
	type(type),
	target(target) {
}

}
}
