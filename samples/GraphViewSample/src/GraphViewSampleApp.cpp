#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <core/BaseApp.h>
#include <views/GraphView.h>

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class GraphViewSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	GraphViewRef mGraph;
};

void GraphViewSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->mFullscreen = false;
	SettingsManager::getInstance()->mWindowSize = ivec2(720, 480);
	SettingsManager::getInstance()->mBorderless = false;
	SettingsManager::getInstance()->mConsoleWindowEnabled = false;
	BaseApp::prepareSettings(settings);
	ScreenLayout::getInstance()->setDisplaySize(SettingsManager::getInstance()->mWindowSize);
}

void GraphViewSampleApp::setup() {
	BaseApp::setup();

	SettingsManager::getInstance()->getParams()->minimize();

	getRootView()->setBackgroundColor(Color::gray(0.5f));
	getRootView()->setSize(ScreenLayout::getInstance()->getAppSize());

	mGraph = GraphViewRef(new GraphView(getWindowSize(), 1));
	//mGraph->addGraph("fps", 0, getFrameRate());
	mGraph->addGraph("mouse_x", 0, getWindowWidth(), ColorA(0, 1, 0, 0.5f), ColorA(1, 0, 0, 0.5f));
	mGraph->addGraph("mouse_y", 0, getWindowHeight(), ColorA(0, 0, 1, 0.5f), ColorA(1, 0, 0, 0.5f));

	setFpsSampleInterval(0.1);

	getRootView()->addChild(mGraph);
}

void GraphViewSampleApp::update() {
	BaseApp::update();
	//mGraph->addValue("fps", getAverageFps());
	mGraph->addValue("mouse_x", (getMousePos() - getWindowPos()).x);
	mGraph->addValue("mouse_y", (getMousePos() - getWindowPos()).y);
}

CINDER_APP(GraphViewSampleApp, RendererGl(RendererGl::Options().msaa(4)), GraphViewSampleApp::prepareSettings);