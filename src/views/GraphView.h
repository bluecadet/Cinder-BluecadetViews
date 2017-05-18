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

	enum class Style {
		Line,
		Gradient
	};

	GraphView(const ci::ivec2 & size, const Style style = Style::Line);
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

	//! Size can't be changed using setSize().
	void setSize(const ci::vec2 & size) override { ci::app::console() << "GraphView: Warning: Size can't be changed using setSize()." << std::endl; }

	//! Draws labels with the current values of each graph if enabled. Defaults to true.
	bool getLabelsEnabled() const { return mLabelsEnabled; }
	void setLabelsEnabled(const bool value) { mLabelsEnabled = value; }

	Style getStyle() const { return mStyle; }
	void setStyle(const Style value) { mStyle = value; }

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
	size_t mCapacity;
	bool mNeedsUpdate;
	bool mLabelsEnabled;
	Style mStyle;

	ci::gl::FboRef mFbo;
	ci::gl::GlslProgRef mGlsl;
	ci::gl::BatchRef mBatch;
};

}
}
