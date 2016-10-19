#include "EllipseView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

EllipseView::EllipseView() : BaseView(),
mSmoothness(1.0f)
{
}

EllipseView::~EllipseView() {
}

void EllipseView::setup(const float radius, const ci::ColorA backgroundColor, const float smoothness) {
	setRadius(radius);
	setBackgroundColor(backgroundColor);
	setSmoothness(smoothness);
}

void EllipseView::setup(const ci::vec2& size, const ci::ColorA backgroundColor, const float smoothness) {
	setSize(size);
	setBackgroundColor(backgroundColor);
	setSmoothness(smoothness);
}


void EllipseView::setRadius(const float radius)
{
	setSize(vec2(2.0f * radius));
}

void EllipseView::draw() {
	const auto& bgColor = getBackgroundColor().value();
	const auto& size = getSize();

	if (size.x <= 0 && size.y <= 0 && bgColor.a <= 0) {
		return;
	}

	auto batch = getSharedEllipseBatch();
	auto prog = batch->getGlslProg();
	prog->uniform("uSize", getSize());
	prog->uniform("uBackgroundColor", vec4(bgColor.r, bgColor.g, bgColor.b, bgColor.a));
	prog->uniform("uSmoothness", mSmoothness);
	batch->draw();
}

ci::gl::BatchRef EllipseView::getSharedEllipseBatch() {
	static ci::gl::BatchRef batch = nullptr;
	if (!batch) {
		auto rect = geom::Rect().rect(Rectf(0, 0, 1, 1));
		batch = gl::Batch::create(rect, getSharedEllipseProg());
	}
	return batch;
}

ci::gl::GlslProgRef EllipseView::getSharedEllipseProg() {
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

				void main(void) {
					color = ciColor * uBackgroundColor;
					normPosition = ciPosition;
					gl_Position = ciModelViewProjection * vec4(
						ciPosition.x * uSize.x - uSize.x * 0.5f,
						ciPosition.y * uSize.y - uSize.y * 0.5f,
						0.0f, 1.0f);
				}
			)).fragment(CI_GLSL(150,

				uniform vec2	uSize;
				uniform float	uSmoothness;

				in vec4			normPosition;
				in vec4			color;
				out vec4		oColor;

				void main(void) {
					vec2 normalizedDelta = normPosition.xy * 2.0f - vec2(1.0f);
					float normalizedRadiusSq = normalizedDelta.x * normalizedDelta.x + normalizedDelta.y * normalizedDelta.y;
					if (normalizedRadiusSq > 1.0f) discard;

					float normalizedRadius = sqrt(normalizedRadiusSq);
					float maxRadius = length(uSize) * 0.5f;
					float minRadius = max(0, maxRadius - uSmoothness);
					float radius = maxRadius * normalizedRadius;

					oColor = color;

					// interpolate smooth edge
					if (uSmoothness > 0 && radius > minRadius) {
						float quota = (maxRadius - radius) / uSmoothness;
						oColor.w = color.w * smoothstep(0.0f, 1.0f, quota);
					}
				}
			))
		);
		prog->uniform("uSize", vec2(0));
		prog->uniform("uSmoothness", 1.0f);
		prog->uniform("uBackgroundColor", vec4(0));
	}
	return prog;
}



}
}
