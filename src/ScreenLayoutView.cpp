#include "ScreenLayoutView.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::utils;

namespace bluecadet {
namespace utils {

ScreenLayoutView::ScreenLayoutView() :
	mNumRows(1),
	mNumColumns(1),
	mDisplaySize(ci::ivec2(1920, 1080)),
	mAppSize(mDisplaySize),
	mBorderWidth(4.0f),
	mBorderColor(ColorA(1.0f, 0.0f, 0.0f, 1.0f)),
	mRootView(nullptr)
{
}

ScreenLayoutView::~ScreenLayoutView() {
}

void ScreenLayoutView::setup(views::BaseViewRef baseRootView, const ci::ivec2& dislaySize, const int numRows, const int numColumns) {

	mRootView = baseRootView;

	// Get key input
	ci::app::getWindow()->getSignalKeyDown().connect(std::bind(&ScreenLayoutView::keyDown, this, std::placeholders::_1));

	// Load display settings
	mDisplaySize = dislaySize;
	mNumRows = numRows;
	mNumColumns = numColumns;

	// Set app width and height based on screen layout //move appWidth/height to screenLayout
	mAppSize = mDisplaySize * ivec2(mNumRows, mNumColumns);

	// Setup the outlines that will draw for each display
	for (int row = 0; row < mNumRows; ++row) {
		for (int col = 0; col < mNumColumns; ++col) {

			ci::Rectf displayBounds = getDisplayBounds(col, row);
			mDisplayOutlines.push_back(displayBounds);

			/*
			// Todo - Create as screen bounds views -- need to create view that allows outlines of shapes only
			debug::RectViewRef rect = debug::RectViewRef(new debug::RectView(vec2(mDisplayWidth, mDisplayHeight), mBorderColor));
			rect->setPosition(vec2(mDisplayWidth*row, mDisplayHeight*col));
			addChild(rect);
			*/
		}
	}

}

ci::Rectf ScreenLayoutView::getDisplayBounds(const int displayId) {
	return getDisplayBounds(getColFromDisplayId(displayId), getColFromDisplayId(displayId));
}

ci::Rectf ScreenLayoutView::getDisplayBounds(const int col, const int row) {
	return ci::Rectf(
		(float)(col * mDisplaySize.x),
		(float)(row * mDisplaySize.y),
		(float)((col + 1) * mDisplaySize.x),
		(float)((row + 1) * mDisplaySize.y));
}

void ScreenLayoutView::draw() {
	gl::color(mBorderColor);
	gl::lineWidth(mBorderWidth);

	for (const auto &displayOutline : mDisplayOutlines) {
		gl::drawStrokedRect(displayOutline);
	}
}

// HELPERS

const float ScreenLayoutView::getScaleToFitBounds(const ci::Rectf &bounds, const ci::vec2 &maxSize, const float padding) {
	Rectf paddedBounds = bounds.inflated(vec2(padding));
	float xScale = maxSize.x / (float)bounds.getWidth();
	float yScale = maxSize.y / (float)bounds.getHeight();
	float scale = std::min(xScale, yScale); // scale to fit
	return scale;
}

const ci::vec2 ScreenLayoutView::getTranslateToCenterBounds(const ci::Rectf &bounds, const ci::vec2& maxSize) {
	return ci::vec2(
		((float)maxSize.x - bounds.getWidth()) * 0.5f - bounds.x1,
		((float)maxSize.y - bounds.getHeight()) * 0.5f - bounds.y1
	);
}


void ScreenLayoutView::zoomToDisplay(const int displayId) {
	zoomToDisplay(getColFromDisplayId(displayId), getColFromDisplayId(displayId));
}

void ScreenLayoutView::zoomToDisplay(const int col, const int row)
{
	ci::Rectf bounds = getDisplayBounds(col, row);
	ci::vec2 winSize = getWindowSize();

	mRootView->setScale(vec2(ScreenLayoutView::getInstance()->getScaleToFitBounds(bounds, winSize)));
	mRootView->setPosition(ScreenLayoutView::getInstance()->getTranslateToCenterBounds(bounds, winSize / mRootView->getScale().value().x));
}

void ScreenLayoutView::scaleRootViewCentered(const float targetScale) {
	/*const float currentScale = mRootView->getScale().value().x;

	vec2 windowSize = vec2(getWindowSize());
	vec2 currentSize = windowSize / currentScale;
	vec2 targetSize = windowSize / targetScale;*/


	vec2 transformOrigin = targetScale + getWindowCenter() - mRootView->getPosition().value();
	mRootView->setTransformOrigin(transformOrigin);
	
	mRootView->setScale(targetScale);

	//mRootView->setPosition(mRootView->getPosition().value() += (targetSize - currentSize) * 0.5f);
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
			const float targetScale = mRootView->getScale().value().x * (1.0f + dir * speed);
			scaleRootViewCentered(targetScale);
			break;
		}
		case KeyEvent::KEY_KP1: case KeyEvent::KEY_KP2: case KeyEvent::KEY_KP3: case KeyEvent::KEY_KP4: case KeyEvent::KEY_KP5: case KeyEvent::KEY_KP6: case KeyEvent::KEY_KP7:
		case KeyEvent::KEY_1: case KeyEvent::KEY_2: case KeyEvent::KEY_3: case KeyEvent::KEY_4: case KeyEvent::KEY_5: case KeyEvent::KEY_6: case KeyEvent::KEY_7: {
			int displayId = (event.getChar() - (int)'0') - 1; // parse int from char, make 0-based
			zoomToDisplay(displayId);
			break;
		}
		case KeyEvent::KEY_UP: {
			mRootView->setPosition(vec2(mRootView->getPosition().value().x, mRootView->getPosition().value().y += getWindowHeight() * (event.isShiftDown() ? 1.0f : 0.25f)));
			break;
		}
		case KeyEvent::KEY_DOWN: {
			mRootView->setPosition(vec2(mRootView->getPosition().value().x, mRootView->getPosition().value().y -= getWindowHeight() * (event.isShiftDown() ? 1.0f : 0.25f)));
			break;
		}
		case KeyEvent::KEY_LEFT: {
			mRootView->setPosition(vec2(mRootView->getPosition().value().x += getWindowWidth() * (event.isShiftDown() ? 1.0f : 0.25f), mRootView->getPosition().value().y));
			break;
		}
		case KeyEvent::KEY_RIGHT: {
			mRootView->setPosition(vec2(mRootView->getPosition().value().x -= getWindowWidth() * (event.isShiftDown() ? 1.0f : 0.25f), mRootView->getPosition().value().y));
			break;
		}
	}
}



}
}