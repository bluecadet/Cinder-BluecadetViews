#include "TextView.h"

#include "bluecadet/text/StyleManager.h"

#if defined(CINDER_MSW)

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

TextView::TextView() : BaseView(), bluecadet::text::StyledTextLayout(),
mTextureFormat(getDefaultTextureFormat()),
mTexture(nullptr),
mAutoRenderEnabled(true),
mPremultiplied(false)
{
}

TextView::~TextView() {
}

TextViewRef TextView::create(const std::string& text, const std::string& styleKey, const bool parseText, const float maxWidth) {
	auto textView = make_shared<TextView>();
	textView->setup(text, styleKey, parseText, maxWidth);
	return textView;
}

TextViewRef TextView::create(const std::wstring& text, const std::string& styleKey, const bool parseText, const float maxWidth) {
	auto textView = make_shared<TextView>();
	textView->setup(text, styleKey, parseText, maxWidth);
	return textView;
}

const ci::gl::Texture::Format & TextView::getDefaultTextureFormat() {
	static gl::Texture::Format format;
	static bool initialized = false;

	if (!initialized) {
		format.immutableStorage(true);
		format.setMaxAnisotropy(4.0f);
		format.enableMipmapping(false);
		format.setMaxMipmapLevel(0);
		format.setMinFilter(GL_LINEAR);
		format.setMagFilter(GL_LINEAR);
		initialized = true;
	}

	return format;
}

const ci::gl::Texture::Format & TextView::getMipMapTextureFormat() {
	static gl::Texture::Format format;
	static bool initialized = false;

	if (!initialized) {
		format.immutableStorage(true);
		format.setMaxAnisotropy(4.0f);
		format.enableMipmapping(true);
		format.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		format.setMagFilter(GL_LINEAR);
		initialized = true;
	}

	return format;
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
		mTexture = gl::Texture2d::create(mSurface, mTextureFormat);
		mSurface = ci::Surface(); // reset surface to save memory
	}

	mHasInvalidRenderedContent = false;
}

void TextView::invalidate(const bool layout, const bool size) {
	StyledTextLayout::invalidate(layout, size);
	mHasInvalidRenderedContent = true;

	BaseView::invalidate(false, true);
}

void TextView::resetRenderedContent() {
	mTexture = nullptr;
	mSurface = ci::Surface();
}

void TextView::setSize(const ci::vec2& size) {
	invalidate();
	setMaxSize(size);
}

inline void TextView::setWidth(const float width){
	invalidate();
	setMaxWidth(width);
}

inline void TextView::setHeight(const float height){
	invalidate();
	setMaxHeight(height);
}

const ci::vec2 TextView::getSize() {
	const vec2 maxSize = StyledTextLayout::getMaxSize();
	const vec2 textSize = StyledTextLayout::getTextSize();
	return vec2(
		max(maxSize.x, textSize.x),
		max(maxSize.y, textSize.y)
	);
}

}
}

#endif
