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

	// All touches have been captured, but not yet assigned to views
	virtual void preUpdate(TouchManager * manager, std::deque<Touch> & touches) {};

	// Touch events with assigned views, one view at a time
	virtual void processEvent(TouchManager * manager, const TouchEvent & event) {};

	// All views have received their touch events at this point
	virtual void postUpdate(TouchManager * manager, std::deque<Touch> & touches) {};

};

}
}
