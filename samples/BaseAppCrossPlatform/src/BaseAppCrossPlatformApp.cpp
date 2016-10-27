#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "core/BaseApp.h"
#include "views/TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class BaseAppCrossPlatformApp : public BaseApp {
public:
    static void prepareSettings(ci::app::App::Settings* settings);
    void setup() override;
    void update() override;
    void draw() override;
};

void BaseAppCrossPlatformApp::prepareSettings(ci::app::App::Settings* settings) {
    // Use this method to set up your window
    SettingsManager::getInstance()->mFullscreen = false;
    SettingsManager::getInstance()->mWindowSize = ivec2(1280, 720);
    SettingsManager::getInstance()->mBorderless = false;
    
    BaseApp::prepareSettings(settings);
    
    // Optional: configure a multi-screen layout (defaults to 1x1 1080p landscape)
    ScreenLayout::getInstance()->setDisplaySize(ivec2(1080, 1920));
    ScreenLayout::getInstance()->setNumRows(1);
    ScreenLayout::getInstance()->setNumColumns(3);
}

void BaseAppCrossPlatformApp::setup() {
    
    BaseApp::setup();
    BaseApp::addTouchSimulatorParams();
    
    // Optional: configure your root view
    getRootView()->setBackgroundColor(Color::gray(0.5f));
    
    // Sample content
    auto button = TouchViewRef(new TouchView());
    button->setSize(getRootView()->getSize() * 0.75f);
    button->setPosition((getRootView()->getSize() - button->getSize()) * 0.5f);
    button->setBackgroundColor(ColorA(1, 0, 0, 0.75f));
    button->setMultiTouchEnabled(true);
    button->mDidTap.connect([=](bluecadet::touch::TouchEvent e) { CI_LOG_I("Button tapped"); });
    getRootView()->addChild(button);
}

void BaseAppCrossPlatformApp::update() {
    // Optional override. BaseApp::update() will update all views.
    BaseApp::update();
}

void BaseAppCrossPlatformApp::draw() {
    // Optional override. BaseApp::draw() will draw all views.
    BaseApp::draw();
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(BaseAppCrossPlatformApp, RendererGl(RendererGl::Options().msaa(4)), BaseAppCrossPlatformApp::prepareSettings);
