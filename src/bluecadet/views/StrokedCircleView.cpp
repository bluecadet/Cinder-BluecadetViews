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

void StrokedCircleView::setup(float radius, ci::ColorA strokeColor, float strokeWidth, float smoothness) {
	setRadius(radius);
	setStrokeColor(strokeColor);
	setSmoothness(smoothness);
	setStrokeWidth(strokeWidth);
}


void StrokedCircleView::setRadius(const float radius) {
	setSize(vec2(2.0f * radius));
}

void StrokedCircleView::draw() {
	const auto & strokeColor = mStrokeColor.value() * getDrawColor();
	const auto & backgroundColor = getBackgroundColor().value() * getDrawColor();
	const auto & size = getSize();

	if ((size.x <= 0 || size.y <= 0) && strokeColor.a <= 0 && backgroundColor.a <= 0) {
		return;
	}

	auto batch = getSharedBatch();
	auto prog = batch->getGlslProg();

	prog->uniform("uSize", getSize());
	prog->uniform("uStrokeColor", strokeColor);
	prog->uniform("uBackgroundColor", backgroundColor);
	prog->uniform("uSmoothness", mSmoothness);
	prog->uniform("uStrokeWidth", mStrokeWidth);

	batch->draw();
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

				in vec4			ciPosition;
				//out vec4		vNormPosition;
				out vec4		vAbsPosition;

				void main(void) {
					//vNormPosition = ciPosition;
					vAbsPosition = vec4(
						ciPosition.x * uSize.x - uSize.x * 0.5f,
						ciPosition.y * uSize.y - uSize.y * 0.5f,
						0.0f, 1.0f);
					gl_Position = ciModelViewProjection * vAbsPosition;
				}
			))
			.fragment(CI_GLSL(150,

				uniform vec2	uSize;
				uniform float	uSmoothness;
				uniform float	uStrokeWidth;
				uniform vec4	uStrokeColor;
				uniform vec4	uBackgroundColor;

				//in vec4		vNormPosition;
				in vec4			vAbsPosition;
				out vec4		oColor;

				void main(void) {
					/*vec2 normalizedDelta = vNormPosition.xy * 2.0f - vec2(1.0f);
					float normalizedRadiusSq = normalizedDelta.x * normalizedDelta.x + normalizedDelta.y * normalizedDelta.y;
					if (normalizedRadiusSq > 1.0f) discard;

					float normalizedRadius = sqrt(normalizedRadiusSq);*/

					//The following radii are in descending order of size from a to d (from outside in)
					float radius = length(vAbsPosition);

					float radiusA = min(uSize.x, uSize.y) * 0.5f;
					float radiusD = radiusA - uStrokeWidth - uSmoothness * 2.0f;

					if (radius > radiusA) discard;

					float radiusB = radiusA - uSmoothness;
					float radiusC = radiusD + uSmoothness;

					oColor = uStrokeColor;

					if (uSmoothness > 0 && radius > radiusB) {
						// interpolate smooth edge
						float quota = (radiusA - radius) / uSmoothness;
						float smoothQuota = smoothstep(0.0f, 1.0f, quota);
						oColor = mix(vec4(0), oColor, smoothstep(0.0f, 1.0f, smoothQuota));

					} else if (uSmoothness > 0 && radius < radiusC) {
						// interpolate smooth edge
						float quota = (radiusC - radius) / uSmoothness;
						float smoothQuota = smoothstep(1.0f, 0.0f, quota);
						oColor = mix(uBackgroundColor, oColor, smoothQuota);
					}

					oColor.rgb /= oColor.a;

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
