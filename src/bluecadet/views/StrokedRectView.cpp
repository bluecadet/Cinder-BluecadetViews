#include "StrokedRectView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

StrokedRectView::StrokedRectView() : BaseView() {
}

StrokedRectView::~StrokedRectView() {
}

void StrokedRectView::cancelAnimations() {
	BaseView::cancelAnimations();
	mSmoothness.stop();
	mStrokeColor.stop();
	mStrokeWidth.stop();
}

void StrokedRectView::draw() {
	const auto & strokeColor = mStrokeColor.value() * getDrawColor();
	const auto & bgColor = getBackgroundColor().value() * getDrawColor();
	const auto & size = getSize();

	if (size.x <= 0 && size.y <= 0 && (bgColor.a <= 0 || strokeColor.a <= 0)) {
		return;
	}

	auto batch = getSharedBatch();
	auto prog = batch->getGlslProg();

	prog->uniform("uBackgroundColor", bgColor);
	prog->uniform("uSmoothness", mSmoothness);
	prog->uniform("uStrokeWidth", mStrokeWidth);
	prog->uniform("uStrokeColor", strokeColor);
	prog->uniform("uSize", size);

	batch->draw();
}

ci::gl::BatchRef StrokedRectView::getSharedBatch() {
	static ci::gl::BatchRef batch = nullptr;
	if (!batch) {
		auto rect = geom::Rect().rect(Rectf(0, 0, 1, 1));
		batch = gl::Batch::create(rect, getSharedProg());
	}
	return batch;
}

ci::gl::GlslProgRef StrokedRectView::getSharedProg() {
	static ci::gl::GlslProgRef prog = nullptr;
	if (!prog) {
		prog = gl::GlslProg::create(
			gl::GlslProg::Format()
			.vertex(CI_GLSL(150,
				uniform mat4 ciModelViewProjection;
				uniform vec2 uSize;

				in vec4 ciPosition;
				in vec4 ciColor;
				out vec4 vPosition;
				out vec4 vColor;

				void main(void) {
					vec4 pos = ciPosition;
					pos.xy *= uSize;
					gl_Position = ciModelViewProjection * pos;
					vPosition = pos;
				}
			)).fragment(CI_GLSL(150,
				in vec4 vPosition;
				in vec4 vColor;
				uniform vec2 uSize;
				uniform vec4 uStrokeColor;
				uniform vec4 uBackgroundColor;
				uniform float uSmoothness = 0.5;
				uniform float uStrokeWidth = 1.0;
				out vec4 oFragColor;

				void main(void) {
					oFragColor = uBackgroundColor;

					float borderAlpha = 1.0;
					borderAlpha *= smoothstep(0, uSmoothness, vPosition.x - uStrokeWidth);
					borderAlpha *= 1.0 - smoothstep(0, uSmoothness, vPosition.x - (uSize.x - uStrokeWidth));
					borderAlpha *= smoothstep(0, uSmoothness, vPosition.y - uStrokeWidth);
					borderAlpha *= 1.0 - smoothstep(0, uSmoothness, vPosition.y - (uSize.y - uStrokeWidth));
					borderAlpha = 1.0 - borderAlpha;

					if (uBackgroundColor.a > 0) {
						oFragColor = mix(uBackgroundColor, uStrokeColor, borderAlpha);

					} else {
						oFragColor = uStrokeColor * vec4(1.0, 1.0, 1.0, borderAlpha);

					}

					if (oFragColor.a <= 0) {
						discard;
					}
				}

			))
		);
	}
	return prog;
}



}
}
