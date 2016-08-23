//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class ScreenLayoutView> ScreenLayoutViewRef;

class ScreenLayoutView {

public:
	ScreenLayoutView();
	~ScreenLayoutView();

	static ScreenLayoutViewRef getInstance() {
		static ScreenLayoutViewRef instance = nullptr;
		if (!instance) instance = ScreenLayoutViewRef(new ScreenLayoutView());
		return instance;
	}



	//! Must be called before calling draw. Adds a key-up event listener.
	void			setup(views::BaseViewRef rootView, const ci::ivec2& dislaySize = ci::ivec2(1920, 1080), const int numRows = 1, const int numColumns = 1);



	//! Draws the current screen layout, transformed appropriately to match the position and scale of rootView
	void			draw();



	//! The width of a single display in the display matrix
	int				getDisplayWidth() const { return mDisplaySize.x; };
	void			setDisplayWidth(const int width) { mDisplaySize.x = width; updateLayout(); };

	//! The height of a single display in the display matrix
	int				getDisplayHeight() const { return mDisplaySize.y; };
	void			setDisplayHeight(const int height) { mDisplaySize.y = height; updateLayout(); };

	//! The number of rows of displays in the display matrix.
	int				getNumRows() const { return mNumRows; };
	void			setNumRows(const int numRows) { mNumRows = numRows; updateLayout(); };

	//! The number of columns of displays in the display matrix.
	int				getNumColumns() const { return mNumColumns; };
	void			setNumColumns(const int numColumns) { mNumColumns = numColumns; updateLayout(); };



	//! Helper to retrieve a display id from a row/col. Ids start at 0 and increment in right-to-left, top-to-bottom sequence.
	inline int		getDisplayId(const int row, const int col) const { return row * mNumColumns + col; };
	//! Helper to extract the column from a display id. Ids start at 0 and increment in right-to-left, top-to-bottom sequence.
	inline int		getColFromDisplayId(const int displayId) const { if (mNumColumns <= 0) return 0; return displayId % mNumColumns; };
	//! Helper to extract the row from a display id. Ids start at 0 and increment in right-to-left, top-to-bottom sequence.
	inline int		getRowFromDisplayId(const int displayId) const { if (mNumRows <= 0) return 0; return displayId / mNumColumns; };



	//! Absolute bounds of the display with the given id
	const ci::Rectf&	getDisplayBounds(const int displayId);

	//! Absolute bounds of the display at col/row
	const ci::Rectf&	getDisplayBounds(const int row, const int col);



	//! The total app size when scaled at 100%
	const ci::ivec2&	getAppSize() const { return mAppSize; };

	//! Overall app width when scaled at 100%
	int					getAppWidth() const { return getAppSize().x; }

	//! Overall app height when scaled at 100%
	int					getAppHeight() const { return getAppSize().y; }

	
	//! Zooms to fit the display at displayId into the current application window.
	void			zoomToDisplay(const int displayId);
	
	//! Zooms to fit the display at col/row into the current application window.
	void			zoomToDisplay(const int row, const int col);

	//! Zooms around a location in window coordinate space
	void			zoomAtLocation(const float scale, const ci::vec2 location);

	//! Zooms around the application window's center
	inline void		zoomAtWindowCenter(const float scale) { zoomAtLocation(scale, ci::app::getWindowCenter()); }

	//! Zooms the app to fit centered into the current window
	void			zoomToFitWindow();



	//! The border color used when drawing the display bounds. Defaults to opaque magenta.
	const ci::ColorA&	getBorderColor() { return mBorderColor; };
	void				setBorderColor(const ci::ColorA& color) { mBorderColor = color; };
	
	//! The border size used when drawing the display bounds. Defaults to 4.
	float			getBordeSize() const { return mBorderSize; }
	void			setBorderSize(const float value) { mBorderSize = value; }


protected:
	float			getScaleToFitBounds(const ci::Rectf &bounds, const ci::vec2 &maxSize, const float padding = 0.0f) const;

	void			updateLayout();
	void			handleKeyDown(ci::app::KeyEvent event);


	//! Layout
	int			mNumRows;
	int			mNumColumns;

	ci::ivec2	mDisplaySize;
	ci::ivec2	mAppSize;

	float		mBorderSize;
	ci::ColorA	mBorderColor;

private:
	//! Used to draw bounds of each display
	std::vector<ci::Rectf>	mDisplayBounds;

	// From BaseApp
	views::BaseViewRef		mRootView;
};

}
}