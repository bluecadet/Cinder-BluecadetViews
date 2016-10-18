#include "FboView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

FboView::FboView() : BaseView(),
mFbo(nullptr),
mForceRedraw(false)
{
	gl::Texture2d::Format fboTexFormat;
	fboTexFormat.enableMipmapping(true);
	fboTexFormat.minFilter(GL_LINEAR_MIPMAP_LINEAR);
	fboTexFormat.magFilter(GL_LINEAR_MIPMAP_LINEAR);

	mFboFormat.setColorTextureFormat(fboTexFormat);
}

FboView::~FboView() {
}

void FboView::setup(const ci::ivec2 & size) {
	setSize(size);
}

void FboView::setSize(const ci::vec2 & size){
	BaseView::setSize(size);
	invalidateFbo();
}

ci::gl::FboRef FboView::createFbo(const ci::ivec2 & size, const ci::gl::Fbo::Format & format){
	return gl::Fbo::create(size.x, size.y, format);
}

void FboView::validateFbo(){
	if (getSize().x > 0 && getSize().y > 0) {
		mFbo = createFbo(getSize(), getFboFormat());
		// Invalidate content to confirm it will redraw to fbo
		invalidate(false, true);
	}
}

inline void FboView::validateContent(){
	if (!mFbo) {
		validateFbo();
	}

	if (!hasInvalidContent()) {
		return;
	}

	if (mFbo) {
		auto vp = gl::getViewport();

		gl::ScopedMatrices scopedMatrices;
		gl::ScopedViewport scopedViewport(ivec2(0), mFbo->getSize());

		gl::setMatricesWindow(mFbo->getSize());

		mFbo->bindFramebuffer();

		gl::viewport(mFbo->getSize());
		gl::clear(ColorA(0, 0, 0, 0));

		BaseView::drawChildren(getDrawColor());

		mFbo->unbindFramebuffer();

		// Set mHasInvalidContent back to false so it doesn't continue to validate on every draw
		BaseView::validateContent();
	}
	else {
		console() << "FboView Warning: No fbo to validate content in (size: " << getSize() << ")" << endl;
	}

}

void FboView::handleEvent(const ViewEvent& event) {
	if (event.type == ViewEvent::Type::UPDATED) {
		invalidate(false, true);
	}
}

void FboView::draw() {
	
	if (!mFbo) {
		console() << "FboView Warning: No fbo to draw to (size: " << getSize() << ")" << endl;
		return;
	}

	if (hasInvalidContent() || mForceRedraw){
		validateContent();
	}

	if (mFbo) {
		gl::draw(mFbo->getColorTexture());
	}
}

}
}
