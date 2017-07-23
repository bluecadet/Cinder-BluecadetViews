#include "FboView.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

FboView::FboView() : BaseView(),
mFbo(nullptr),
mForceRedraw(false),
mDrawsToScreen(true),
mResolution(1.0f)
{
	gl::Texture2d::Format fboTexFormat;
	fboTexFormat.enableMipmapping(true);
	fboTexFormat.minFilter(GL_LINEAR_MIPMAP_LINEAR);
	fboTexFormat.magFilter(GL_LINEAR_MIPMAP_LINEAR);

	mFboFormat.setColorTextureFormat(fboTexFormat);
}

FboView::~FboView() {
}

void FboView::setup(const ci::ivec2 & size, const float resolution) {
	setFboScale(resolution);
	setSize(size);
}

void FboView::setSize(const ci::vec2 & size){
	BaseView::setSize(size);
	invalidateFbo();
}

void FboView::setFboScale(const float value) {
	mResolution = value;
	invalidateFbo();
}

ci::gl::FboRef FboView::createFbo(const ci::ivec2 & size, const ci::gl::Fbo::Format & format){
	return gl::Fbo::create(size.x, size.y, format);
}

void FboView::validateFbo(){
	const ivec2 size = glm::ceil(mResolution * getSize());
	if (size.x > 0 && size.y > 0) {
		mFbo = createFbo(size, getFboFormat());
		// Invalidate content to confirm it will redraw to fbo
		invalidate(ValidationFlags::CONTENT);
	}
}

void FboView::validateContent(){
	if (!mFbo) {
		validateFbo();
	}

	if (!hasInvalidContent()) {
		return;
	}

	if (mFbo) {
		gl::ScopedMatrices scopedMatrices;

		gl::setMatricesWindow(mFbo->getSize());
		gl::ScopedViewport scopedViewport(mFbo->getSize());
		gl::ScopedFramebuffer scopedFbo(mFbo);
		
		gl::clear(ColorA(0, 0, 0, 0));
		gl::scale(vec3(mResolution, mResolution, 1.0f));

		if (getBlendMode() == BlendMode::PREMULT) {
			gl::ScopedBlendPremult scopedBlend;
			BaseView::drawChildren(getDrawColor());

		} else {
			BaseView::drawChildren(getDrawColor());
		}

		// Set mHasInvalidContent back to false so it doesn't continue to validate on every draw
		BaseView::validateContent();
	}
	else {
		CI_LOG_W("No fbo to validate content in (size: " << getSize().value() << ")");
	}

}

void FboView::handleEvent(ViewEvent& event) {
	if (event.type == ViewEvent::Type::CONTENT_INVALIDATED) {
		invalidate(ValidationFlags::CONTENT);
	}
}

void FboView::draw() {
	
	if (!mFbo) {
		CI_LOG_W("No fbo to validate content in (size: " << getSize().value() << ")");
		return;
	}

	if (hasInvalidContent() || mForceRedraw){
		validateContent();
	}

	BaseView::draw();

	if (mFbo && mDrawsToScreen) {
		const float inverseScale = 1.0f / mResolution;
		gl::scale(vec3(inverseScale, inverseScale, 1.0));
		gl::draw(mFbo->getColorTexture());
	}
}

}
}
