# Cinder-BluecadetViews

This block presents a set of classes to build an interactive scene graph and a set of base implementations to more easily create app UIs in Cinder.

The scene graph is composed of individual views, which can have children and each have basic animatable properties like `position`, `scale`, `rotation` and `alpha`. Children inherit their parents' transformations.

In addition to nested transformations and drawing, this block connects to the text and touch blocks to provide `TextView` and `TouchView`.

To combine all pieces conveniently, this block comes with a `BaseApp` class that provides a basic implementation with a root view, touch manager and various utilities.

Built for and tested with [Cinder v0.9.1](https://github.com/cinder/Cinder/tree/v0.9.1). See [notes below](#Notes) for setup instructions.

![](docs/media/class-hierarchy.png)

## Key Features

### Scene Graph

* Add/remove children to/from `BaseView`s and all its subclasses
* Conversion from/to local/global coordinate spaces
* Simple event system to bubble messages up the graph
* Index management (e.g. move child to front/back)

### Touch Management

* Support for TUIO, native touch and mouse events
* Touch simulator for stress-testing tapping and dragging
* Multi-touch simulator for mouse input (e.g. to scale/rotate)
* Extendable plugin architecture (e.g. for TangibleEngine pucks or third-party gesture libraries like GestureWorks)
* Shape-based hit detection with ability to override on a per-class basis

### Core App Classes

* Define screen layout for multi-screen matrices
* Pan and zoom around your app using keyboard shortcuts with a minimap with touchable views
* Central, extendable settings manager to load common and custom JSON and CLI settings like FPS, V-Sync, Screen Layout, etc.

### BaseView

A basic, rectangular view with an optional size and background color that can contain children and be added as a child to other `BaseView`s.

* Animatable properties: `position`, `scale`, `rotation`, `tint`, `alpha`, `backgroundColor`
* Transform origin for rotating and scaling around a local point
* `update()` and `draw()` loops

### TouchView

* Extends `BaseView` with added touch capabilities
* Touch *began*, *updated* and *ended* events, overrideable protected methods and explicit signals 
* Distinction between dragging and tapping with distance and time thresholds

### TextView

* Multi-line text layout with basic inline styling support
* HTML tags: `<b>`, `<i>`, `<br>`, `<p>`
* Styles: `fontFamily`, `fontStyle`, `fontWeight`, `fontSize`, `leadingOffset`, `textColor`, `textAlign`, `textTransform`
* Automatic word-wrapping and other layout modes (single line, strip line-breaks, multi-line clip, multi-line auto-wrap)
* `string` and `wstring` support
* Layout-caching minimizes re-calculation of layout while maintaining ability to call methods like `getSize()` at any time
* *Windows only, requires [Cinder-BluecadetText](/bluecadet/Cinder-BluecadetText)*

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

Version 1.4.0

Based on [Cinder v0.9.1](https://github.com/cinder/Cinder/tree/v0.9.1)

Cinder setup instructions:


```bash
git clone --recursive git@github.com:cinder/Cinder.git
git checkout tags/v0.9.1
git submodule update --init
```
