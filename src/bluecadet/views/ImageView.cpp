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
mScaleMode(sDefaultScaleMode),
mTopDown(false)
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
	if (!mTexture) return;
	
	BaseView::draw();

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
			uniform vec2 uTexScale;
			uniform int uTopDown;
			in vec2 vTexCoord0;
			in vec4 vColor;
			out vec4 oColor;

			void main(void) {
				vec2 texCoord = vTexCoord0;
				texCoord.y = uTopDown != 0 ? 1.0 - texCoord.y : texCoord.y; // flip y if necessary
				texCoord = (texCoord - vec2(0.5)) * uTexScale + vec2(0.5); // scale around center

				if (texCoord.x < 0 || texCoord.y < 0 || texCoord.x > 1.0 || texCoord.y > 1.0) {
					discard;
				}

				oColor = texture(uTex0, texCoord);
				oColor.rgb /= oColor.a;
			}
		)));

		batch = gl::Batch::create(geom::Rect().rect(Rectf(0, 0, 1.0f, 1.0f)), shader);
	}

	mTexture->bind(0);
	shader->uniform("uTexScale", mTextureScale);
	shader->uniform("uSize", mTextureSize);
	shader->uniform("uTopDown", mTopDown ? 1 : 0);
	batch->draw();
}

}
}
