#pragma once

//==================================================
// TouchManagerPlugin Base Class
// 

#include "bluecadet/touch/Touch.h"

namespace bluecadet {
namespace touch {

class TouchManager;

typedef std::shared_ptr<class TouchManagerPlugin> TouchManagerPluginRef;

class TouchManagerPlugin {

public:
	TouchManagerPlugin() {};
	virtual ~TouchManagerPlugin() {};

	virtual void wasAddedTo(TouchManager * manager) {};
	virtual void willBeRemovedFrom(TouchManager * manager) {};

	virtual void preUpdate(TouchManager * manager, std::deque<Touch> & touches) {};
	virtual void processEvent(TouchManager * manager, const TouchEvent & event) {};
	virtual void postUpdate(TouchManager * manager, std::deque<Touch> & touches) {};

};

}
}
