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
	mViewportView(new BaseView()),
	mBorderColor(ColorA(1, 1, 1, 0.75f))
{
	setBackgroundColor(ColorA(0, 0, 0, 0.25f));
	mViewportView->setBackgroundColor(ColorA(1, 1, 1, 0.25f));
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
	gl::translate(vec2(mPadding));
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

	gl::clear(getBackgroundColor().value());

	mGlsl->uniform("uAppSize", vec2(mAppSize));
	mGlsl->uniform("uScaledSize", vec2(mScaledSize));
	mGlsl->uniform("uDisplaySize", vec2(mDisplaySize));
	mGlsl->uniform("uBorderColor", mBorderColor);

	mBatch->draw();

	mFbo->unbindFramebuffer();

	mRequiresContentUpdate = false;
}

void MiniMapView::setupShaders() {
	mGlsl = gl::GlslProg::create(
		gl::GlslProg::Format().vertex(CI_GLSL(150,
			uniform vec2	uAppSize;
			uniform vec2	uScaledSize;
			uniform mat4	ciModelViewProjection;
			in vec4			ciPosition;
			out vec2		vAppPosition;

			void main(void) {
				vAppPosition = ciPosition.xy * uAppSize;
				vec4 pos = vec4(ciPosition.x * uScaledSize.x, ciPosition.y * uScaledSize.y, 0.0f, 1.0f);
				gl_Position = ciModelViewProjection * pos;
			}
		)).fragment(CI_GLSL(150,
			uniform vec2	uAppSize;
			uniform vec2	uScaledSize;
			uniform vec2	uDisplaySize;
			uniform vec4	uBackgroundColor;
			uniform vec4	uBorderColor;
			in vec2			vAppPosition;
			out vec4		oColor;

			void main(void) {
				vec2 scale = uScaledSize / uAppSize;
				vec2 edgeDist = abs(mod(vAppPosition, uDisplaySize)) * scale;

				if (edgeDist.x <= 1.0 || edgeDist.x >= uDisplaySize.x * scale.x - 1.0 ||
					edgeDist.y <= 1.0 || edgeDist.y >= uDisplaySize.y * scale.y - 1.0) {
					oColor = uBorderColor;
				} else {
					discard;
				}
			}
		))
	);

	mBatch = gl::Batch::create(geom::Rect(Rectf(0, 0, 1, 1)), mGlsl);
}


}
}