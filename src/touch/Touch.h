#pragma once

namespace bluecadet {

namespace views {
	// Forward declaration of TouchView class and shared_ptr
	typedef std::shared_ptr<class TouchView> TouchViewRef;
}

namespace touch {

//==================================================
// Types
//

enum TouchType { Touch, Mouse, Simulator, Fiducial, Other };

enum TouchPhase { Began, Moved, Ended };

struct TouchEvent {
	// Mandatory values
	int					id				= -1;
	ci::vec2			position		= ci::vec2(0);	//! The global touch position in app coordinate space; (0, 0) is at the top left of your application regardless of zoom/pan.
	ci::vec2			localPosition	= ci::vec2(0);	//! The local touch position in the target view's coordinate space; (0, 0) is at your view's origin
	ci::vec2			windowPosition	= ci::vec2(0);	//! The raw touch position in window space. (0, 0) is at the top-left of your window and coordinates are unscaled and true to window pixels.
	TouchType			type			= Other;
	TouchPhase			phase			= Began;

	// Optional values
	views::TouchViewRef	target			= nullptr;
	bool				isCanceled		= false;
	bool				isVirtual		= false;

	TouchEvent() {};

	TouchEvent(const int id, const ci::vec2& position, const TouchType type, const TouchPhase phase) :
		id(id),
		position(position),
		windowPosition(position),
		localPosition(position),
		type(type),
		phase(phase)
	{}
};

}
}
