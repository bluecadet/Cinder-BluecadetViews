#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class NestedTransformsSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;

	int spawnChildren(BaseViewRef parent, const int numChildren);
};

void NestedTransformsSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->mFullscreen = false;
	SettingsManager::getInstance()->mWindowSize = ivec2(800, 800);
	SettingsManager::getInstance()->mBorderless = false;
	BaseApp::prepareSettings(settings);
	ScreenLayout::getInstance()->setDisplaySize(SettingsManager::getInstance()->mWindowSize);
}

void NestedTransformsSampleApp::setup() {
	BaseApp::setup();

	gl::enableDepth(false);

	// Spawn nested children
	int spawnedChildren = spawnChildren(getRootView(), 40);
	console() << "Spawned " << to_string(spawnedChildren) << " children" << endl;
}

int NestedTransformsSampleApp::spawnChildren(BaseViewRef parent, const int numChildren) {
	int spawnedChildren = numChildren;

	for (int i = 0; i < numChildren; ++i) {
		BaseViewRef item(new BaseView());

		item->setBackgroundColor(ColorA(randFloat(), randFloat(), randFloat(), 0.5f));
		item->setSize(vec2(randFloat(max(parent->getWidth(), parent->getHeight()))) * 0.5f);
		item->setPosition(vec2(randFloat(parent->getWidth()), randFloat(parent->getHeight())) - item->getSize() * 0.5f);
		item->setTransformOrigin(item->getSize() * 0.5f);

		item->getTimeline()->apply(&item->getRotation(), glm::angleAxis(randFloat(0, 2.0f * M_PI), vec3(0, 0, 1)), randFloat(5, 10), EaseInOutQuad()).pingPong(true).loop(true);

		parent->addChild(item);

		spawnedChildren += spawnChildren(item, numChildren / 4);
	}

	return spawnedChildren;
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(NestedTransformsSampleApp, RendererGl(RendererGl::Options().msaa(4)), NestedTransformsSampleApp::prepareSettings);
