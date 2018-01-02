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
mTopDown(false),
mHasCustomTexCoords(false),
mTexCoordsAreDirty(true)
{
	mDefaultTexCoords = { vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f) };
	mTexCoords = mDefaultTexCoords;
}

ImageView::~ImageView() {
}

void ImageView::reset() {
    BaseView::reset();
    setTexture(nullptr);
	setScaleMode(sDefaultScaleMode);
}

void ImageView::setTexCoords(std::vector<ci::vec2> coords ) {
	mUserTexCoords = coords;
	mHasCustomTexCoords = true;
	mTexCoordsAreDirty = true;
	invalidate(false, true);
}

void ImageView::setTexture(ci::gl::TextureRef texture, std::vector<ci::vec2> coords, const bool resizeToTexture) {
	setTexCoords(coords);
	setTexture(texture, resizeToTexture);
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

	//use the appropriate tex coords 
	if (!mHasCustomTexCoords) {
		mTexCoords = mDefaultTexCoords;	
	} else {
		mTexCoords = mUserTexCoords;
	}

	//flip the tex coords if we're not using top down
	if (!mTopDown) {
		swap(mTexCoords[0], mTexCoords[3]);
		swap(mTexCoords[1], mTexCoords[2]);
	}

	//setup static shader once for all instances
	static gl::GlslProgRef shader = nullptr;

	if (!shader) {
		shader = gl::GlslProg::create(gl::GlslProg::Format().vertex(CI_GLSL(150,

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
			in vec2 vTexCoord0;
			in vec4 vColor;
			out vec4 oColor;

			void main(void) {
				vec2 texCoord = vTexCoord0;
				texCoord = (texCoord - vec2(0.5)) * uTexScale + vec2(0.5); // scale around center

				if (texCoord.x < 0 || texCoord.y < 0 || texCoord.x > 1.0 || texCoord.y > 1.0) {
					discard;
				}

				oColor = texture(uTex0, texCoord);
				oColor.rgb /= oColor.a;
				oColor *= vColor;
			}

		)));
	}

	// make the batch only if: it hasn't been made and the shader is ready,
	//or if the tex coords need updating
	if ( (!mBatch && shader) || mTexCoordsAreDirty ) {

		auto rect = geom::Rect().rect(Rectf(0, 0, 1.0f, 1.0f))
								.texCoords( mTexCoords[0],
											mTexCoords[1],
											mTexCoords[2],
											mTexCoords[3] );
		mTexCoordsAreDirty = false;

		mBatch = gl::Batch::create(rect, shader);

	}

}

void ImageView::draw() {

	if (!mTexture || !mBatch) return;
	
	BaseView::draw();

	mTexture->bind(0);
	mBatch->getGlslProg()->uniform("uTexScale", mTextureScale);
	mBatch->getGlslProg()->uniform("uSize", mTextureSize);
	mBatch->draw();
}

}
}
