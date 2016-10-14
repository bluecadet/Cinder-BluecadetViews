#include "Event.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

const string Event::Type::UPDATED = "updated";
const string Event::Type::GENERIC = "generic";

Event::Event(const std::string & type, BaseView * target) :
	type(type),
	target(target) {
}

}
}
