#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ContentController.h"
#include "TouchManager.h"
#include "touch_drivers/TuioDriver.h"
#include "touch_drivers/Mouse.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::touch;

class NestedViewsSampleApp : public App {
public:
	static void prepareSettings(App::Settings *settings);

	void setup() override;
	void update() override;
	void draw() override;

private:
	// Touches
	TouchManagerRef			mTouchManager;
	Mouse					mMouseDriver;
	TuioDriver				mTuioDriver;	

	// Views and content
	ContentControllerRef	mContentController;
};

void NestedViewsSampleApp::prepareSettings(App::Settings *settings) {
	// Separate console window
	settings->setConsoleWindowEnabled(true);
	settings->setResizable(false);
	settings->setWindowSize(vec2(1536, 512));
}

void NestedViewsSampleApp::setup() {
	// Setup touches
	mTouchManager = TouchManager::getInstance();
	mMouseDriver.connect();
	mTuioDriver.connect();

	// Create shared pointer to content controller. All child views will be added here, so we only need to update and draw this one main container view.
	mContentController = ContentControllerRef(new ContentController());
	mContentController->setup();
}

void NestedViewsSampleApp::update() {
	// Update touches
	mTouchManager->update(mContentController);

	// Update views & content
	mContentController->updateScene(0);
}

void NestedViewsSampleApp::draw() {
	gl::clear(Color(0, 0, 0));

	// Draw touches (debugging)
	mTouchManager->draw();

	// Draw views & content
	mContentController->drawScene();
}


//==================================================
// Cinder app setup
// 

CINDER_APP(
	NestedViewsSampleApp,
	RendererGl(RendererGl::Options().msaa(8)), // enable MSAA to smoothly animate text and thin lines
	NestedViewsSampleApp::prepareSettings
)
