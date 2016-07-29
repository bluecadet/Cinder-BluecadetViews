#include "ContentController.h"

#include "ImageManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;
using namespace bluecadet::utils;

ContentController::ContentController() : BaseView(){
}

ContentController::~ContentController(){
}

void ContentController::setup() {

	// Preload assets in "testImages" folder
	ImageManager::getInstance()->loadAllImagesInDirectory("testImages");

	// Create one header
	mHeader = HeaderRef(new Header());
	mHeader->setup();
	mHeader->setPosition(vec2(getWindowWidth()*0.5f, 40.0f));
	
	// Create several detail views
	for (int i = 0; i < 10; i++) {
		DetailImageRef detail = DetailImageRef(new DetailImage());
		detail->setDebugDrawTouchPath(true); // draw debug touch path
		detail->setup(i);

		// Add the individual detail to a container that holds all the detalis
		mDetailImages.push_back(detail);

		// Add each detail as a child
		addChild(detail);
	}

	// Add heading (will display above images because we added it after we added each detail as a child)
	addChild(mHeader);
}

void ContentController::update(double deltaTime) {
}

void ContentController::draw() {
}