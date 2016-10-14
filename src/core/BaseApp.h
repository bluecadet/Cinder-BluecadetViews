#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

// These aren't used in the header, but added as convenience includes for BaseApp sub classes.
#include "SettingsManager.h"
#include "ScreenLayout.h"

#include <views/BaseView.h>
#include <touch/TouchManager.h>
#include <touch/drivers/MouseDriver.h>
#include <touch/drivers/TuioDriver.h>
#include <touch/drivers/SimulatedTouchDriver.h>

namespace bluecadet {
namespace core {

typedef std::shared_ptr<class BaseApp> BaseAppRef;

class BaseApp : public ci::app::App {

public:

	BaseApp();
	virtual ~BaseApp();

	// Cinder app events
	virtual void setup() override;
	virtual void update() override;
	virtual void draw() override { draw(true); };
	virtual void draw(const bool clear);

	virtual void keyDown(ci::app::KeyEvent event) override;

	//! Call this method when initializing your app with the CINDER_APP macro
	static void prepareSettings(ci::app::App::Settings *settings);

	//! Adds a set of params to control the touch simulator
	void		addTouchSimulatorParams(float touchesPerSecond = 50.f);

	//! Use this view to add any children. The root view may be scaled and translated when using ScreenLayout to zoom/pan around the app.
	views::BaseViewRef	getRootView() const	{ return mRootView; };

	//! The last time that update was called in seconds since app launch.
	double		getLastUpdateTime() const	{ return mLastUpdateTime; }


	//! The main touch driver running on TUIO. Automatically connected at app launch.
	touch::drivers::TuioDriver				getTouchDriver() const		{ return mTuioDriver; }

	//! The main mouse driver. Automatically connected at app launch.
	touch::drivers::MouseDriver				getMouseDriver() const { return mMouseDriver; }
	
	//! The main mouse driver. Configured with the current window size at app launch, but needs to be started explicitly.
	touch::drivers::SimulatedTouchDriver	getTouchSimDriver() const	{ return mSimulatedTouchDriver; }

private:
	views::BaseViewRef						mRootView;
	double									mLastUpdateTime;

	touch::drivers::TuioDriver				mTuioDriver;
	touch::drivers::MouseDriver				mMouseDriver;
	touch::drivers::SimulatedTouchDriver	mSimulatedTouchDriver;
};

}
}
