#include "StrokedRoundedRectView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

StrokedRoundedRectView::StrokedRoundedRectView() : BaseView() {
}

StrokedRoundedRectView::~StrokedRoundedRectView() {
}

void StrokedRoundedRectView::cancelAnimations() {
	BaseView::cancelAnimations();
	mSmoothness.stop();
	mStrokeColor.stop();
	mStrokeWidth.stop();
}

void StrokedRoundedRectView::draw() {
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
	prog->uniform("uCornerRadius", mCornerRadius);
	prog->uniform("uSize", size);

	batch->draw();
}

ci::gl::BatchRef StrokedRoundedRectView::getSharedBatch() {
	static ci::gl::BatchRef batch = nullptr;
	if (!batch) {
		auto rect = geom::Rect().rect(Rectf(0, 0, 1, 1));
		batch = gl::Batch::create(rect, getSharedProg());
	}
	return batch;
}

ci::gl::GlslProgRef StrokedRoundedRectView::getSharedProg() {
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
				uniform float uSmoothness = 1.0;
				uniform float uStrokeWidth = 1.0;
				uniform float uCornerRadius = 0.0;
				out vec4 oColor;

				float rectSdf(vec2 p, vec2 b, float r) {
					vec2 d = abs(p) - b + vec2(r);
					return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r;
				}

				// Based on https://www.shadertoy.com/view/ltS3zW
				void main(void) {
					float halfStrokeWidth = uStrokeWidth * 0.5;
					vec2 halfSize = uSize / 2.0 - vec2(halfStrokeWidth);
					vec2 centerPos = (vPosition.xy - uSize.xy * 0.5);

					float fieldDistance = rectSdf(centerPos, halfSize, uCornerRadius - halfStrokeWidth);

					vec4 fromColor = uBackgroundColor;
					vec4 toColor = vec4(0.0);

					if (halfStrokeWidth > 0.0) {
						fromColor = uStrokeColor;

						if (fieldDistance < 0.0) {
							toColor = uBackgroundColor;
						}

						fieldDistance = abs(fieldDistance) - halfStrokeWidth;
					}

					float blendAmount = smoothstep(-uSmoothness, uSmoothness, fieldDistance);

					oColor = mix(fromColor, toColor, blendAmount);
					
					if (oColor.a <= 0) {
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
