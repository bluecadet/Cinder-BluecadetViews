#include "ContentController.h"

#include "ImageManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

ContentController::ContentController() : BaseView(){
}

ContentController::~ContentController(){
}

void ContentController::setup() {

	// Preload assets in "testImages" folder
	ImageManager::getInstance()->loadAllImagesInDirectory("testImages");

	// Create heading
	mHeading = HeadingRef(new Heading());
	mHeading->setup();
	
	// Create detail views
	for (int i = 0; i < 10; i++) {
		mDetailImage = DetailImageRef(new DetailImage());
		mDetailImage->setup(i);
		
		// Add each detail image
		addChild(mDetailImage);
	}

	// Add heading above images
	addChild(mHeading);
}

void ContentController::update(double deltaTime) {
}

void ContentController::draw() {
}