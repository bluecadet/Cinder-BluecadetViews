#include "TextView.h"

#include "bluecadet/text/StyleManager.h"
#include "ImageView.h"
#include "cinder/Log.h"

#if defined(CINDER_MSW)

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::text;

namespace bluecadet {
namespace views {

TextView::TextView(const ci::gl::Texture::Format & textureFormat) : BaseView(), text::StyledTextLayout(),
mTextureFormat(textureFormat) {
}

TextView::~TextView() {
}

ci::gl::Texture::Format TextView::getDefaultTextureFormat() {
	gl::Texture::Format format;
	format.immutableStorage(true);
	format.setMaxAnisotropy(4.0f);
	format.enableMipmapping(true);
	format.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	format.setMagFilter(GL_LINEAR);
	return format;
}

void TextView::setup(const std::wstring & text, const std::string & styleKey, const bool parseText, const float maxWidth, const TokenParserMapRef customTokenParsers) {
	setMaxWidth(maxWidth);

	if (text.empty()) {
		auto style = text::StyleManager::getInstance()->getStyle(styleKey);
		setCurrentStyle(style);
	} else if (parseText) {
		setText(text, styleKey, customTokenParsers);

	} else {
		if (customTokenParsers != nullptr) {
			CI_LOG_W("Custom token parsers won't be used on plain text.");
		}

		setPlainText(text, styleKey);
	}
}

void TextView::setup(const std::string & text, const std::string & styleKey, const bool parseText, const float maxWidth, const TokenParserMapRef customTokenParsers) {
	setup(text::wideString(text), styleKey, parseText, maxWidth, customTokenParsers);
}

void TextView::reset() {
	BaseView::reset();
	clearText();
	resetRenderedContent();
}

void TextView::willDraw() {
	if (mAutoRenderEnabled) {
		renderContent(false, true, getBlendMode() == BlendMode::PREMULT, false);
	}
}

void TextView::draw() {
	BaseView::draw();

	if (mTexture) {
		//gl::draw(mTexture);
		//return;

		static gl::GlslProgRef shader = nullptr;
		static gl::BatchRef batch = nullptr;

		if (!shader) {
			shader = gl::GlslProg::create(gl::GlslProg::Format()
				.vertex(CI_GLSL(150,
					uniform mat4 ciModelViewProjection;
			uniform vec2 uSize;
			in vec4 ciPosition;
			in vec4 ciColor;
			in vec2 ciTexCoord0;
			out vec4 vColor;
			out vec2 vTexCoord0;

			void main(void) {
				vColor = ciColor;
				vTexCoord0 = ciTexCoord0;
				vec4 pos = ciPosition * vec4(uSize, 0, 1);
				gl_Position = ciModelViewProjection * pos;
			}
			)).fragment(CI_GLSL(150,
				uniform sampler2D uTex0;
			uniform int uDemultiply;
			in vec2 vTexCoord0;
			in vec4 vColor;
			out vec4 oColor;

			void main(void) {
				oColor = texture(uTex0, vTexCoord0);

				if (oColor.a == 0) {
					discard;
				}

				if (uDemultiply == 1) {
					// This compensates for GDI+ interpolation between text
					// and transparent black background
					oColor.rgb /= oColor.a;
				}

				oColor *= vColor;
			}
			)));
			batch = gl::Batch::create(geom::Rect().rect(Rectf(0, 0, 1.0f, 1.0f)), shader);
		}

		gl::ScopedTextureBind textureBind(mTexture, 0);
		shader->uniform("uSize", getSize());
		shader->uniform("uDemultiply", mDemultiplyEnabled ? 1 : 0);
		batch->draw();
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
		mSurface = renderToSurface(alpha, premultiplied, getBackgroundColor().value());
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

void TextView::setSize(const ci::vec2 & size) {
	invalidate();
	setMaxSize(size);
}

inline void TextView::setWidth(const float width) {
	invalidate();
	setMaxWidth(width);
}

inline void TextView::setHeight(const float height) {
	invalidate();
	setMaxHeight(height);
}

void TextView::setBlendMode(const BlendMode blendMode) {
	if (blendMode != getBlendMode()) {
		invalidate(false, true);
	}
	BaseView::setBlendMode(blendMode);
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
