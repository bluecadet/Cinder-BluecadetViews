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
namespace views {

typedef std::shared_ptr<class ScreenLayoutView> ScreenLayoutViewRef;

class ScreenLayoutView : public bluecadet::views::BaseView{

public:
	ScreenLayoutView();
	~ScreenLayoutView();

	void setup();
//	virtual void update(double deltaTime) override;
	virtual void draw() override;

	//! Display functions - may not need
	void		setDisplayWidth(const int& width) { mDisplayWidth = width; };
	const int	getDisplayWidth() { return mDisplayWidth; };

	void		setDisplayHeight(const int& height) { mDisplayHeight = height; };
	const int	getDisplayHeight() { return mDisplayHeight; };

	void		setDisplayTotalRows(const int& totalRows) { mDisplayTotalRows = totalRows; };
	const int	getDisplayTotalRows() { return mDisplayTotalRows; };

	void		setDisplayTotalColumns(const int& totalColumns) { mDisplayTotalColumns = totalColumns; };
	const int	getDisplayTotalColumns() { return mDisplayTotalColumns; };


	const void	setBorderColor(const ci::ColorA& color) { mBorderColor = color; };

	ci::Rectf getDisplayBounds(const int& displayId);

//testing
	const float getScaleToFitBounds(ci::Rectf bounds, ci::vec2 maxSize, float padding = 0.0f);
	const ci::vec2 getTranslateToCenterBounds(ci::Rectf bounds, ci::vec2 maxSize);

protected:

	ci::Rectf getDisplayBounds(const int& column, const int& row);
	
	//! Display
	int			mDisplayWidth;
	int			mDisplayHeight;
	int			mDisplayTotalRows;
	int			mDisplayTotalColumns;

	float		mBorderWidth;
	ci::ColorA	mBorderColor;

private:

	//! Used to draw bounds of each screen & 
	std::vector<std::pair<int, ci::Rectf>> mDisplayOutlines;

};

}
}