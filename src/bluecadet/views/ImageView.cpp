#include "ImageView.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

ImageView::ScaleMode ImageView::sDefaultScaleMode = ImageView::ScaleMode::STRETCH;

ImageView::ImageView() : BaseView(),
mTexture(nullptr),
mTextureScale(1.0f),
mScaleMode(sDefaultScaleMode)
{
}

ImageView::~ImageView() {
}

void ImageView::reset() {
    BaseView::reset();
    setTexture(nullptr);
	setScaleMode(sDefaultScaleMode);
}

void ImageView::setTexture(ci::gl::TextureRef texture, const bool resizeToTexture) {
	mTexture = texture;

	if (resizeToTexture) {
		if (mTexture) {
			// apply texture size
			setSize(vec2(mTexture->getSize()));
		} else {
			// reset to 0
			setSize(vec2());
		}
	}

	invalidate(false, true);
}

void ImageView::validateContent() {
	BaseView::validateContent();

	mTextureScale = vec2(1.0f);

	switch (mScaleMode) {
		case ScaleMode::NONE:
			mTextureScale = vec2(1.0f);
			mTextureSize = mTexture->getSize();
			break;
		case ScaleMode::STRETCH:
			mTextureScale = vec2(1.0f);
			mTextureSize = getSize();
			break;
		case ScaleMode::FIT:
		case ScaleMode::COVER:
		{
			mTextureSize = getSize();
			mTextureScale = mTextureSize / vec2(mTexture->getSize());
			if (mScaleMode == ScaleMode::FIT) {
				mTextureScale /= glm::min(mTextureScale.x, mTextureScale.y);
			} else {
				mTextureScale /= glm::max(mTextureScale.x, mTextureScale.y);
			}
			break;
		}
	}
}

void ImageView::draw() {
	BaseView::draw();

	if (mTexture) {
		drawTexture(mTexture, getDrawBlendMode(), mTextureScale, mTextureSize);
	}
}

void ImageView::drawTexture(ci::gl::TextureRef texture, BlendMode blendMode, ci::vec2 scale, ci::vec2 size) {
	int options = DrawOptions::NONE;

	if (blendMode == BlendMode::PREMULT) {
		options |= DrawOptions::PREMULTIPLY;
	}

	drawTexture(texture, (DrawOptions)options, scale, size);
}

void ImageView::drawTexture(ci::gl::TextureRef texture, DrawOptions options, ci::vec2 scale, ci::vec2 size) {
	static gl::GlslProgRef shader = nullptr;
	static gl::BatchRef batch = nullptr;

	if (!shader) {
		shader = gl::GlslProg::create(
			gl::GlslProg::Format()
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
					uniform vec2 uTexScale;
					uniform int uOptions;
					in vec2 vTexCoord0;
					in vec4 vColor;
					out vec4 oColor;

					void main(void) {
						vec2 texCoord = vTexCoord0;
						texCoord = (texCoord - vec2(0.5)) * uTexScale + vec2(0.5); // scale around center

						if (texCoord.x < 0 || texCoord.y < 0 || texCoord.x > 1.0 || texCoord.y > 1.0) {
							discard;
						}

						oColor = texture(uTex0, texCoord) * vColor;
						
						if ((uOptions & OPT_PREMULT) == OPT_PREMULT) {
							oColor.rgb *= oColor.a;
						}

						if ((uOptions & OPT_DEMULT) == OPT_DEMULT) {
							oColor.rgb /= oColor.a;
						}
					}
				)
			).define("OPT_PREMULT", to_string(DrawOptions::PREMULTIPLY))
			.define("OPT_DEMULT", to_string(DrawOptions::DEMULTIPLY))
		);


		batch = gl::Batch::create(geom::Rect().rect(Rectf(0, 0, 1.0f, 1.0f)), shader);
	}

	if (size.x < 0 || size.y < 0) {
		size = texture->getSize();
	}

	gl::ScopedTextureBind textureBind(texture, 0);
	shader->uniform("uTexScale", scale);
	shader->uniform("uSize", size);
	shader->uniform("uOptions", options);
	batch->draw();
}

}
}
