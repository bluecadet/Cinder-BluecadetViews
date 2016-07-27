#include "LineView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

LineView::LineView() : BaseView(),
	mLineWidth(1.0f),
	mLineColor(ColorA::white())
{
}

LineView::~LineView() {
}

void LineView::draw() {
	gl::ScopedColor color(mLineColor);
	gl::ScopedLineWidth lineWidth(mLineWidth);

	gl::drawLine(vec2(0, 0), getEndPoint());
}

}
}
