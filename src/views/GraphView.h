#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class GraphView> GraphViewRef;

class GraphView : public views::BaseView {

public:

	GraphView(const ci::ivec2 & size, const int pxPerValue = 1);
	~GraphView();

	//! Creates a graph with green min and red max colors
	void addGraph(const std::string & id, const float min, const float max);

	//! Creates a graph with the same min and max colors
	void addGraph(const std::string & id, const float min, const float max, const ci::ColorA color);

	//! Creates a graph with custom min and max colors
	void addGraph(const std::string & id, const float min, const float max, const ci::ColorA minColor, const ci::ColorA maxColor);
	
	//! Adds a value to the end of the graph and scrolls the previous values to the left.
	void addValue(const std::string & id, const float value);

	//! Replaces all current values and resets the scroll position.
	void setValues(const std::string & id, const std::vector<float> & values);

protected:

	struct Graph {
		ci::ColorA minColor;
		ci::ColorA maxColor;
		float min;
		float max;
		std::vector<float> values;
		size_t index;
	};

	void draw() override;
	void render();
	void setupShaders(const int numValues);

	std::map<std::string, Graph> mGraphs;
	bool mNeedsUpdate;

	ci::gl::FboRef mFbo;
	size_t mCapacity;
	int mPxPerValue;

	ci::gl::GlslProgRef mGlsl;
	ci::gl::BatchRef mBatch;
};

}
}
