#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <touch/TouchManager.h>
#include <views/TouchView.h>

namespace bluecadet {
namespace touch {

typedef std::shared_ptr<class GestureWorksTouchPlugin> GestureWorksTouchPluginRef;

class GestureWorksTouchPlugin : public TouchManagerPlugin {

public:
	GestureWorksTouchPlugin();
	~GestureWorksTouchPlugin();

	void wasAddedTo(TouchManager * manager) override;
	void willBeRemovedFrom(TouchManager * manager) override;

	void preUpdate(TouchManager * manager) override;
	void processEvent(TouchManager * manager, const TouchEvent & event) override;
	void postUpdate(TouchManager * manager) override;

protected:

	void initialize();

	std::map<std::string, size_t> mNumGesturesPerViewId;
	std::map<std::string, views::TouchViewWeakRef> mViewsById;

	ci::vec2 mAppSize;
	bool mIsInitialized;
};

}
}
