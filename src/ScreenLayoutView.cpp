#include "ScreenLayoutView.h"
#include "SettingsManager.h"
#include "debug/RectView.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;
using namespace bluecadet::utils;


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

void ScreenLayoutView::setup() {
	// Load display settings
	mDisplayWidth = SettingsManager::getInstance()->getField<int>("settings.display.width");
	mDisplayHeight = SettingsManager::getInstance()->getField<int>("settings.display.height");
	mDisplayTotalRows = SettingsManager::getInstance()->getField<int>("settings.display.totalRows");
	mDisplayTotalColumns = SettingsManager::getInstance()->getField<int>("settings.display.totalColumns");

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

	for (auto &displayOutline : mDisplayOutlines){
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



}
}