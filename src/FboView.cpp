#include "FboView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

FboView::FboView() : BaseView(),
mHasInvalidContent(true),
mForceRedraw(false),
mFbo(nullptr)
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

void FboView::setSize(const ci::vec2 & size)
{
	BaseView::setSize(size);
	invalidateFbo();
}

ci::gl::FboRef FboView::createFbo(const ci::ivec2 & size, const ci::gl::Fbo::Format & format)
{
	return gl::Fbo::create(size.x, size.y, format);
}

void FboView::validateFbo()
{
	if (getSize().x > 0 && getSize().y > 0) {
		mFbo = createFbo(getSize(), getFboFormat());
	}
}

void FboView::validateContent()
{
	auto vp = gl::getViewport();

	gl::ScopedMatrices scopedMatrices;
	gl::ScopedViewport scopedViewport(ivec2(0), mFbo->getSize());
	
	gl::setMatricesWindow(mFbo->getSize());

	mFbo->bindFramebuffer();

	gl::viewport(mFbo->getSize());
	gl::clear(ColorA(0, 0, 0, 0));

	BaseView::drawChildren(getDrawColor());

	mFbo->unbindFramebuffer();

	mHasInvalidContent = false;
}

void FboView::draw() {
	if (!mFbo) {
		validateFbo();
	}

	if (!mFbo) {
		console() << "FboView Warning: No fbo to draw to (size: " << getSize() << ")" << endl;
		return;
	}

	// TODO : REVIEW
	bool childHasInvalidTransform = false;
	for (auto& child : BaseView::getChildren()) {
		if (child->hasInvalidTransforms()) {
			childHasInvalidTransform = true;
			break;
		}
	}

	if (BaseView::hasInvalidTransforms() || childHasInvalidTransform) {
		validateContent();
	}
	else {
		console() << "it doesn't need to be updated!" << endl;
	}

	//	if (mHasInvalidContent || mForceRedraw) {
	//		validateContent();
	//	}

	if (mFbo) {
		gl::draw(mFbo->getColorTexture());
	}
}

void FboView::drawChildren(const ci::ColorA & parentTint)
{
	// Don't do anything; Children are only drawn in validateFbo().
}


}
}
