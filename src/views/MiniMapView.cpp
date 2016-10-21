#include "MiniMapView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

MiniMapView::MiniMapView(const float mapScale, const float padding) :
	mRequiresContentUpdate(true),
	mCols(0),
	mRows(0),
	mDisplaySize(0),
	mMapScale(mapScale),
	mPadding(padding),
	mViewportView(new BaseView())
{
	mViewportView->setBackgroundColor(ColorA(0.5f, 1, 0.5f, 0.5f));
	addChild(mViewportView);
	setupShaders();
}

MiniMapView::~MiniMapView() {
}

void MiniMapView::setLayout(const int cols, const int rows, const ci::ivec2 & displaySize) {
	mCols = cols;
	mRows = rows;
	mDisplaySize = displaySize;

	mAppSize = mDisplaySize * ivec2(mCols, mRows);
	mScaledSize = ivec2(round(vec2(mAppSize) * mMapScale));

	setSize(vec2(mScaledSize) + 2.0f * vec2(mPadding));

	mFbo = gl::Fbo::create((int)mScaledSize.x, (int)mScaledSize.y, true, false, false);
}

void MiniMapView::setViewport(const ci::Area & viewport) {
	vec2 topLeft = vec2(viewport.getUL()) * mMapScale;
	vec2 size = vec2(viewport.getSize()) * mMapScale;
	
	mViewportView->setPosition(topLeft);
	mViewportView->setSize(size);

	mRequiresContentUpdate = true;
}

void MiniMapView::draw() {
	updateContent();
	gl::draw(mFbo->getColorTexture());
}

void MiniMapView::updateContent() {
	if (!mRequiresContentUpdate) {
		return;
	}

	gl::ScopedViewport scopedViewport(ivec2(0), mScaledSize);
	gl::ScopedMatrices scopedMatrices;
	gl::setMatricesWindow(mScaledSize);

	mFbo->bindFramebuffer();

	gl::clear(ColorA(0, 0, 0, 0.25));
	gl::ScopedColor scopedColor(Color::white());

	mGlsl->uniform("uAppSize", mScaledSize);
	mGlsl->uniform("uScaledSize", mScaledSize);
	mGlsl->uniform("uDisplaySize", mDisplaySize);
	mGlsl->uniform("uCols", mCols);
	mGlsl->uniform("uRows", mRows);

	gl::translate(vec2(mPadding));
	mBatch->draw();

	mFbo->unbindFramebuffer();

	mRequiresContentUpdate = false;
}

void MiniMapView::setupShaders() {
	mGlsl = gl::GlslProg::create(gl::GlslProg::Format().vertex(CI_GLSL(150,
		uniform vec2	uAppSize;
		uniform vec2	uScaledSize;
		uniform vec2	uDisplaySize;
		uniform vec2	uCols;
		uniform vec2	uRows;

		uniform mat4	ciModelViewProjection;
		in vec4			ciPosition;
		in vec4			ciColor;
		out vec4		color;

		void main(void) {
			vec4 pos = vec4(ciPosition.x * uScaledSize.x, ciPosition.y * uScaledSize.y, 0.0f, 1.0f);
			gl_Position = ciModelViewProjection * pos;
			color = ciColor;
		}
	)).fragment(CI_GLSL(150,
		in vec4			color;
		out vec4		oColor;

		void main(void) {
			oColor = color;
		}
	)));

	mBatch = gl::Batch::create(geom::Rect(Rectf(0, 0, 1, 1)), mGlsl);
}


}
}