# Cinder-BluecadetViews

This block presents a set of classes to build an interactive scene graph and a set of base implementations to more easily create app UIs in Cinder.

The scene graph is composed of individual views, which can have children and each have basic animatable properties like `position`, `scale`, `rotation` and `alpha`. Children inherit their parents' transformations.

In addition to nested transformations and drawing, this block connects to the text and touch blocks to provide `TextView` and `TouchView`.

To combine all pieces conveniently, this block comes with a `BaseApp` class that provides a basic implementation with a root view, touch manager and various utilities.

Built for and tested with [Cinder v0.9.1](https://github.com/cinder/Cinder/tree/v0.9.1). See [notes below](#Notes) for setup instructions.

## Getting Started

Clone the block and check the dependencies below to make sure you're all set to start your first project.

You can use the boilerplate below for your main application file:

```c++
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/TouchView.h"

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
	// Optional: Override the shared settings manager instance with your subclass
	SettingsManager::setInstance(myApp::MyAppSettingsManager::getInstance());
	
	// Initialize the settings manager with the cinder app settings and the settings json
	SettingsManager::getInstance()->setup(settings, ci::app::getAssetPath("appSettings.json"), [](SettingsManager * manager) {
		// Optional: Override json defaults at runtime
		manager->mFullscreen = false;
		manager->mWindowSize = ivec2(1280, 720);
	});
}

void BaseAppSampleApp::setup() {

	BaseApp::setup();
	BaseApp::addTouchSimulatorParams();

	// Optional: configure your root view
	getRootView()->setBackgroundColor(Color::gray(0.5f));

	// Sample content
	auto button = TouchViewRef(new TouchView());
	button->setPosition(vec2(400, 300));
	button->setSize(vec2(200, 100));
	button->setBackgroundColor(Color(1, 0, 0));
	button->getSignalTapped().connect([=](bluecadet::touch::TouchEvent e) { CI_LOG_I("Button tapped"); });
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

Version 1.3.1

Based on [Cinder v0.9.1](https://github.com/cinder/Cinder/tree/v0.9.1)

Cinder setup instructions:


```bash
git clone --recursive git@github.com:cinder/Cinder.git
git checkout tags/v0.9.1
git submodule update --init
```
