#include "ShapeView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

//==================================================
// Class Lifecycle
// 
ShapeView::ShapeView() : BaseView(),
mLineWidth(0.0f)
{
}

ShapeView::~ShapeView() {
}

void ShapeView::reset() {
	if (mSolidCircle) mSolidCircle.reset();
	if (mLine) {
		setScale(vec2(0.0f, 1.0f));
		mLine.reset();
	}
}

void ShapeView::createLine(const ci::vec2 &startPos, const ci::vec2 &endPos, float lineWidth) {
	mLineWidth = lineWidth;

	ci::gl::VertBatch mLineBatch = ci::gl::VertBatch(GL_LINE_STRIP);
	mLineBatch.vertex(startPos);
	mLineBatch.vertex(endPos);
	mLine = ci::gl::Batch::create(mLineBatch, ci::gl::getStockShader(ci::gl::ShaderDef().color()));
}

void ShapeView::createSolidCircle(const ci::vec2 &pos, float radius) {
	setPosition(pos);
	gl::GlslProgRef solidShader = gl::getStockShader(gl::ShaderDef().color());
	mSolidCircle = gl::Batch::create(geom::Circle().radius(radius), solidShader);
}

void ShapeView::animateOn(float alpha, float aniDur, float aniDelay) {
	if (mLine) {
		getTimeline()->apply(&getScale(), vec2(1.0f), aniDur, EaseOutSine()).delay(aniDelay);
		getTimeline()->apply(&getAlpha(), alpha, aniDur, easeOutQuad).delay(aniDelay);
	}
	else {
		getTimeline()->apply(&getAlpha(), alpha, aniDur, easeOutQuad).delay(aniDelay);
	}
}

void ShapeView::animateOff(float alpha, float aniDur, float aniDelay) {
	if (mLine) {
		getTimeline()->apply(&getScale(), vec2(0.0f), aniDur, easeOutQuad).delay(aniDelay);
		getTimeline()->apply(&getAlpha(), alpha, aniDur, easeOutQuad).delay(aniDelay);
	}
	else {
		getTimeline()->apply(&getAlpha(), alpha, aniDur, easeOutQuad).delay(aniDelay);
	}
}

void ShapeView::draw() {

	if (mLineWidth > 0) {
		gl::lineWidth(mLineWidth);
		if (mLine) mLine->draw();
	}

	if (mSolidCircle) mSolidCircle->draw();
}

}
}