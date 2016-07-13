#include "RectButton.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {
namespace debug {

RectButton::RectButton() : TouchView() {
}

RectButton::~RectButton() {
	
}

void RectButton::setup(const vec2& size, const ci::ColorA& touchUpColor, const ci::ColorA& touchDownColor) {
	TouchView::setup(size);

	mTouchUpColor = touchUpColor;
	mTouchDownColor = touchDownColor;

	Rectf rect = Rectf(0.0f, 0.0f, size.x, size.y);
	auto shaderDef = gl::ShaderDef().color();
	gl::GlslProgRef shader = gl::getStockShader(shaderDef);
	mRectBatch = gl::Batch::create(geom::Rect(rect), shader);
}

void RectButton::update(double deltaTime) {
	setColor((getNumTouches() > 0) ? mTouchDownColor : mTouchUpColor);
}

void RectButton::draw() {
	mRectBatch->draw();
}

}
}
}