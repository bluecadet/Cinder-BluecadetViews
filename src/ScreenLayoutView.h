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
	~ScreenLayoutView();

	//! Singleton
	static ScreenLayoutViewRef getInstance() {
		static ScreenLayoutViewRef instance = nullptr;
		if (!instance) instance = ScreenLayoutViewRef(new ScreenLayoutView());
		return instance;
	}

	void setup(views::BaseViewRef baseRootView, const ci::ivec2& dislaySize = ci::ivec2(1920, 1080), const int numRows = 1, const int numColumns = 1);
	void draw();

	//! Display
	void			setDisplayWidth(const int width) { mDisplaySize.x = width; };
	int				getDisplayWidth() const { return mDisplaySize.x; };

	void			setDisplayHeight(const int height) { mDisplaySize.y = height; };
	int				getDisplayHeight() const { return mDisplaySize.y; };

	void			setNumRows(const int numRows) { mNumRows = numRows; };
	int				getNumRows() const { return mNumRows; };

	void			setNumColumns(const int numColumns) { mNumColumns = mNumColumns; };
	int				getNumColumns() const { return mNumColumns; };
	
	ci::Rectf		getDisplayBounds(const int displayId);
	ci::Rectf		getDisplayBounds(const int col, const int row);

	// Full app 
	int					getAppWidth() const { return mAppSize.x; }
	int					getAppHeight() const { return mAppSize.y; }
	const ci::ivec2&	getAppSize() const { return mAppSize; };


	// Debugging
	const float		getScaleToFitBounds(const ci::Rectf &bounds, const ci::vec2 &maxSize, const float padding = 0.0f);
	const ci::vec2	getTranslateToCenterBounds(const ci::Rectf &bounds, const ci::vec2& maxSize);
	void			zoomToDisplay(const int displayId);
	void			zoomToDisplay(const int col, const int row);
	void			scaleRootViewCentered(const float targetScale);

	void				setBorderColor(const ci::ColorA& color) { mBorderColor = color; };
	const ci::ColorA&	getBorderColor() { return mBorderColor; };

	inline int		getColFromDisplayId(const int displayId) const { if (displayId < 0 || mNumColumns <= 0 || mNumRows <= 0) return 0; return displayId % mNumColumns; };
	inline int		getRowFromDisplayId(const int displayId) const { if (displayId < 0 || mNumColumns <= 0 || mNumRows <= 0) return 0; return displayId / mNumRows; };

protected:
	ScreenLayoutView();
	void keyDown(ci::app::KeyEvent event);


	//! Layout
	int			mNumRows;
	int			mNumColumns;

	ci::ivec2	mDisplaySize;
	ci::ivec2	mAppSize;

	float		mBorderWidth;
	ci::ColorA	mBorderColor;

private:
	//! Used to draw bounds of each screen & 
	std::vector<ci::Rectf>	mDisplayOutlines;

	// From BaseApp
	views::BaseViewRef		mRootView;
};

}
}