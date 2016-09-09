#include "LineView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

LineView::LineView() : BaseView(),
	mLineWidth(1.0f){
}

LineView::~LineView() {
}

inline void LineView::setup(const ci::vec2 & endPoint, const ci::ColorA & lineColor, const float lineWidth) {
	setEndPoint(endPoint);
	setBackgroundColor(lineColor);
	setLineWidth(lineWidth);
}

void LineView::draw() {
	gl::enableAlphaBlending();

	ColorA lineColor = getBackgroundColor().value();
	lineColor.a = getDrawColor().a * lineColor.a;
	console() << lineColor << endl;
	console() << getDrawColor().a << endl;

	gl::ScopedColor color(lineColor);
	gl::ScopedLineWidth lineWidth(mLineWidth);
	gl::drawLine(vec2(0, 0), getEndPoint());
}

}
}
