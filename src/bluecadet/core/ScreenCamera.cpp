#include "ScreenCamera.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

namespace bluecadet {
namespace core {

ScreenCamera::ScreenCamera() : mLayout(nullptr), mPlaceholderView(new BaseView()) {
}

ScreenCamera::~ScreenCamera() {}

void ScreenCamera::setup(ScreenLayoutRef layout) {
	mLayout = layout;

	mSignalConnections.clear();

	// Connect w low priority to give other parts of app first dibs on key events.
	// E.g. this prevents zooming/panning when typing into gl::InterfaceGl params.
	mSignalConnections += getWindow()->getSignalKeyDown().connect(
		-1, std::bind(&ScreenCamera::handleKeyDown, this, std::placeholders::_1));
	mSignalConnections += getWindow()->getSignalResize().connect(-1,
																 std::bind(&ScreenCamera::handleWindowResize, this));
}

//==================================================
// Scaling/zooming helpers
//

void ScreenCamera::zoomToDisplay(const int displayId) {
	zoomToDisplay(mLayout->getRowFromDisplayId(displayId), mLayout->getColFromDisplayId(displayId));
	updateViewport();
}

void ScreenCamera::zoomToDisplay(const int row, const int col) {
	const Rectf displayBounds = mLayout->getDisplayBounds(row, col);
	const vec2 winSize = getWindowSize();
	const float scale = getScaleToFitBounds(displayBounds, winSize);

	const vec2 offset = (winSize - scale * displayBounds.getSize()) * 0.5f;

	mPlaceholderView->setScale(scale);
	mPlaceholderView->setPosition(vec2(0, 0));

	const vec2 pos = mPlaceholderView->convertLocalToGlobal(displayBounds.getUpperLeft());

	mPlaceholderView->setPosition(offset - pos);
	updateViewport();
}

void ScreenCamera::zoomToFitWindow() {
	const vec2 winSize = getWindowSize();
	const vec2 appSize = mLayout->getAppSize();
	const Rectf appBounds = Rectf(vec2(), appSize);
	const float scale = getScaleToFitBounds(appBounds, winSize);
	const vec2 pos = (winSize - appSize * scale) * 0.5f;

	mPlaceholderView->setScale(scale);
	mPlaceholderView->setPosition(pos);
	updateViewport();
}

void ScreenCamera::zoomAtCurrentLocation(const float targetScale) {
	const vec2 currentScale = mPlaceholderView->getScale();
	const vec2 deltaScale = vec2(targetScale) / currentScale;

	const vec2 winCenter = getWindowCenter();

	const vec2 currentPos = mPlaceholderView->getPosition();
	const vec2 targetPos = (currentPos - winCenter) * deltaScale +
						   winCenter;  // see http://math.stackexchange.com/a/5808/363352

	mPlaceholderView->setScale(targetScale);
	mPlaceholderView->setPosition(targetPos);

	updateViewport();
}

void ScreenCamera::zoomAtLocation(const float targetScale, const vec2 location) {
	mPlaceholderView->setScale(targetScale);
	mPlaceholderView->setPosition(location);
	updateViewport();
}

float ScreenCamera::getScaleToFitBounds(const ci::Rectf & bounds, const ci::vec2 & maxSize, const float padding) const {
	Rectf paddedBounds = bounds.inflated(vec2(padding));
	float xScale = maxSize.x / (float)paddedBounds.getWidth();
	float yScale = maxSize.y / (float)paddedBounds.getHeight();
	float scale = std::min(xScale, yScale);  // scale to fit
	return scale;
}


//==================================================
// Event Handlers
//

void ScreenCamera::handleWindowResize() {
	updateViewport();
}

void ScreenCamera::handleKeyDown(KeyEvent event) {
	if (event.isHandled()) {
		// Don't change layout when event has been handled by other parts of the app
		return;
	}

	float panSpeed = (float)min(getWindowWidth(), getWindowHeight()) * (event.isShiftDown() ? 0.5f : 0.125f);
	float zoomSpeed = event.isShiftDown() ? 1.25f : 1.1f;

	switch (event.getCode()) {
		case KeyEvent::KEY_KP_PLUS:
		case KeyEvent::KEY_KP_MINUS:
		case KeyEvent::KEY_PLUS:
		case KeyEvent::KEY_EQUALS:
		case KeyEvent::KEY_MINUS: {
			// zoom in/out
			const auto code = event.getCode();
			const bool zoomIn = (code == KeyEvent::KEY_KP_PLUS || code == KeyEvent::KEY_PLUS ||
								 code == KeyEvent::KEY_EQUALS);
			const float deltaScale = (zoomIn ? zoomSpeed : 1.0f / zoomSpeed);
			const float targetScale = mPlaceholderView->getScale().value().x * deltaScale;
			zoomAtCurrentLocation(targetScale);
			break;
		}
		case KeyEvent::KEY_UP: {
			// pan up
			mPlaceholderView->setPosition(
				vec2(mPlaceholderView->getPosition().value().x, mPlaceholderView->getPosition().value().y += panSpeed));
			updateViewport();
			break;
		}
		case KeyEvent::KEY_DOWN: {
			// pan down
			mPlaceholderView->setPosition(
				vec2(mPlaceholderView->getPosition().value().x, mPlaceholderView->getPosition().value().y -= panSpeed));
			updateViewport();
			break;
		}
		case KeyEvent::KEY_LEFT: {
			// pan left
			mPlaceholderView->setPosition(
				vec2(mPlaceholderView->getPosition().value().x += panSpeed, mPlaceholderView->getPosition().value().y));
			updateViewport();
			break;
		}
		case KeyEvent::KEY_RIGHT: {
			// pan right
			mPlaceholderView->setPosition(
				vec2(mPlaceholderView->getPosition().value().x -= panSpeed, mPlaceholderView->getPosition().value().y));
			updateViewport();
			break;
		}
		case KeyEvent::KEY_0:
		case KeyEvent::KEY_KP0: {
			if (mZoomToggleHotkeyEnabled) {
				// toggle zoom to fit window
				if (mPlaceholderView->getScale().value().x != 1.0f) {
					zoomAtCurrentLocation(1.0f);
				} else {
					zoomToFitWindow();
				}
			}
			break;
		}
		case KeyEvent::KEY_KP1:
		case KeyEvent::KEY_KP2:
		case KeyEvent::KEY_KP3:
		case KeyEvent::KEY_KP4:
		case KeyEvent::KEY_KP5:
		case KeyEvent::KEY_KP6:
		case KeyEvent::KEY_KP7:
		case KeyEvent::KEY_KP8:
		case KeyEvent::KEY_KP9:
		case KeyEvent::KEY_1:
		case KeyEvent::KEY_2:
		case KeyEvent::KEY_3:
		case KeyEvent::KEY_4:
		case KeyEvent::KEY_5:
		case KeyEvent::KEY_6:
		case KeyEvent::KEY_7:
		case KeyEvent::KEY_8:
		case KeyEvent::KEY_9: {
			if (mDisplayIdHotkeysEnabled) {
				// zoom into display on top-most row
				int displayId = (event.getChar() - (int)'0') - 1;  // parse int from char, make 0-based
				zoomToDisplay(displayId);
			}
			break;
		}
	}
}

void ScreenCamera::updateViewport() {
	ivec2 winSize = (AppBase::get() && getWindow()) ? getWindowSize() : ivec2(0);
	float scale = mPlaceholderView->getScale().value().x;
	ivec2 size = ivec2(vec2(winSize) / scale);
	ivec2 pos = -ivec2(mPlaceholderView->getPosition().value() / scale);
	mViewport = Area(pos, pos + size);
	mViewportChangedSignal.emit(mViewport);
}

}  // namespace core
}  // namespace bluecadet
