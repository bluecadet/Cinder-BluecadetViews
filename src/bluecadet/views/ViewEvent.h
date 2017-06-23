#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

namespace bluecadet {
namespace views {

// forward declarations
typedef std::shared_ptr<class BaseView> BaseViewRef;

struct ViewEvent {
	// Types
	struct Type {
		static const std::string CONTENT_INVALIDATED;
	};

	// Properties
	std::string			type;
	BaseViewRef			target;
	BaseViewRef			currentTarget = nullptr;

	bool				shouldPropagate = true;

	void				stopPropagation();

	ViewEvent(const std::string & type, BaseViewRef target = nullptr);
	virtual ~ViewEvent() {};
};

}
}


