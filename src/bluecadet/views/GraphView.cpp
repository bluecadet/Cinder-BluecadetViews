#include "GraphView.h"

#include "cinder/Log.h"

#include "cinder/gl/TextureFont.h"

#include <algorithm>

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

GraphView::GraphView(const ci::ivec2 & size, const Style style, const Layout layout)
	: BaseView(),
	  mNeedsUpdate(false),
	  mLabelsEnabled(true),
	  mCapacity(size.x),
	  mStyle(style),
	  mLayout(layout),
	  mLabelFont("Consolas", 16) {
	mLabelTextureFont = gl::TextureFont::create(mLabelFont);

	setBackgroundColor(ColorA(0, 0, 0, 0.25f));
	GraphView::setSize(size);
}

GraphView::~GraphView() {
}

void GraphView::addGraph(const std::string & id, const float min, const float max, const bool hidden) {
	addGraph(id, min, max, ci::ColorA(0.0f, 1.0f, 0, 0.5f), ci::ColorA(1.0f, 0.0f, 0, 0.5f), hidden);
}

void GraphView::addGraph(const std::string & id, const float min, const float max, const ci::ColorA color,
						 const bool hidden) {
	addGraph(id, min, max, color, color, hidden);
}

void GraphView::addGraph(const std::string & id, const float min, const float max, const ci::ColorA minColor,
						 const ci::ColorA maxColor, const bool hidden) {
	if (mGraphs.find(id) != mGraphs.end()) {
		CI_LOG_W("Graph already exists for id '" << id << "'");
		return;
	}
	Graph graph;
	graph.min	  = min;
	graph.max	  = max;
	graph.minColor = minColor;
	graph.maxColor = maxColor;
	graph.hidden   = hidden;
	graph.values   = vector<float>(mCapacity, 0.0f);
	graph.index	= 0;
	mGraphs[id]	= graph;
	mGraphIdsInOrder.push_back(id);
	mNeedsUpdate = true;
}

void GraphView::removeGraph(const std::string & id) {
	for (auto it = mGraphIdsInOrder.begin(); it != mGraphIdsInOrder.end();) {
		if (*it == id) {
			mGraphs.erase(*it);
			it = mGraphIdsInOrder.erase(it);
		} else {
			it++;
		}
	}
}

void GraphView::addValue(const std::string & id, const float value) {
	auto it = mGraphs.find(id);
	if (it == mGraphs.end()) {
		CI_LOG_W("No graph found with id '" << id << "'");
		return;
	}
	if (mCapacity <= 0) {
		return;
	}
	Graph & graph		  = it->second;
	auto currIdx		  = mCapacity - graph.index - 1;
	auto nextIdx		  = (graph.index + 1) % mCapacity;
	graph.values[currIdx] = value;
	graph.index			  = nextIdx;
	mNeedsUpdate		  = true;
}

void GraphView::setValues(const std::string & id, const std::vector<float> & values) {
	if (values.size() != mCapacity) {
		CI_LOG_W("Values must be an array of size '" << mCapacity << "' and not '" << to_string(values.size()) << "'");
		return;
	}
	auto it = mGraphs.find(id);
	if (it == mGraphs.end()) {
		CI_LOG_W("No graph found with id '" << id << "'");
		return;
	}
	it->second.values = values;
	mNeedsUpdate	  = true;
}

void GraphView::setSize(const ci::vec2 & size) {
	BaseView::setSize(size);

	gl::Texture::Format texFormat;
	gl::Fbo::Format fboFormat;

	texFormat.internalFormat(GL_RGBA).mipmap(false).minFilter(GL_LINEAR).magFilter(GL_NEAREST);
	fboFormat.colorTexture(texFormat).disableDepth();

	mFbo = ci::gl::Fbo::create(size.x, size.y, fboFormat);
	setupShaders((int)mCapacity);

	mNeedsUpdate = true;
}

void GraphView::showGraph(const std::string & id, bool showing) {
	auto it = mGraphs.find(id);
	if (it == mGraphs.end()) {
		CI_LOG_W("No graph found with id '" << id << "'");
		return;
	}
	it->second.hidden = !showing;
	mNeedsUpdate	  = true;
}

bool GraphView::isShowing(const std::string & id) const {
	auto it = mGraphs.find(id);
	if (it == mGraphs.end()) {
		CI_LOG_W("No graph found with id '" << id << "'");
		return false;
	}
	return !it->second.hidden;
}

void GraphView::draw() {
	if (!mFbo) return;

	render();
	gl::draw(mFbo->getColorTexture());

	const float lineHeight = mLabelFont.getSize();
	vec2 labelPos		   = mLabelOffset + vec2(0, getHeight());

	if (!mLabelsEnabled) {
		return;
	}

	for (auto idIt = mGraphIdsInOrder.rbegin(); idIt != mGraphIdsInOrder.rend(); ++idIt) {
		const auto graphIt = mGraphs.find(*idIt);
		const auto & graph = graphIt->second;
		if (graph.hidden) {
			continue;
		}
		const int index		   = (int)((mCapacity - graph.index) % mCapacity);
		const float value	  = graph.values[index];
		const string labelText = graphIt->first + ": " + to_string(value);
		gl::ScopedColor labelColor(graph.maxColor);
		if (mLayout == Layout::LabelsLeft) {
			const vec2 labelSize = mLabelTextureFont->measureString(labelText);
			mLabelTextureFont->drawString(labelText, labelPos + labelSize * vec2(-1.0f, 0.0f));
		} else {
			mLabelTextureFont->drawString(labelText, labelPos);
		}
		labelPos.y -= lineHeight;
	}
}

inline void GraphView::render() {
	if (!mNeedsUpdate) {
		return;
	}

	const ivec2 fboSize = mFbo->getSize();
	gl::ScopedViewport scopedViewport(ivec2(0), fboSize);
	gl::ScopedMatrices scopedMatrices;
	gl::setMatricesWindow(fboSize);

	mFbo->bindFramebuffer();

	gl::clear(getBackgroundColor());

	for (const auto & graphId : mGraphIdsInOrder) {
		const auto graphIt = mGraphs.find(graphId);
		const auto & graph = graphIt->second;
		if (graph.hidden) {
			continue;
		}
		mGlsl->uniform("uMinColor", graph.minColor);
		mGlsl->uniform("uMaxColor", graph.maxColor);
		mGlsl->uniform("uSize", mFbo->getSize());
		mGlsl->uniform("uMin", graph.min);
		mGlsl->uniform("uMax", graph.max);
		mGlsl->uniform("uIndex", (int)graph.index);
		mGlsl->uniform("uValues", graph.values.data(), (int)graph.values.capacity());
		mGlsl->uniform("uStyle", mStyle == Style::Line ? 0 : 1);
		mBatch->draw();
	}

	mFbo->unbindFramebuffer();

	mNeedsUpdate = false;
}

void GraphView::setupShaders(const int numValues) {
	try {
		string vert = CI_GLSL(150, uniform mat4 ciModelViewProjection; uniform ivec2 uSize; in vec4 ciPosition;
							  out vec4 vPosition; void main(void) {
								  vPosition   = ciPosition;
								  gl_Position = ciModelViewProjection * (ciPosition * vec4(uSize, 1, 1));
							  });
		string frag = CI_GLSL(150,
							  uniform ivec2 uSize;
							  uniform int uStyle;  // 0 = line, 1 = gradient
							  uniform vec4 uMinColor = vec4(0, 1, 0, 1); uniform vec4 uMaxColor = vec4(1, 0, 0, 1);
							  uniform float uValues[NUM_VALUES]; uniform int uIndex = 0; uniform float uMin = 0.0;
							  uniform float uMax = 1.0; in vec4 vPosition; out vec4 oColor;

							  void main(void) {
								  float range = uMax - uMin;

								  if (NUM_VALUES <= 0 || range == 0) {
									  discard;
								  }

								  int column	 = int(vPosition.x * float(NUM_VALUES));
								  int index		 = NUM_VALUES - int(mod(uIndex + column, NUM_VALUES)) - 1;
								  float value	= clamp((uValues[index] - uMin) / range, 0, 1.0);
								  float y		 = 1.0 - vPosition.y;
								  float pxHeight = 1.0 / uSize.y;

								  if (value < y) {
									  discard;

								  } else if (uStyle == 0 && value <= y + pxHeight) {
									  // line
									  oColor = mix(uMinColor, uMaxColor, y);

								  } else if (uStyle == 1) {
									  // gradient
									  oColor = mix(uMinColor, uMaxColor, y);
								  }
							  });

		mGlsl = gl::GlslProg::create(
			gl::GlslProg::Format().vertex(vert).fragment(frag).define("NUM_VALUES " + to_string(numValues)));
		mBatch = gl::Batch::create(geom::Rect(Rectf(0, 0, 1, 1)), mGlsl);

	} catch (Exception e) {
		CI_LOG_EXCEPTION("Could not set up shaders", e);
		mGlsl  = nullptr;
		mBatch = nullptr;
	}
}

}  // namespace views
}  // namespace bluecadet