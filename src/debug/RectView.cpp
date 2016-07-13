#include "RectView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {
namespace debug {

//==================================================
// Lifecycle
// 

RectView::RectView(const vec2& size, const ci::ColorA& color) : views::BaseView() {
	setColor(color);
	Rectf rect = Rectf(0.0f, 0.0f, size.x, size.y);
	auto shaderDef = gl::ShaderDef().color();
	gl::GlslProgRef shader = gl::getStockShader(shaderDef);
	mRectBatch = gl::Batch::create(geom::Rect(rect), shader);
}

RectView::~RectView() {
}

void RectView::draw() {
	mRectBatch->draw();
}

}
}
}