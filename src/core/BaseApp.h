#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

// These aren't used in the header, but added as convenience includes for BaseApp sub classes.
#include "SettingsManager.h"
#include "ScreenLayout.h"

#include "../views/BaseView.h"
#include "../views/MiniMapView.h"
#include "../views/GraphView.h"
#include "../touch/TouchManager.h"
#include "../touch/drivers/MouseDriver.h"
#include "../touch/drivers/TuioDriver.h"
#include "../touch/drivers/SimulatedTouchDriver.h"

namespace bluecadet {
namespace core {

typedef std::shared_ptr<class BaseApp> BaseAppRef;

class BaseApp : public ci::app::App {

public:

	BaseApp();
	virtual ~BaseApp();

	// Cinder app events
	void setup() override;
	void update() override;
	void draw() override { draw(true); };
	virtual void draw(const bool clear);

	void keyDown(ci::app::KeyEvent event) override;

	virtual void handleAppSizeChange(const ci::ivec2 & appSize);
	virtual void handleViewportChange(const ci::Area & viewport);

	//! Call this method when initializing your app with the CINDER_APP macro
	static void prepareSettings(ci::app::App::Settings *settings);

	//! Adds a set of params to control the touch simulator
	void		addTouchSimulatorParams(float touchesPerSecond = 50.f);

	//! Use this view to add any children. The root view may be scaled and translated when using ScreenLayout to zoom/pan around the app.
	views::BaseViewRef	getRootView() const	{ return mRootView; };

	//! The last time that update was called in seconds since app launch.
	double		getLastUpdateTime() const	{ return mLastUpdateTime; }

	//! Debug view to render stats like fps in a graph.
	views::GraphViewRef	getStats() const { return mStats; };

	//! The main touch driver running on TUIO. Automatically connected at app launch.
	touch::drivers::TuioDriver &            getTouchDriver()		{ return mTuioDriver; }

	//! The main mouse driver. Automatically connected at app launch.
	touch::drivers::MouseDriver &           getMouseDriver()        { return mMouseDriver; }
	
	//! The main mouse driver. Configured with the current window size at app launch, but needs to be started explicitly.
	touch::drivers::SimulatedTouchDriver &	getTouchSimDriver()     { return mSimulatedTouchDriver; }

private:
	views::BaseViewRef						mRootView;
	views::MiniMapViewRef					mMiniMap;
	views::GraphViewRef						mStats;
	double									mLastUpdateTime;
	float									mDebugUiPadding;

	touch::drivers::TuioDriver				mTuioDriver;
	touch::drivers::MouseDriver				mMouseDriver;
	touch::drivers::SimulatedTouchDriver	mSimulatedTouchDriver;
};

}
}
