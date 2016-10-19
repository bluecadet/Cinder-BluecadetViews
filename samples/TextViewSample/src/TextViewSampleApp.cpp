#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <core/BaseApp.h>
#include <views/TextView.h>

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::text;

class TextViewSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;
	TextViewRef	mTitle;
	int count;
};

void TextViewSampleApp::prepareSettings(ci::app::App::Settings* settings) {
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

void TextViewSampleApp::setup() {

	BaseApp::setup();
	BaseApp::addTouchSimulatorParams();

	// Optional: configure the size and background of your root view
	getRootView()->setBackgroundColor(Color::gray(0.5f));
	getRootView()->setSize(ScreenLayout::getInstance()->getAppSize());

	// Sample content
	mTitle = TextViewRef(new TextView());

	// Set a size to wrap text; -1.0 will let the textview grow in that direction.
	mTitle->setSize(vec2(200.0f, -1.0f));

	// Background color and text color can both be set independently
	mTitle->setBackgroundColor(Color(1, 0, 0));
	mTitle->setTextColor(Color(1.0f, 1.0f, 1.0f));

	// Change font size
	mTitle->setFontSize(64.0f);

	// All styles will be applied to text now
	mTitle->setTextAlign(TextAlign::Center);
	mTitle->setText("Sample Title " + toString(count));

	getRootView()->addChild(mTitle);

	// init
	count = -1;
	mouseDown(MouseEvent());
}

void TextViewSampleApp::update() {
	// Optional override. BaseApp::update() will update all views.
	BaseApp::update();
}

void TextViewSampleApp::draw() {
	// Optional override. BaseApp::draw() will draw all views.
	BaseApp::draw();
}

void TextViewSampleApp::mouseDown(MouseEvent event) {
	// Increase count to be displayed
	count++;

	// Clear and reset the text
	mTitle->setWidth(400.0f);
	mTitle->setHeight(400.0f);
	mTitle->setText("Sample Title " + toString(count));

	vec2 padding = vec2(16.f, 16.f);

	// top left plus padding
	vec2 pos = vec2(0) + padding;

	if (count % 2) {
		// bottom right minus padding
		pos = vec2(getWindowSize()) - mTitle->getSize() - padding;
	}

	mTitle->getTimeline()->apply(&mTitle->getPosition(), pos, 0.33f, EaseOutElastic(1.1f, 0.75f));
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(TextViewSampleApp, RendererGl(RendererGl::Options().msaa(4)), TextViewSampleApp::prepareSettings);
