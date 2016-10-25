//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ScreenLayout.h"
#include <views/BaseView.h>

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
	void				setup(ScreenLayoutRef layout);

	//! Dispatched whenever the current viewport (pan/zoom) is changed.
	ci::signals::Signal<void(const ci::Area & viewport)> & getViewportChangedSignal() { return mViewportChangedSignal; };

	//! Zooms to fit the display at displayId into the current application window.
	void				zoomToDisplay(const int displayId);
	
	//! Zooms to fit the display at col/row into the current application window.
	void				zoomToDisplay(const int row, const int col);

	//! Zooms around a location in window coordinate space
	void				zoomAtLocation(const float scale, const ci::vec2 location);

	//! Zooms around the application window's center
	inline void			zoomAtWindowCenter(const float scale) { zoomAtLocation(scale, ci::app::getWindowCenter()); }

	//! Zooms the app to fit centered into the current window
	void				zoomToFitWindow();

	//! The current camera transform.
	const ci::mat4&		getTransform() const { return mPlaceholderView->getTransform(); };

	//! The current viewport in app coordinates.
	const ci::Area &	getViewport() const { return mViewport; }

protected:
	float				getScaleToFitBounds(const ci::Rectf &bounds, const ci::vec2 &maxSize, const float padding = 0.0f) const;

	void				updateViewport();
	void				handleWindowResize();
	void				handleKeyDown(ci::app::KeyEvent event);

	ci::Area			mViewport;
	views::BaseViewRef	mPlaceholderView;
	ScreenLayoutRef		mLayout;

	ci::signals::Signal<void(const ci::Area & viewport)> mViewportChangedSignal;
};

}
}
