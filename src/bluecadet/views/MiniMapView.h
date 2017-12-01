#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class MiniMapView> MiniMapViewRef;

class MiniMapView : public BaseView {

public:
	typedef std::function<std::string()> StatsFn;

	MiniMapView(const float mapScale = 0.05f);
	virtual ~MiniMapView();

	void setLayout(const int cols, const int rows, const ci::ivec2 & displaySize, const ci::ivec2 bezel);
	void setViewport(const ci::Area & viewport);

	//! Defaults to white
	ci::ColorA getBorderColor() const { return mBorderColor; }
	void setBorderColor(const ci::ColorA value) { mBorderColor = value; }

protected:
	void draw() override;
	virtual void updateContent();
	virtual void setupShaders();

	bool mRequiresContentUpdate;

	float mMapScale;
	ci::ColorA mBorderColor;

	int mCols;
	int mRows;
	ci::ivec2 mDisplaySize;
	ci::ivec2 mBezelDims;

	ci::ivec2 mAppSize;
	ci::ivec2 mScaledSize;

	ci::gl::FboRef mFbo;
	ci::gl::GlslProgRef mGlsl;
	ci::gl::BatchRef mBatch;

	BaseViewRef mViewportView;
};

}
}