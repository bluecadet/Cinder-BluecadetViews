# Cinder-BluecadetViews

This block presents a set of classes to build an interactive scene graph and a set of base implementations to more easily create app UIs in Cinder.

The scene graph is composed of individual views, which can have children and each have basic animatable properties like `position`, `scale`, `rotation` and `alpha`. Children inherit their parents' transformations.

In addition to nested transformations and drawing, this block connects to the text and touch blocks to provide `TextView` and `TouchView`.

To combine all pieces conveniently, this block comes with a `BaseApp` class that provides a basic implementation with a root view, touch manager and various utilities.

Built for and tested with [Cinder v0.9.2 dev](https://github.com/cinder/Cinder/). See [notes below](#notes) for setup instructions.

![](docs/media/class-hierarchy.png)

## Key Features

### Scene Graph

* Add/remove children to/from `BaseView`s and all its subclasses
* Conversion from/to local/global coordinate spaces
* Simple event system to bubble messages up the graph
* Index management (e.g. move child to front/back)
* Inherited transformations, alpha and tint

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

### MaskView

* A `MaskView` can use any `BaseView` as a mask for its child views
* `REVEAL` and `HIDE` masked content modes (essentially stencil or knockout)
* Uses GL stencils, so doesn't support semi-transparency, but does allow for more custom shapes than GL scissor
* Most view subclasses can be used individually and combined as masks

### SettingsManager

The SettingsManager provides an easy means to map JSON settings to app parameters, override them via command line parameters for development and load/save them to/from JSON via InterfaceGl params.

### View Samples

![](docs/media/view-types-sample.gif)

### Misc Features

| Multi-Screen Support | Virtual Touches & Stress Testing |
|---|---|
| ![](docs/media/debug-multi-screen.gif) | ![](docs/media/debug-touch-stress-test.gif) |
| Bezel compensation, debug layout, mini-map, keyboard-based panning/zooming. | Built-in support to create virtual touches and stress test your app. Can also be used to simulate complex touch patterns like capacitive fiducials. |

| Multi-Touch Simulation | Debug Info | Plugin Support |
|---|---|--|
| ![](docs/media/debug-multi-touch-sim.gif) | ![](docs/media/debug-view-info.gif) | ![](docs/media/debug-plugins.gif) |
| Simulate multiple touches with your mouse cursor. | Display view bounds, position, transform origin, type and name or id. | Simulate, intercept and manipulate touches with custom plugins. |

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
	SettingsManager::setInstance(myApp::MyAppSettingsManager::get());
	
	// Initialize the settings manager with the cinder app settings and the settings json
	SettingsManager::get()->setup(settings);
}

void BaseAppSampleApp::setup() {

	BaseApp::setup();

	// Optional: configure your root view
	getRootView()->setBackgroundColor(Color::gray(0.5f));

	// Sample content
	auto button = make_shared<TouchView>();
	button->setPosition(400.f, 300.f);
	button->setSize(200.f, 100.f);
	button->setBackgroundColor(Color(1, 0, 0));
	button->getSignalTapped().connect([=](...) { CI_LOG_I("Button tapped"); });
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

## Tinderbox Template

To make setup easier, this block includes a template called `Bluecadet App`. When you open TinderBox to create a new project, select `Bluecadet App` from the `Template` dropdown at the first step:

![docs/media/tinderbox-template.png](docs/media/tinderbox-template.png)

This will create a base app class and a settings manager for you. The default namespace for the settings manager is `bluecadet`, which you're free to change.

For maximum compatibility, you should include the `OSC` and `TUIO` blocks via *copy* and not as *relative*:

![docs/media/tinderbox-includes.png](docs/media/tinderbox-includes.png)

Due to a TinderBox bug, your `SettingsManager` subclass header will be located in `Header Files` in your VS project. You can simply drag it to `Source Files`.

### Cinder Path

To support maximum compatibility across machines, we encourage you to use the provided `ProjectConfig.props` property sheet included in the template (at `templates/BluecadetApp/ProjectConfig.props`) to define your Cinder path on each computer independently.

To use it, open your Visual Studio project:
1. Open the Property Manager view via *Views > Other Windows > Property Manager*
2. Select *Add Existing Property Sheet*	
3. Add the `ProjectConfig.props` file that should be in your app's root directory now (assuming you used the *Bluecadete App* template to create the project)
4. Try to build your project (it should fail)
5. Double-click the build error that says `Cinder path is not configured correctly ('C:\Users\...'). Please enter your Cinder path in 'C:\Users\...\UserConfig.props'.`
6. Enter your relative or absolute path to the Cinder root directory in `<CinderDir>...</CinderDir>`
7. Rebuild

For your convenience, the template includes a `.gitignore` file that will automatically be copied to your project directory to ignore the auto-generated `UserConfig.props` file, which is machine-specific.

## Custom Subviews

Out of the box, Cinder-BluecadetViews supplies the most basic types of views needed to stub out an interactive application. Eventually, you'll want to write your own `BaseView` subclasses that override `update()` or `draw()`.

Below is a simple example:

### PathView.h

```c++
#pragma once

#include "bluecadet/views/BaseView.h"

typedef std::shared_ptr<class PathView> PathViewRef;

class PathView : public bluecadet::views::BaseView {

public:
	PathView(ci::Path2d path) : mPath(path) {}
	~PathView() {}

protected:
	void update(const FrameInfo & frame) override;
	void draw() override;

	ci::Path2d mPath;
};
```

### PathView.cpp

```c++
#include "PathView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void PathView::update(const FrameInfo & frame) {
	// update your view on each frame if you'd like
	// no need to call base view implementation.
	// FrameInfo contains the time since the previous
	// update call (deltaTime) and the time the app
	// has been running (absoluteTime).
}
void PathView::draw() {
	// no need to call base-view implementation
	// unless you want to draw a solid rect of
	// getSize() and getBackgroundColor()
	// bluecadet::views::BaseView::draw();
	
	// you could set the color to the current background color
	// but by default getTint() and getAlpha() are used
	// gl::ScopedColor color(getBackgroundColor());
	
	// this will draw the path using the current color, which
	// defaults to getDrawColor() (combination of tint and alpha)
	gl::draw(mPath);
}
```


## Dependencies

* Cinder-BluecadetText (https://github.com/bluecadet/Cinder-BluecadetText)
* Cinder OSC block
* Cinder TUIO block

## Notes

Version 1.7.0

Built for [Cinder v0.9.2 dev](https://github.com/cinder/Cinder/) and [Cinder v0.9.1](https://github.com/cinder/Cinder/tree/v0.9.1). Samples require VS 2015 v140 toolset, but tested with VS 2013 v120 as well.

Cinder setup instructions:

```bash
# Cinder dev
git clone --depth 1 --recursive https://github.com/cinder/Cinder.git

# Cinder 0.9.1 stable
# git clone -b v0.9.1 --depth 1 --recursive https://github.com/cinder/Cinder.git

# Bluecadet blocks + dependencies
cd Cinder/blocks
git clone git@github.com:bluecadet/Cinder-BluecadetText.git
git clone git@github.com:bluecadet/Cinder-BluecadetViews.git
```
