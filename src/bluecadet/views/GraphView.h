#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/TextureFont.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class GraphView> GraphViewRef;

class GraphView : public views::BaseView {

public:
	enum class Style { Line, Gradient };
	enum class Layout { LabelsLeft, LabelsRight };

	GraphView(const ci::ivec2 & size = ci::ivec2(128, 48), const Style style = Style::Line,
			  const Layout layout = Layout::LabelsRight);
	~GraphView();

	//! Creates a graph with green min and red max colors
	void addGraph(const std::string & id, const float min, const float max, const bool hidden = false);

	//! Creates a graph with the same min and max colors
	void addGraph(const std::string & id, const float min, const float max, const ci::ColorA color,
				  const bool hidden = false);

	//! Creates a graph with custom min and max colors
	void addGraph(const std::string & id, const float min, const float max, const ci::ColorA minColor,
				  const ci::ColorA maxColor, const bool hidden = false);

	//! Removes a graph if it exists
	void removeGraph(const std::string & id);

	//! Adds a value to the end of the graph and scrolls the previous values to the left.
	void addValue(const std::string & id, const float value);

	//! Replaces all current values and resets the scroll position.
	void setValues(const std::string & id, const std::vector<float> & values);

	//! This re-creates the FBO and shaders, so this is an expensive method.
	void setSize(const ci::vec2 & size) override;

	//! Draws labels with the current values of each graph if enabled. Defaults to true.
	bool getLabelsEnabled() const { return mLabelsEnabled; }
	void setLabelsEnabled(const bool value) { mLabelsEnabled = value; }

	void showGraph(const std::string & id, bool showing = true);
	bool isShowing(const std::string & id) const;

	Style getStyle() const { return mStyle; }
	void setStyle(const Style value) { mStyle = value; }

	inline void setLabelOffset(ci::vec2 value) { mLabelOffset = value; }
	inline ci::vec2 getLabelOffset() const { return mLabelOffset; }

	inline void setLabelFont(ci::Font value) { mLabelFont = value; }
	inline ci::Font getLabelFont() const { return mLabelFont; }

	inline void setLayout(Layout value) { mLayout = value; }
	inline Layout getLayout() const { return mLayout; }

protected:
	struct Graph {
		ci::ColorA minColor;
		ci::ColorA maxColor;
		float min   = 0.0f;
		float max   = 1.0f;
		bool hidden = false;
		std::vector<float> values;
		size_t index = 0;
	};

	void draw() override;
	void render();
	void setupShaders(const int numValues);

	std::vector<std::string> mGraphIdsInOrder;
	std::map<std::string, Graph> mGraphs;
	size_t mCapacity;
	bool mNeedsUpdate;
	bool mLabelsEnabled;
	ci::vec2 mLabelOffset;
	Style mStyle;
	Layout mLayout;

	ci::gl::FboRef mFbo;
	ci::gl::GlslProgRef mGlsl;
	ci::gl::BatchRef mBatch;

	ci::Font mLabelFont;
	ci::gl::TextureFontRef mLabelTextureFont;
};

}  // namespace views
}  // namespace bluecadet
