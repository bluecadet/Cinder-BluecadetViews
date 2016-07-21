#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

#include "drivers/MouseDriver.h"
#include "drivers/TuioDriver.h"
#include "TouchManager.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class BaseApp> BaseAppRef;

class BaseApp : public ci::app::App {

public:

	BaseApp();
	virtual ~BaseApp();

	virtual void setup() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void keyDown(ci::app::KeyEvent event) override;

//	void		zoomToScreen(const int& screenId = 1);
//	void		scaleRootViewCentered(const float& targetScale);

	static void prepareSettings(ci::app::App::Settings *settings);

protected:
	views::BaseViewRef			mRootView;
	touch::drivers::MouseDriver	mMouseDriver;
	touch::drivers::TuioDriver	mTuioDriver;

	double						mLastFrameTime;
};

}
}