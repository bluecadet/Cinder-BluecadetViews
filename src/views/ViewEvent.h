#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

//#include "boost/signals2.hpp"

namespace bluecadet {
namespace views {

class BaseView; // forward declaration

struct ViewEvent {
	// Types
	struct Type {
		static const std::string CONTENT_INVALIDATED;
	};

	// Properties
	std::string			type;
	BaseView *			target;
	BaseView *			currentTarget = nullptr;

	bool				shouldPropagate = true;

	void				stopPropagation();

	ViewEvent(const std::string & type, BaseView * target = nullptr);
};

}
}


