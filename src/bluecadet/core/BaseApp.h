#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

// These aren't used in the header, but added as convenience includes for BaseApp sub classes.
#include "ScreenLayout.h"
#include "SettingsManager.h"

#include "../views/BaseView.h"
#include "../views/GraphView.h"
#include "../views/MiniMapView.h"

#ifndef NO_TOUCH
#include "../touch/TouchManager.h"
#include "../touch/drivers/MouseDriver.h"
#include "../touch/drivers/MultiNativeTouchDriver.h"
#include "../touch/drivers/NativeTouchDriver.h"
#include "../touch/drivers/SimulatedTouchDriver.h"
#include "../touch/drivers/TuioDriver.h"
#endif

namespace bluecadet {
namespace core {

typedef std::shared_ptr<class BaseApp> BaseAppRef;

class BaseApp : public ci::app::App {

public:
	BaseApp();
	virtual ~BaseApp();

	// Cinder app events
	void setup() override;		 // Use this method to do any initial setup before the first update() and draw().
	virtual void lateSetup(){};  // Use this method to finish any setup. Called after the first update() and draw().
	void update() override;		 // Called on each frame before draw().
	void draw() override { draw(true); };  // Called on each frame after update().
	virtual void draw(const bool clear);   // Use this method instead of just draw() to enable/disable clearing.

	void keyDown(ci::app::KeyEvent event) override;

	// Navigates up the directory tree to find the sub path.
	void findAssetDir(const std::string & subPath, bool stopAtFirst = true);

	// Use this view to add any children. The root view may be scaled and translated when using ScreenLayout to
	// zoom/pan around the app.
	views::BaseViewRef getRootView() const { return mRootView; };

	// The last time that update was called in seconds since app launch.
	double getLastUpdateTime() const { return mLastUpdateTime; }

	// Debug view to render stats like fps in a graph.
	views::GraphViewRef getStats() const { return mStats; };

#ifndef NO_TOUCH
	// The main touch driver running on TUIO. Automatically connected at app launch.
	touch::drivers::TuioDriver & getTouchDriver() { return mTuioDriver; }

	// The main mouse driver. Automatically connected at app launch.
	touch::drivers::MouseDriver & getMouseDriver() { return mMouseDriver; }

	// The main native driver. Disconnected by default at app launch.
	touch::drivers::NativeTouchDriver & getNativeTouchDriver() { return mNativeTouchDriver; }

	// The main mouse driver. Configured with the current window size at app launch, but needs to be started
	// explicitly.
	touch::drivers::SimulatedTouchDriver & getTouchSimDriver() { return mSimulatedTouchDriver; }

	// Adds a set of params to control the touch simulator
	void addTouchSimulatorParams(float touchesPerSecond = 50.f);
#endif

protected:
	virtual void handleAppSizeChange(const ci::ivec2 & appSize);
	virtual void handleViewportChange(const ci::Area & viewport);
	virtual void handleSettingsLoaded();

private:
	views::BaseViewRef mRootView;
	views::MiniMapViewRef mMiniMap;
	views::GraphViewRef mStats;
	double mLastUpdateTime;
	float mDebugUiPadding;
	bool mIsLateSetupCompleted;

#ifndef NO_TOUCH
	touch::drivers::TuioDriver mTuioDriver;
	touch::drivers::MouseDriver mMouseDriver;
	touch::drivers::NativeTouchDriver mNativeTouchDriver;
	touch::drivers::MultiNativeTouchDriver mMultiNativeTouchDriver;
	touch::drivers::SimulatedTouchDriver mSimulatedTouchDriver;
#endif
};

}  // namespace core
}  // namespace bluecadet