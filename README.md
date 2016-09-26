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

#include "BaseApp.h"
#include "TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::views;
using namespace bluecadet::utils;
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
	BaseApp::prepareSettings(settings);
	settings->setFullScreen(false);
	settings->setWindowSize(1280, 720);
	settings->setBorderless(false);
}

void BaseAppSampleApp::setup() {
	// Optional: configure a multi-screen layout
	ScreenLayout::getInstance()->setNumRows(3);
	ScreenLayout::getInstance()->setNumColumns(4);

	BaseApp::setup();
	BaseApp::addTouchSimulatorParams();

	getRootView()->setBackgroundColor(Color::gray(0.5f));
	getRootView()->setSize(ScreenLayout::getInstance()->getAppSize());

	auto button = TouchViewRef(new TouchView());
	button->setPosition(vec2(100, 100));
	button->setSize(vec2(200, 100));
	button->setBackgroundColor(Color(1, 0, 0));
	button->mDidTap.connect([=](bluecadet::touch::TouchEvent e) { CI_LOG_I("Button tapped"); });
	getRootView()->addChild(button);
}

void BaseAppSampleApp::update() {
	BaseApp::update();
}

void BaseAppSampleApp::draw() {
	// Optional override. BaseApp::draw() will draw all views.
	BaseApp::draw();
}

// MSAA is optional, but passing in prepareSettings need to happen to parse and apply certain app settings
CINDER_APP(BaseAppSampleApp, RendererGl(RendererGl::Options().msaa(4)), BaseAppSampleApp::prepareSettings);

```

## Dependencies

* Cinder-BluecadetUtils (https://github.com/bluecadet/Cinder-BluecadetUtils)
* Cinder-BluecadetText (https://github.com/bluecadet/Cinder-BluecadetText)
* Cinder-BluecadetTouch (https://github.com/bluecadet/Cinder-BluecadetTouch)
	* Requires Cinder OSC block   
	* Requires Cinder TUIO block   

1. Clone each of the above dependency blocks into your `Cinder\blocks` folder (Cinder OSC & TUIO will come automatically when you download Cinder). 
2. Create a new project with TinderBox and make sure to select *relative* when including all Bluecadet blocks

## Notes

Version 1.0.0
