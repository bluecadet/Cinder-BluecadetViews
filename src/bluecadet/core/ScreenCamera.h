//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents:
//  Comments:
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "../views/BaseView.h"
#include "ScreenLayout.h"

namespace bluecadet {
namespace core {

typedef std::shared_ptr<class ScreenCamera> ScreenCameraRef;

class ScreenCamera {

public:
	ScreenCamera();
	~ScreenCamera();


	static ScreenCameraRef getInstance() {
		static ScreenCameraRef instance = nullptr;
		if (!instance) instance = ScreenCameraRef(new ScreenCamera());
		return instance;
	}


	//! Must be called before calling draw. Adds a key-up event listener.
	void setup(ScreenLayoutRef layout);

	//! Dispatched whenever the current viewport (pan/zoom) is changed.
	ci::signals::Signal<void(const ci::Area & viewport)> & getViewportChangedSignal() {
		return mViewportChangedSignal;
	};

	//! Zooms to fit the display at displayId into the current application window.
	void zoomToDisplay(const int displayId);

	//! Zooms to fit the display at col/row into the current application window.
	void zoomToDisplay(const int row, const int col);

	//! Zooms around the current location
	void zoomAtCurrentLocation(const float scale);

	//! Zooms around a location in window coordinate space
	void zoomAtLocation(const float scale, const ci::vec2 location);

	//! Zooms around the application window's center
	inline void zoomAtWindowCenter(const float scale) { zoomAtLocation(scale, ci::app::getWindowCenter()); }

	//! Zooms the app to fit centered into the current window
	void zoomToFitWindow();

	//! The current camera transform.
	const ci::mat4 & getTransform() const { return mPlaceholderView->getTransform(); };

	//! The current camera translation.
	const ci::vec2 & getTranslation() const { return mPlaceholderView->getPosition().value(); };

	//! Sets the current camera translation.
	void setTranslation(const ci::vec2 & translation) {
		mPlaceholderView->setPosition(translation);
		updateViewport();
	};

	//! The current camera scale.
	const ci::vec2 & getScale() const { return mPlaceholderView->getScale().value(); };

	//! The current viewport in app coordinates.
	const ci::Area & getViewport() const { return mViewport; }

	//! When true, will map keys 1-9 to zoom to displays 1-9
	bool getDisplayIdHotkeysEnabled() const { return mDisplayIdHotkeysEnabled; }
	void setDisplayIdHotkeysEnabled(const bool value) { mDisplayIdHotkeysEnabled = value; }

	//! When true, will map the 0 key to toggling between scale to fit and 100%
	bool getZoomToggleHotkeyEnabled() const { return mZoomToggleHotkeyEnabled; }
	void setZoomToggleHotkeyEnabled(const bool value) { mZoomToggleHotkeyEnabled = value; }

protected:
	float getScaleToFitBounds(const ci::Rectf & bounds, const ci::vec2 & maxSize, const float padding = 0.0f) const;

	void updateViewport();
	void handleWindowResize();
	void handleKeyDown(ci::app::KeyEvent event);

	ci::Area mViewport;
	views::BaseViewRef mPlaceholderView;
	ScreenLayoutRef mLayout;

	bool mDisplayIdHotkeysEnabled = true;
	bool mZoomToggleHotkeyEnabled = true;

	ci::signals::Signal<void(const ci::Area & viewport)> mViewportChangedSignal;
	ci::signals::ConnectionList mSignalConnections;
};

}  // namespace core
}  // namespace bluecadet
