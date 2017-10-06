#include "StrokedCircleView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

StrokedCircleView::StrokedCircleView() : BaseView(),
mSmoothness(1.0f),
mStrokeWidth(1.0f) {
}

StrokedCircleView::~StrokedCircleView() {
}

void StrokedCircleView::setup(float radius, ci::ColorA backgroundColor, float strokeWidth, float smoothness) {
	setRadius(radius);
	setBackgroundColor(backgroundColor);
	setSmoothness(smoothness);
	setStrokeWidth(strokeWidth);
}

void StrokedCircleView::setup(ci::vec2 size, ci::ColorA backgroundColor, float strokeWidth, float smoothness) {
	setSize(size);
	setBackgroundColor(backgroundColor);
	setSmoothness(smoothness);
	setStrokeWidth(strokeWidth);
}


void StrokedCircleView::setRadius(const float radius) {
	setSize(vec2(2.0f * radius));
}

void StrokedCircleView::draw() {
	const auto& bgColor = getBackgroundColor().value();
	const auto& size = getSize();

	if (size.x <= 0 && size.y <= 0 && bgColor.a <= 0) {
		return;
	}

	auto batch = getSharedBatch();
	auto prog = batch->getGlslProg();
	prog->uniform("uSize", getSize());
	prog->uniform("uBackgroundColor", vec4(bgColor.r, bgColor.g, bgColor.b, bgColor.a));
	prog->uniform("uSmoothness", mSmoothness);
	prog->uniform("uStrokeWidth", mStrokeWidth);
	batch->draw();
}

void StrokedCircleView::debugDrawOutline() {
	gl::ScopedModelMatrix scopedModelMatrix;
	gl::ScopedViewMatrix scopedViewMatrix;

	gl::translate(-getSize() * 0.5f);

	BaseView::debugDrawOutline();
}

ci::gl::BatchRef StrokedCircleView::getSharedBatch() {
	static ci::gl::BatchRef batch = nullptr;
	if (!batch) {
		auto rect = geom::Rect().rect(Rectf(0, 0, 1, 1));
		batch = gl::Batch::create(rect, getSharedProg());
	}
	return batch;
}

ci::gl::GlslProgRef StrokedCircleView::getSharedProg() {
	static ci::gl::GlslProgRef prog = nullptr;
	if (!prog) {
		prog = gl::GlslProg::create(gl::GlslProg::Format()
			.vertex(CI_GLSL(150,
				uniform vec2	uSize;
				uniform mat4	ciModelViewProjection;
				uniform vec4	uBackgroundColor;

				in vec4			ciPosition;
				in vec4			ciColor;
				out vec4		color;
				out vec4		normPosition;
				out vec4		absPosition;

				void main(void) {
					color = ciColor * uBackgroundColor;
					normPosition = ciPosition;
					absPosition = vec4(
						ciPosition.x * uSize.x - uSize.x * 0.5f,
						ciPosition.y * uSize.y - uSize.y * 0.5f,
						0.0f, 1.0f);
					gl_Position = ciModelViewProjection * absPosition;
				}
			))
			.fragment(CI_GLSL(150,

				uniform vec2	uSize;
				uniform float	uSmoothness;
				uniform float	uStrokeWidth;

				in vec4			normPosition;
				in vec4			absPosition;
				in vec4			color;
				out vec4		oColor;

				void main(void) {
					/*vec2 normalizedDelta = normPosition.xy * 2.0f - vec2(1.0f);
					float normalizedRadiusSq = normalizedDelta.x * normalizedDelta.x + normalizedDelta.y * normalizedDelta.y;
					if (normalizedRadiusSq > 1.0f) discard;

					float normalizedRadius = sqrt(normalizedRadiusSq);*/

					//The following radii are in descending order of size from a to d (from outside in)
					float radius = length(absPosition);

					float radiusA = min(uSize.x, uSize.y) * 0.5f;
					float radiusD = radiusA - uStrokeWidth - uSmoothness * 2.0f;

					if (radius > radiusA || radius < radiusD) discard;

					float radiusB = radiusA - uSmoothness;
					float radiusC = radiusD + uSmoothness;

					oColor = color;

					// interpolate smooth edge
					if (uSmoothness > 0 && radius > radiusB) {
						float quota = (radiusA - radius) / uSmoothness;
						oColor.w = color.w * smoothstep(0.0f, 1.0f, quota);
					}

					if (uSmoothness > 0 && radius < radiusC) {
						float quota = (radiusC - radius) / uSmoothness;
						oColor.w = color.w * smoothstep(1.0f, 0.0f, quota);
					}
				}
			))
		);
		prog->uniform("uSize", vec2(0));
		prog->uniform("uSmoothness", 1.0f);
		prog->uniform("uBackgroundColor", vec4(0));
		prog->uniform("uStrokeWidth", 1.0f);
	}
	return prog;
}



}
}
