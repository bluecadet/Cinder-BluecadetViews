#include "LineView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

LineView::LineView() : BaseView(),
	mLineWidth(1.0f),
	mLineColor(ColorA::white()){
}

LineView::~LineView() {
}

inline void LineView::setup(const ci::vec2 & endPoint, const ci::ColorA & lineColor, const float lineWidth) {
	setEndPoint(endPoint);
	setLineColor(lineColor);
	setLineWidth(lineWidth);
}

void LineView::draw() {

	//! This was introduced because of a bug that was causing gl::ScopedColor to override the parent views tint/alpha
	ColorA correctedColor = mLineColor.value() * getDrawColor();
	gl::ScopedColor color(correctedColor);
	float screenScale = bluecadet::core::ScreenCamera::getInstance()->getScale().x;
	gl::ScopedLineWidth lineWidth(mLineWidth * screenScale);
	gl::drawLine(vec2(0, 0), getEndPoint());
}

}
}
