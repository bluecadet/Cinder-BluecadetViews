#include "GraphView.h"

#include <algorithm>

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

GraphView::GraphView(const ci::ivec2 & size, const Style style) : BaseView(),
mNeedsUpdate(false),
mLabelsEnabled(true),
mCapacity(size.x),
mStyle(style)
{
	gl::Texture::Format texFormat;
	texFormat
		.internalFormat(GL_RGBA)
		.mipmap(false)
		.minFilter(GL_LINEAR)
		.magFilter(GL_NEAREST);

	gl::Fbo::Format fboFormat;
	fboFormat
		.colorTexture(texFormat)
		.disableDepth();

	if (mCapacity <= 0) {
		console() << "GraphView: Error: Size must be wider and taller than 0" << endl;
		return;
	};

	mFbo = ci::gl::Fbo::create(size.x, size.y, fboFormat);

	BaseView::setSize(mFbo->getSize());

	setupShaders((int)mCapacity);
}

GraphView::~GraphView() {
}

void GraphView::addGraph(const std::string & id, const float min, const float max) {
	addGraph(id, min, max, ci::ColorA(0.0f, 1.0f, 0, 0.5f), ci::ColorA(1.0f, 0.0f, 0, 0.5f));
}

void GraphView::addGraph(const std::string& id, const float min, const float max, const ci::ColorA color) {
	addGraph(id, min, max, color, color);
}

void GraphView::addGraph(const std::string& id, const float min, const float max, const ci::ColorA minColor, const ci::ColorA maxColor) {
	Graph graph;
	graph.min = min;
	graph.max = max;
	graph.minColor = minColor;
	graph.maxColor = maxColor;
	graph.values = vector<float>(mCapacity, 0.0f);
	graph.index = 0;
	mGraphs[id] = graph;
	mNeedsUpdate = true;
}

void GraphView::addValue(const std::string & id, const float value) {
	auto it = mGraphs.find(id);
	if (it == mGraphs.end()) {
		console() << "GraphView: Warning: No graph found with id '" << id << "'" << endl;
		return;
	}
	if (mCapacity <= 0) {
		return;
	}
	Graph & graph = it->second;
	graph.values[mCapacity - graph.index - 1] = value;
	graph.index = (graph.index + 1) % mCapacity;
	mNeedsUpdate = true;
}

void GraphView::setValues(const std::string & id, const std::vector<float> & values) {
	if (values.size() != mCapacity) {
		console() << "GraphView: Warning: Values must be an array of size '" << mCapacity << "' and not '" << to_string(values.size()) << "'" << endl;
		return;
	}
	auto it = mGraphs.find(id);
	if (it == mGraphs.end()) {
		console() << "GraphView: Warning: No graph found with id '" << id << "'" << endl;
		return;
	}
	it->second.values = values;
	mNeedsUpdate = true;
}

void GraphView::draw() {
	if (!mFbo) return;
	render();
	gl::draw(mFbo->getColorTexture());
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

	gl::clear(ColorA(0, 0, 0, 0.25));

	static const Font labelFont("Arial", 16);
	vec2 labelPos(0, getSize().y);

	for (const auto & graphPair : mGraphs) {
		const auto & graph = graphPair.second;
		mGlsl->uniform("uMinColor", graph.minColor);
		mGlsl->uniform("uMaxColor", graph.maxColor);
		mGlsl->uniform("uSize", mFbo->getSize());
		mGlsl->uniform("uMin", graph.min);
		mGlsl->uniform("uMax", graph.max);
		mGlsl->uniform("uIndex", (int)graph.index);
		mGlsl->uniform("uValues", graph.values.data(), (int)graph.values.capacity());
		mGlsl->uniform("uStyle", mStyle == Style::Line ? 0 : 1);
		mBatch->draw();

		if (mLabelsEnabled) {
			labelPos.y -= labelFont.getSize();
			const int index = (int)((mCapacity - graph.index) % mCapacity);
			const float value = graph.values[index];
			const ColorA labelColor = graph.maxColor;
			const string labelText = graphPair.first + ": " + to_string(value);
			gl::drawString(labelText, labelPos, labelColor, labelFont);
		}
	}

	mFbo->unbindFramebuffer();

	mNeedsUpdate = false;
}

void GraphView::setupShaders(const int numValues) {
	try {
		string vert = CI_GLSL(150,
			uniform mat4 ciModelViewProjection;
			uniform ivec2 uSize;
			in vec4 ciPosition;
			out vec4 vPosition;
			void main(void) {
				vPosition = ciPosition;
				gl_Position = ciModelViewProjection * (ciPosition * vec4(uSize, 1, 1));
			}
		);
		string frag = CI_GLSL(150,
			uniform ivec2 uSize;
			uniform int uStyle; // 0 = line, 1 = gradient
			uniform vec4 uMinColor = vec4(0, 1, 0, 1);
			uniform vec4 uMaxColor = vec4(1, 0, 0, 1);
			uniform float uValues[NUM_VALUES];
			uniform int uIndex = 0;
			uniform float uMin = 0.0;
			uniform float uMax = 1.0;
			in vec4 vPosition;
			out vec4 oColor;

			void main(void) {
				float range = uMax - uMin;

				if (NUM_VALUES <= 0 || range == 0) {
					discard;
				}

				int column = int(vPosition.x * float(NUM_VALUES));
				int index = NUM_VALUES - int(mod(uIndex + column, NUM_VALUES)) - 1;
				float value = (uValues[index] - uMin) / range;
				float y = 1.0 - vPosition.y;
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
			}
		);

		mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
			.vertex(vert)
			.fragment(frag)
			.define("NUM_VALUES " + to_string(numValues))
		);
		mBatch = gl::Batch::create(geom::Rect(Rectf(0, 0, 1, 1)), mGlsl);

	} catch (Exception e) {
		console() << "Could not set up shaders: " << e.what() << endl;
		mGlsl = nullptr;
		mBatch = nullptr;
	}
}

}
}