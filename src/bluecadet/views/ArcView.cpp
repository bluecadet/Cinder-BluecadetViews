#include "ArcView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

ArcView::ArcView() : BaseView() {
}

ArcView::~ArcView() {
}

void ArcView::setup(float innerRadius, float outerRadius, float startAngle, float endAngle, ci::ColorA backgroundColor) {
	setInnerRadius(innerRadius);
	setOuterRadius(outerRadius);
	setStartAngle(startAngle);
	setEndAngle(endAngle);
	setBackgroundColor(backgroundColor);
}

void ArcView::cancelAnimations() {
	BaseView::cancelAnimations();
	mSmoothness.stop();
	mInnerRadius.stop();
	mOuterRadius.stop();
	mStartAngle.stop();
	mEndAngle.stop();
}

void ArcView::update(const FrameInfo & info) {
	const auto & size = getSize();
	const float diameter = 2.0f * mOuterRadius;
	if (size.x != diameter || size.y != diameter) {
		setSize(vec2(diameter));
	}
}

void ArcView::draw() {
	const auto& bgColor = getBackgroundColor().value();
	const auto& size = getSize();

	if (size.x <= 0 && size.y <= 0 && bgColor.a <= 0) {
		return;
	}

	auto batch = getSharedBatch();
	auto prog = batch->getGlslProg();

	prog->uniform("uBackgroundColor", bgColor);
	prog->uniform("uSmoothness", mSmoothness);
	prog->uniform("uInnerRadius", mInnerRadius);
	prog->uniform("uOuterRadius", mOuterRadius);
	prog->uniform("uStartAngle", mStartAngle);
	prog->uniform("uEndAngle", mEndAngle);
	prog->uniform("uSize", getSize());

	batch->draw();
}

ci::gl::BatchRef ArcView::getSharedBatch() {
	static ci::gl::BatchRef batch = nullptr;
	if (!batch) {
		auto rect = geom::Rect().rect(Rectf(0, 0, 1, 1));
		batch = gl::Batch::create(rect, getSharedProg());
	}
	return batch;
}

ci::gl::GlslProgRef ArcView::getSharedProg() {
	static ci::gl::GlslProgRef prog = nullptr;
	if (!prog) {
		prog = gl::GlslProg::create(
			gl::GlslProg::Format()
			.vertex(CI_GLSL(150,
				uniform mat4 ciModelViewProjection;
				uniform vec2 uSize;
				uniform vec4 uBackgroundColor;

				in vec4 ciPosition;
				in vec4 ciColor;
				out vec4 vPosition;
				out vec4 vColor;

				void main(void) {
					gl_Position = ciModelViewProjection * vec4(
						ciPosition.x * uSize.x - uSize.x * 0.5f,
						ciPosition.y * uSize.y - uSize.y * 0.5f,
						0.0f, 1.0f);
					vPosition = vec4(ciPosition.xy * uSize, 0.0, 1.0);
					vColor = ciColor * uBackgroundColor;
				}
			))
			.fragment(CI_GLSL(150,
				in vec4 vPosition;
				in vec4 vColor;
				uniform vec2 uSize;
				uniform float uSmoothness = 4.0;
				uniform float uInnerRadius = 0.5;
				uniform float uOuterRadius = 1.0;
				uniform float uStartAngle = 0.33 * M_PI;
				uniform float uEndAngle = 0.66 * M_PI;
				out vec4 oFragColor;

				void main(void) {
					float startAngle = clamp(uStartAngle, 0.0, uEndAngle);
					float endAngle = clamp(uEndAngle, uStartAngle, M_TWO_PI);

					float normSmooth = uSmoothness / uOuterRadius;
					vec2 centerPos = vPosition.xy - uSize * 0.5;

					// Inspired by http://blog.lapingames.com/draw-arc-in-shader/
					float dist = length(centerPos);
					float inner = smoothstep(uInnerRadius, uInnerRadius + dist * normSmooth, dist);
					float outer = smoothstep(uOuterRadius, uOuterRadius - dist * normSmooth, dist);
					float bandAlpha = inner * outer;

					float angle = (atan(centerPos.y, centerPos.x) + M_PI);
					float startEdge = smoothstep(angle, angle - normSmooth, startAngle);
					float endEdge = smoothstep(angle, angle + normSmooth, endAngle);
					float angleAlpha = startEdge * endEdge;

					float alpha = bandAlpha * angleAlpha * vColor.a;

					if (alpha <= 0) {
						discard;
					}

					oFragColor = vec4(vColor.rgb, alpha);
				}

			))
			.define("M_PI", to_string(glm::pi<float>()))
			.define("M_TWO_PI", to_string(glm::two_pi<float>()))
		);
	}
	return prog;
}



}
}
