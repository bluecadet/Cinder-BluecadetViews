#include "TextView.h"

#include "bluecadet/text/StyleManager.h"
#include "cinder/Log.h"

#if defined(CINDER_MSW)

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

TextView::TextView() : BaseView(), text::StyledTextLayout(),
mTexture(nullptr),
mAutoRenderEnabled(true),
mSmoothScalingEnabled(true),
mPremultiplied(false)
{
}

TextView::~TextView() {
}

TextViewRef TextView::create(const std::string& text, const std::string& styleKey, const bool parseText, const float maxWidth) {
	TextViewRef textView(new TextView());
	textView->setup(text, styleKey, parseText, maxWidth);
	return textView;
}

TextViewRef TextView::create(const std::wstring& text, const std::string& styleKey, const bool parseText, const float maxWidth) {
	TextViewRef textView(new TextView());
	textView->setup(text, styleKey, parseText, maxWidth);
	return textView;
}

void TextView::setup(const std::wstring& text, const std::string& styleKey, const bool parseText, const float maxWidth) {
	setMaxWidth(maxWidth);

	if (text.empty()) {
		auto style = text::StyleManager::getInstance()->getStyle(styleKey);
		setCurrentStyle(style);
	} else if (parseText) {
		setText(text, styleKey);

	} else {
		setPlainText(text, styleKey);
	}
}

void TextView::setup(const std::string& text, const std::string& styleKey, const bool parseText, const float maxWidth) {
	setup(text::wideString(text), styleKey, parseText, maxWidth);
}

void TextView::reset() {
	BaseView::reset();
	clearText();
	resetRenderedContent();
}

void TextView::willDraw() {
	if (needsToBeRendered(false) && mAutoRenderEnabled) {
		renderContent(false, true, mPremultiplied);
	}
}

void TextView::setSize(const ci::vec2& size) {
	BaseView::setSize(size);
	StyledTextLayout::setMaxSize(size);
}

void TextView::setWidth(float value) {
	BaseView::setSize(vec2(value, getHeight()));
	StyledTextLayout::setMaxSize(vec2(value, getMaxHeight()));
}

void TextView::setHeight(float value) {
	BaseView::setSize(vec2(getWidth(), value));
	StyledTextLayout::setMaxSize(vec2(getMaxWidth(), value));
}

ci::Anim<ci::vec2>& TextView::getSize() {
	validateContent();
	return BaseView::getSize();
}

void TextView::validateContent() {
	if (!hasInvalidContent()) {
		return;
	}

	// Check if max size should be changed to view size
	vec2 viewSize = BaseView::getSize().value();
	vec2 maxSize = StyledTextLayout::getMaxSize();

	if (viewSize.x > 0 && maxSize.x > 0) {
		maxSize.x = viewSize.x;
	}
	if (viewSize.y > 0 && maxSize.y > 0) {
		maxSize.x = viewSize.x;
	}

	StyledTextLayout::setMaxSize(maxSize);
	StyledTextLayout::validateSize();

	// Check if view size should be changed to text size
	const vec2 & textSize = StyledTextLayout::getTextSize();

	viewSize.x = glm::max(maxSize.x, textSize.x);
	viewSize.y = glm::max(maxSize.y, textSize.y);

	BaseView::setSize(viewSize);
	BaseView::validateContent();
}

void TextView::draw() {
	BaseView::draw();
	if (mTexture) {
		gl::draw(mTexture);
	}
}

bool TextView::needsToBeRendered(bool surfaceOnly) const {
	if (mHasInvalidRenderedContent) return true;
	if (hasChanges()) return true; // content has changes, so we def need to re-render
	if (!surfaceOnly && !mTexture) return true; // we don't have a texture but need one
	return false;
}

void TextView::renderContent(bool surfaceOnly, bool alpha, bool premultiplied, bool force) {
	if (!needsToBeRendered(surfaceOnly) && !force) {
		return;
	}

	if (mHasInvalidRenderedContent || hasChanges() || (mSurface.getSize() != getTextSize()) || (mTexture && mSurface.getSize() != mTexture->getSize())) {
		mSurface = renderToSurface(alpha, premultiplied);
	}

	if (surfaceOnly) {
		mTexture = nullptr; // reset texture to save memory

	} else {
		if (mTexture && mTexture->getSize() == mSurface.getSize()) {
			mTexture->update(mSurface);
		} else {
			mTexture = gl::Texture2d::create(mSurface, createTextureFormat(mSmoothScalingEnabled));
		}

		mSurface = ci::Surface(); // reset surface to save memory
	}

	mHasInvalidRenderedContent = false;
}

void TextView::invalidate(const bool layout, const bool size) {
	BaseView::invalidate(ValidationFlags::CONTENT);
	StyledTextLayout::invalidate(layout, size);
	mHasInvalidRenderedContent = true;
}

void TextView::invalidate(const int flags) {
	BaseView::invalidate(flags);

	const bool contentChanged = (flags & ValidationFlags::CONTENT) == ValidationFlags::CONTENT;
	StyledTextLayout::invalidate(contentChanged, contentChanged);
	
	mHasInvalidRenderedContent = mHasInvalidRenderedContent || contentChanged;
}

void TextView::resetRenderedContent() {
	mTexture = nullptr;
	mSurface = ci::Surface();
}

ci::gl::Texture::Format TextView::createTextureFormat(bool smoothScaling) const {
	gl::Texture::Format format;
	format.immutableStorage(true);

	if (smoothScaling) {
		format.enableMipmapping(true);
		format.setMaxMipmapLevel(2);
		format.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		format.setMagFilter(GL_LINEAR);
	} else {
		format.setMaxAnisotropy(4.0f);
		format.enableMipmapping(false);
		format.setMaxMipmapLevel(0);
		format.setMinFilter(GL_LINEAR);
		format.setMagFilter(GL_LINEAR);
	}

	return format;
}

}
}

#endif
