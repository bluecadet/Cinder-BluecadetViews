#include "ScreenLayout.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

namespace bluecadet {
namespace views {

ScreenLayout::ScreenLayout() :
	mNumRows(1),
	mNumColumns(1),
	mDisplaySize(ci::ivec2(1920, 1080)),
	mAppSize(mDisplaySize),
	mBorderSize(2.0f),
	mBorderColor(ColorA(1.0f, 0.0f, 1.0f, 1.0f)),
	mPlaceholderView(new BaseView())
{
}

ScreenLayout::~ScreenLayout() {
}

void ScreenLayout::setup(const ci::ivec2& dislaySize, const int numRows, const int numColumns) {
	mDisplaySize = dislaySize;
	mNumRows = numRows;
	mNumColumns = numColumns;

	updateLayout();

	// Connect w low priority to give other parts of app first dibs on key events.
	// E.g. this prevents zooming/panning when typing into gl::InterfaceGl params.
	getWindow()->getSignalKeyDown().connect(-1, std::bind(&ScreenLayout::handleKeyDown, this, std::placeholders::_1));
}

void ScreenLayout::updateLayout() {
	mDisplayBounds.clear();

	for (int row = 0; row < mNumRows; ++row) {
		for (int col = 0; col < mNumColumns; ++col) {
			Rectf displayBounds = getDisplayBounds(row, col);
			mDisplayBounds.push_back(displayBounds);
		}
	}

	mAppSize = mDisplaySize * ivec2(mNumColumns, mNumRows);
}

Rectf ScreenLayout::getDisplayBounds(const int displayId) {
	return getDisplayBounds(getColFromDisplayId(displayId), getRowFromDisplayId(displayId));
}

Rectf ScreenLayout::getDisplayBounds(const int row, const int col) {
	return Rectf(
		(float)(col * mDisplaySize.x),
		(float)(row * mDisplaySize.y),
		(float)((col + 1) * mDisplaySize.x),
		(float)((row + 1) * mDisplaySize.y));
}

void ScreenLayout::draw() {

	gl::ScopedColor scopedColor(mBorderColor);
	gl::ScopedLineWidth scopedLineWidth(mBorderSize);
	gl::ScopedModelMatrix scopedMatrix;

	gl::multModelMatrix(getTransform());

	for (const auto &outline : mDisplayBounds) {
		gl::drawStrokedRect(outline);
	}
}

//==================================================
// Scaling/zooming helpers
// 

void ScreenLayout::zoomToDisplay(const int displayId) {
	zoomToDisplay(getRowFromDisplayId(displayId), getColFromDisplayId(displayId));
}

void ScreenLayout::zoomToDisplay(const int row, const int col) {
	const Rectf displayBounds = getDisplayBounds(row, col);
	const vec2 winSize = getWindowSize();
	const float scale = getScaleToFitBounds(displayBounds, winSize);

	mPlaceholderView->setScale(scale);
	mPlaceholderView->setPosition(vec2(0, 0));
	const vec2 pos = mPlaceholderView->convertLocalToGlobal(displayBounds.getUpperLeft());
	mPlaceholderView->setPosition(-pos);
}

void ScreenLayout::zoomToFitWindow() {
	const vec2 winSize = getWindowSize();
	const vec2 appSize = getAppSize();
	const Rectf appBounds = Rectf(vec2(), appSize);
	const float scale = getScaleToFitBounds(appBounds, winSize);
	const vec2 pos = (winSize - appSize * scale) * 0.5f;

	mPlaceholderView->setScale(scale);
	mPlaceholderView->setPosition(pos);
}

void ScreenLayout::zoomAtLocation(const float targetScale, const vec2 location) {
	const vec2 currentScale = mPlaceholderView->getScale();
	const vec2 deltaScale = vec2(targetScale) / currentScale;

	const vec2 winCenter = getWindowCenter();

	const vec2 currentPos = mPlaceholderView->getPosition();
	const vec2 targetPos = (currentPos - winCenter) * deltaScale + winCenter; // see http://math.stackexchange.com/a/5808/363352

	mPlaceholderView->setScale(targetScale);
	mPlaceholderView->setPosition(targetPos);
}

float ScreenLayout::getScaleToFitBounds(const ci::Rectf &bounds, const ci::vec2 &maxSize, const float padding) const {
	Rectf paddedBounds = bounds.inflated(vec2(padding));
	float xScale = maxSize.x / (float)bounds.getWidth();
	float yScale = maxSize.y / (float)bounds.getHeight();
	float scale = std::min(xScale, yScale); // scale to fit
	return scale;
}


//==================================================
// Event Handlers
// 

void ScreenLayout::handleKeyDown(KeyEvent event) {
	if (event.isHandled()) {
		// Don't change layout when event has been handled by other parts of the app
		return;
	}

	switch (event.getCode()) {
		case KeyEvent::KEY_KP_PLUS:
		case KeyEvent::KEY_KP_MINUS:
		case KeyEvent::KEY_PLUS:
		case KeyEvent::KEY_EQUALS:
		case KeyEvent::KEY_MINUS: {
			// zoom in/out
			const auto code = event.getCode();
			const bool zoomIn = (code == KeyEvent::KEY_KP_PLUS || code == KeyEvent::KEY_PLUS || code == KeyEvent::KEY_EQUALS);
			const float speed = event.isShiftDown() ? 1.25f : 1.1f;
			const float deltaScale = (zoomIn ? speed : 1.0f / speed);
			const float targetScale = mPlaceholderView->getScale().value().x * deltaScale;
			zoomAtWindowCenter(targetScale);
			break;
		}
		case KeyEvent::KEY_UP: {
			// pan up
			mPlaceholderView->setPosition(vec2(mPlaceholderView->getPosition().value().x, mPlaceholderView->getPosition().value().y += getWindowHeight() * (event.isShiftDown() ? 1.0f : 0.25f)));
			break;
		}
		case KeyEvent::KEY_DOWN: {
			// pan down
			mPlaceholderView->setPosition(vec2(mPlaceholderView->getPosition().value().x, mPlaceholderView->getPosition().value().y -= getWindowHeight() * (event.isShiftDown() ? 1.0f : 0.25f)));
			break;
		}
		case KeyEvent::KEY_LEFT: {
			// pan left
			mPlaceholderView->setPosition(vec2(mPlaceholderView->getPosition().value().x += getWindowWidth() * (event.isShiftDown() ? 1.0f : 0.25f), mPlaceholderView->getPosition().value().y));
			break;
		}
		case KeyEvent::KEY_RIGHT: {
			// pan right
			mPlaceholderView->setPosition(vec2(mPlaceholderView->getPosition().value().x -= getWindowWidth() * (event.isShiftDown() ? 1.0f : 0.25f), mPlaceholderView->getPosition().value().y));
			break;
		}
		case KeyEvent::KEY_0:
		case KeyEvent::KEY_KP0: {
			// toggle zoom to fit window
			if (mPlaceholderView->getScale().value().x != 1.0f) {
				zoomAtWindowCenter(1.0f);
			} else {
				zoomToFitWindow();
			}
			break;
		}
		case KeyEvent::KEY_KP1: case KeyEvent::KEY_KP2: case KeyEvent::KEY_KP3: case KeyEvent::KEY_KP4: case KeyEvent::KEY_KP5: case KeyEvent::KEY_KP6: case KeyEvent::KEY_KP7: case KeyEvent::KEY_KP8: case KeyEvent::KEY_KP9:
		case KeyEvent::KEY_1: case KeyEvent::KEY_2: case KeyEvent::KEY_3: case KeyEvent::KEY_4: case KeyEvent::KEY_5: case KeyEvent::KEY_6: case KeyEvent::KEY_7: case KeyEvent::KEY_8: case KeyEvent::KEY_9: {
			// zoom into display on top-most row
			int displayId = (event.getChar() - (int)'0') - 1; // parse int from char, make 0-based
			zoomToDisplay(displayId);
			break;
		}
	}
}



}
}
