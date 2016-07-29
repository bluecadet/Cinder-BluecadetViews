#include "ScreenLayoutView.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

namespace bluecadet {
namespace views {

ScreenLayoutView::ScreenLayoutView() :
	mDisplayWidth(1920),
	mDisplayHeight(1080),
	mDisplayTotalRows(1),
	mDisplayTotalColumns(1),
	mBorderWidth(4.0f),
	mBorderColor(ColorA(1.0f, 0.0f, 0.0f, 1.0f))
{
}

ScreenLayoutView::~ScreenLayoutView() {
}

void ScreenLayoutView::setup(BaseViewRef baseRootView, const int displayWidth, const int displayHeight, const int rows, const int columns) {

	mBaseRootView = baseRootView;

	// Get key input
	ci::app::getWindow()->getSignalKeyDown().connect(std::bind(&ScreenLayoutView::keyDown, this, std::placeholders::_1));

	// Load display settings
	mDisplayWidth = displayWidth;
	mDisplayHeight = displayHeight;
	mDisplayTotalRows = rows;
	mDisplayTotalColumns = columns;

	// Set app width and height based on screen layout //move appWidth/height to screenLayout
	mAppWidth = mDisplayWidth*mDisplayTotalColumns;
	mAppHeight = mDisplayHeight*mDisplayTotalRows;

	// Setup the outlines that will draw for each display
	int screenId = 1;
	for (int row = 0; row < mDisplayTotalRows; ++row) {
		for (int col = 0; col < mDisplayTotalColumns; ++col) {

			ci::Rectf displayBounds = getDisplayBounds(col, row);
			mDisplayOutlines.push_back(std::make_pair(screenId, displayBounds));
			screenId++;

			/*
			// Todo - Create as screen bounds views -- need to create view that allows outlines of shapes only
			debug::RectViewRef rect = debug::RectViewRef(new debug::RectView(vec2(mDisplayWidth, mDisplayHeight), mBorderColor));
			rect->setPosition(vec2(mDisplayWidth*row, mDisplayHeight*col));
			addChild(rect);
			*/
		}
	}

}

ci::Rectf ScreenLayoutView::getDisplayBounds(const int& displayId) {
	for (auto &displayOutline : mDisplayOutlines) {
		if (displayOutline.first == displayId) {
			return displayOutline.second;
		}
	}
	return ci::Rectf();
}

ci::Rectf ScreenLayoutView::getDisplayBounds(const int& column, const int& row) {
	return ci::Rectf(
		(float)(column * mDisplayWidth),
		(float)(row * mDisplayHeight),
		(float)((column + 1.0f) * mDisplayWidth),
		(float)((row + 1.0f) * mDisplayHeight));
}

//void ScreenLayout::update(double deltaTime) {
//}

void ScreenLayoutView::draw() {
	ci::gl::color(mBorderColor);
	ci::gl::lineWidth(mBorderWidth);

	for (auto &displayOutline : mDisplayOutlines) {
		ci::gl::drawStrokedRect(displayOutline.second);
	}
}

// HELPERS

const float ScreenLayoutView::getScaleToFitBounds(ci::Rectf bounds, ci::vec2 maxSize, float padding) {
	bounds.x1 -= padding;
	bounds.y1 -= padding;
	bounds.x2 += padding;
	bounds.y2 += padding;

	float xScale = maxSize.x / (float)bounds.getWidth();
	float yScale = maxSize.y / (float)bounds.getHeight();
	float scale = std::min(xScale, yScale); // scale to fit

	return scale;
}

const ci::vec2 ScreenLayoutView::getTranslateToCenterBounds(ci::Rectf bounds, ci::vec2 maxSize) {
	return ci::vec2(
		((float)maxSize.x - bounds.getWidth()) * 0.5f - bounds.x1,
		((float)maxSize.y - bounds.getHeight()) * 0.5f - bounds.y1
	);
}

void ScreenLayoutView::zoomToScreen(const int& screenId) {
	ci::Rectf bounds = ScreenLayoutView::getInstance()->getDisplayBounds(screenId);
	ci::vec2 winSize = getWindowSize();

	mBaseRootView->setScale(vec2(ScreenLayoutView::getInstance()->getScaleToFitBounds(bounds, winSize)));
	mBaseRootView->setPosition(ScreenLayoutView::getInstance()->getTranslateToCenterBounds(bounds, winSize / mBaseRootView->getScale().value().x));
}

void ScreenLayoutView::scaleRootViewCentered(const float& targetScale) {
	const float currentScale = mBaseRootView->getScale().value().x;

	vec2 windowSize = vec2(getWindowSize());
	vec2 currentSize = windowSize / currentScale;
	vec2 targetSize = windowSize / targetScale;

	mBaseRootView->setScale(vec2(targetScale));
	mBaseRootView->setPosition(mBaseRootView->getPosition().value() += (targetSize - currentSize) * 0.5f);
}

void ScreenLayoutView::keyDown(KeyEvent event) {

	switch (event.getCode()) {

	case KeyEvent::KEY_KP_PLUS:
	case KeyEvent::KEY_KP_MINUS:
	case KeyEvent::KEY_PLUS:
	case KeyEvent::KEY_EQUALS:
	case KeyEvent::KEY_MINUS: {
		const auto code = event.getCode();
		const float dir = (code == KeyEvent::KEY_KP_PLUS || code == KeyEvent::KEY_PLUS || code == KeyEvent::KEY_EQUALS) ? 1.0f : -1.0f;
		const float speed = event.isShiftDown() ? 0.25f : 0.1f;
		const float targetScale = mBaseRootView->getScale().value().x * (1.0f + dir * speed);
		scaleRootViewCentered(targetScale);
		break;
	}
	case KeyEvent::KEY_KP1: case KeyEvent::KEY_KP2: case KeyEvent::KEY_KP3: case KeyEvent::KEY_KP4: case KeyEvent::KEY_KP5: case KeyEvent::KEY_KP6: case KeyEvent::KEY_KP7:
	case KeyEvent::KEY_1: case KeyEvent::KEY_2: case KeyEvent::KEY_3: case KeyEvent::KEY_4: case KeyEvent::KEY_5: case KeyEvent::KEY_6: case KeyEvent::KEY_7: {
		int screenId = (event.getChar() - (int)'0') - 1; // parse int from char, make 0-based
		zoomToScreen(screenId);
		break;
	}
	case KeyEvent::KEY_UP: {
		mBaseRootView->setPosition(vec2(mBaseRootView->getPosition().value().x, mBaseRootView->getPosition().value().y += getWindowHeight() * (event.isShiftDown() ? 1.0f : 0.25f)));
		break;
	}
	case KeyEvent::KEY_DOWN: {
		mBaseRootView->setPosition(vec2(mBaseRootView->getPosition().value().x, mBaseRootView->getPosition().value().y -= getWindowHeight() * (event.isShiftDown() ? 1.0f : 0.25f)));
		break;
	}
	case KeyEvent::KEY_LEFT: {
		mBaseRootView->setPosition(vec2(mBaseRootView->getPosition().value().x += getWindowWidth() * (event.isShiftDown() ? 1.0f : 0.25f), mBaseRootView->getPosition().value().y));
		break;
	}
	case KeyEvent::KEY_RIGHT: {
		mBaseRootView->setPosition(vec2(mBaseRootView->getPosition().value().x -= getWindowWidth() * (event.isShiftDown() ? 1.0f : 0.25f), mBaseRootView->getPosition().value().y));
		break;
	}
	}
}



}
}