#include "EllipseView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

EllipseView::EllipseView() : BaseView()
{
}

EllipseView::~EllipseView() {
}

void EllipseView::setup(const float radius, const ci::ColorA backgroundColor) {
	setRadius(radius);
	setBackgroundColor(backgroundColor);
}

void EllipseView::setup(const ci::vec2& size, const ci::ColorA backgroundColor) {
	setSize(size);
	setBackgroundColor(backgroundColor);
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

				in vec4			normPosition;
				in vec4			color;
				out vec4		oColor;

				void main(void) {
					vec2 delta = normPosition.xy - vec2(0.5f);
					float rSq = delta.x * delta.x + delta.y * delta.y;
					// max radius is 0.5, but since we're using rSq, which is essentially r * r,
					// we want to compare it with 0.5 * 0.5, which is 0.25
					if (rSq > 0.25f) discard;
					oColor = color;
				}
			))
		);
		prog->uniform("uSize", vec2(0));
		prog->uniform("uBackgroundColor", vec4(0));
	}
	return prog;
}



}
}
