#pragma once

#include "../views/ViewEvent.h"

namespace bluecadet {

namespace views {
// Forward declaration of TouchView class and shared_ptr
typedef std::shared_ptr<class TouchView> TouchViewRef;
}

namespace touch {

//==================================================
// Types
//

enum class TouchType { Touch, Mouse, Simulator, Fiducial, Other };

enum class TouchPhase { Began, Moved, Ended };

typedef ci::signals::Signal<void(const struct TouchEvent & touchEvent)> TouchSignal;

//! Touches are used to store the state of active touches within the TouchManager.
struct Touch {

	// Mandatory values
	int				id				= -1;
	ci::vec2		appPosition		= ci::vec2(0);	//! Gets calculated by the TouchManager based on app's pan/zoom
	ci::ivec2		windowPosition	= ci::ivec2(0);	//! Raw touch position in window coordinate space
	TouchType		type			= TouchType::Other;
	TouchPhase		phase			= TouchPhase::Began;

	//! Mostly used for touch simulation
	bool			isVirtual		= false;

	Touch() {};
	Touch(const int id, const ci::ivec2 windowPosition, const TouchType type, const TouchPhase phase);
};


//! TouchEvents are dispatched by the TouchManager via touched views.
struct TouchEvent : public views::ViewEvent {
	struct Type : public views::ViewEvent::Type {
		static const std::string TOUCH;
	};

	// Mandatory values
	ci::vec2			globalPosition	= ci::vec2(0);	//! The global touch position in app coordinate space; (0, 0) is at the top left of your application regardless of zoom/pan.
	ci::vec2			localPosition	= ci::vec2(0);	//! The local touch position in the target view's coordinate space; (0, 0) is at your view's origin
	ci::ivec2			windowPosition	= ci::vec2(0);	//! The raw touch position in window space. (0, 0) is at the top-left of your window and coordinates are unscaled and true to window pixels.

	int					touchId			= -1;
	TouchType			touchType		= TouchType::Other;
	TouchPhase			touchPhase		= TouchPhase::Began;

	// Optional values
	views::TouchViewRef	touchTarget		= nullptr;
	bool				isCanceled		= false;

	TouchEvent() : ViewEvent(TouchEvent::Type::TOUCH) {};
	TouchEvent(const Touch & touch);
	TouchEvent(const int touchId, const ci::ivec2 & windowPosition, const ci::vec2 & globalPosition, const TouchType touchType, const TouchPhase phase);
};

}
}
