# Cinder-BluecadetViews

This block presents a set of classes to build an interactive scene graph and a set of base implementations to more easily create app UIs in Cinder.

The scene graph is composed of individual views, which can have children and each have basic animatable properties like `position`, `scale`, `rotation` and `alpha`. Children inherit their parents' transformations.

In addition to nested transformations and drawing, this block connects to the text and touch blocks to provide `TextView` and `TouchView`.

To combine all pieces conveniently, this block comes with a `BaseApp` class that provides a basic implementation with a root view, touch manager and various utilities.

## Getting Started

Clone the block and check the dependencies below to make sure you're all set to start your first project.

You can use the boilerplate below for your main application file:

```c++
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <core/BaseApp.h>
#include <views/TouchView.h>

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class BaseAppSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	void draw() override;
};

void BaseAppSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	// Use this method to set up your window
	SettingsManager::getInstance()->mFullscreen = false;
	SettingsManager::getInstance()->mWindowSize = ivec2(1280, 720);
	SettingsManager::getInstance()->mBorderless = false;

	BaseApp::prepareSettings(settings);

	// Optional: configure a multi-screen layout
	ScreenLayout::getInstance()->setDisplaySize(ivec2(1080, 1920));
	ScreenLayout::getInstance()->setNumRows(1);
	ScreenLayout::getInstance()->setNumColumns(3);
}

void BaseAppSampleApp::setup() {

	BaseApp::setup();
	BaseApp::addTouchSimulatorParams();

	// Optional: configure the size and background of your root view
	getRootView()->setBackgroundColor(Color::gray(0.5f));
	getRootView()->setSize(ScreenLayout::getInstance()->getAppSize());

	// Sample content
	auto button = TouchViewRef(new TouchView());
	button->setPosition(vec2(100, 100));
	button->setSize(vec2(200, 100));
	button->setBackgroundColor(Color(1, 0, 0));
	button->mDidTap.connect([=](bluecadet::touch::TouchEvent e) { CI_LOG_I("Button tapped"); });
	getRootView()->addChild(button);
}

void BaseAppSampleApp::update() {
	// Optional override. BaseApp::update() will update all views.
	BaseApp::update();
}

void BaseAppSampleApp::draw() {
	// Optional override. BaseApp::draw() will draw all views.
	BaseApp::draw();
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(BaseAppSampleApp, RendererGl(RendererGl::Options().msaa(4)), BaseAppSampleApp::prepareSettings);

```

## Dependencies

* Cinder-BluecadetUtils (https://github.com/bluecadet/Cinder-BluecadetUtils)
* Cinder-BluecadetText (https://github.com/bluecadet/Cinder-BluecadetText)
* Cinder OSC block
* Cinder TUIO block

1. Clone each of the above dependency blocks into your `Cinder\blocks` folder (Cinder OSC & TUIO will come automatically when you download Cinder). 
2. Create a new project with TinderBox and make sure to select *relative* when including all Bluecadet blocks

## Notes

Version 1.1.0
