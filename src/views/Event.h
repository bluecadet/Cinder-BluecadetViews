#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

//#include "boost/signals2.hpp"

namespace bluecadet {
namespace views {

class BaseView; // forward declaration

struct Event {
	// Types
	struct Type {
		static const std::string GENERIC;
		static const std::string UPDATED;
	};

	// Properties
	std::string			type;
	BaseView *			target;
	BaseView *			currentTarget = nullptr;

	bool				shouldPropagate = true;

	void				stopPropagation();

	Event(const std::string & type = Type::GENERIC, BaseView * target = nullptr);
};

//typedef std::function<void(const Event & event)>			EventCallback;
//typedef boost::signals2::signal<void(const Event & event)>	EventSignal;

}
}


