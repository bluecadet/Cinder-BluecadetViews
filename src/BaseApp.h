#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "BaseView.h"

// These aren't used in the header, but added as convenience includes for BaseApp sub classes.
#include "SettingsManager.h"
#include "ScreenLayout.h"
#include "TouchManager.h"

#include "drivers/MouseDriver.h"
#include "drivers/TuioDriver.h"
#include "drivers/SimulatedTouchDriver.h"

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

	static void prepareSettings(ci::app::App::Settings *settings);
	void		addTouchSimulatorParams(float touchesPerSecond = 50.f);

protected:
	
	views::BaseViewRef						mRootView;
	double									mLastFrameTime;

	touch::drivers::MouseDriver				mMouseDriver;
	touch::drivers::TuioDriver				mTuioDriver;
	touch::drivers::SimulatedTouchDriver	mSimulatedTouchDriver;

};

}
}
