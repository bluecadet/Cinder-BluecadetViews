#include "ScreenLayout.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

namespace bluecadet {
namespace core {

ScreenLayout::ScreenLayout() :
	mNumRows(1),
	mNumColumns(1),
	mDisplaySize(ci::ivec2(1920, 1080)),
	mAppSize(mDisplaySize),
	mBorderSize(2.0f),
	mBorderColor(ColorA(1.0f, 0.0f, 1.0f, 1.0f)) {
}

ScreenLayout::~ScreenLayout() {
}

void ScreenLayout::setup(const ci::ivec2& dislaySize, const int numRows, const int numColumns, const ci::ivec2 bezel) {
	mDisplaySize = dislaySize;
	mNumRows = numRows;
	mNumColumns = numColumns;
	mBezelDims = bezel;

	updateLayout();
}

void ScreenLayout::updateLayout() {
	mDisplayBounds.clear();

	for (int row = 0; row < mNumRows; ++row) {
		for (int col = 0; col < mNumColumns; ++col) {
			Rectf displayBounds = getDisplayBounds(row, col);
			mDisplayBounds.push_back(displayBounds);
		}
	}

	mAppSize = mDisplaySize * ivec2(mNumColumns, mNumRows) + mBezelDims * ivec2(mNumColumns - 1, mNumRows - 1);
	mAppSizeChanged.emit(mAppSize);
}

Rectf ScreenLayout::getDisplayBounds(const int displayId) {
	return getDisplayBounds(getRowFromDisplayId(displayId), getColFromDisplayId(displayId));
}

Rectf ScreenLayout::getDisplayBounds(const int row, const int col) {

	ivec2 addBezel = mBezelDims * ivec2(col, row);

	return Rectf(
		(float)(col * mDisplaySize.x + addBezel.x),
		(float)(row * mDisplaySize.y + addBezel.y),
		(float)((col + 1) * mDisplaySize.x + addBezel.x),
		(float)((row + 1) * mDisplaySize.y + addBezel.y));

}

void ScreenLayout::draw() {

	gl::ScopedColor scopedColor(mBorderColor);
	gl::ScopedLineWidth scopedLineWidth(mBorderSize);

	for (const auto & outline : mDisplayBounds) {
		gl::drawStrokedRect(outline);
	}
}

}
}
