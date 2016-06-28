#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class GraphView> GraphViewRef;

class GraphView : public BaseView {

public:

	GraphView(const ci::ivec2& size, const int pxPerValue = 1);
	~GraphView();

	void addGraph(const std::string& id, const float min, const float max, const ci::ColorA color = ci::ColorA(1.0f, 0, 0, 1.0f));
	void addValue(const std::string& id, const float value);

	const ci::ivec2& getSize() const { return mFbo->getSize() * mPxPerValue; }

protected:

	struct Graph {
		ci::ColorA color;
		float min;
		float max;
		std::vector<float> values;
		size_t index;
	};

	void drawContent() override;
	void render();

	ci::gl::FboRef mFbo;
	std::map<std::string, Graph> mGraphs;
	bool mNeedsUpdate;
	int mPxPerValue;
};

}
}