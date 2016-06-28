#include "GraphView.h"

using namespace ci;
using namespace ci::app;
using namespace std;


namespace bluecadet {
namespace views {

GraphView::GraphView(const ci::ivec2& size, const int pxPerValue) : BaseView(),
mNeedsUpdate(false),
mPxPerValue(pxPerValue)
{
	gl::Texture::Format texFormat;
	texFormat
		.mipmap(false)
		.minFilter(GL_LINEAR)
		.magFilter(GL_NEAREST);

	gl::Fbo::Format fboFormat;
	fboFormat
		.colorTexture(texFormat)
		.disableDepth()
		.stencilBuffer(false);
	mFbo = ci::gl::Fbo::create(size.x / mPxPerValue, size.y / mPxPerValue, fboFormat);
}

GraphView::~GraphView() {
}

void GraphView::addGraph(const std::string& id, const float min, const float max, const ci::ColorA color) {
	Graph graph;
	graph.min = min;
	graph.max = max;
	graph.color = color;
	graph.values = vector<float>(mFbo->getSize().x, 0.0f);
	graph.index = 0;
	mGraphs[id] = graph;
	mNeedsUpdate = true;
}

void GraphView::addValue(const std::string& id, const float value) {
	auto it = mGraphs.find(id);
	if (it == mGraphs.end()) return;
	Graph& graph = it->second;
	const size_t size = graph.values.capacity();
	graph.values[size - graph.index - 1] = value;
	graph.index = (graph.index + 1) % graph.values.capacity();
	mNeedsUpdate = true;
}

void GraphView::drawContent() {
	render();
	gl::ScopedMatrices scopedMatrices;
	gl::scale(vec2((float)mPxPerValue));
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

	mFbo->bindFramebuffer(); {
		gl::clear(ColorA(0, 0, 0, 0.25));

		const float w = (float)fboSize.x;
		const float h = (float)fboSize.y;
		Rectf r;

		for (const auto& graphPair : mGraphs) {
			const auto& graph = graphPair.second;
			const int idx = (int)graph.index;
			const float range = graph.max - graph.min;

			gl::ScopedColor scopedColor(graph.color);

			for (int i = 0; i < w; ++i) {
				const float v = (graph.values[i] - graph.min) / range;
				const float x = (float)(w - (i + idx) % fboSize.x);
				const float y = v * h;
				r.x1 = x;
				r.x2 = x + 1;
				r.y1 = h - y;
				r.y2 = h;
				gl::drawSolidRect(r);
			}
		}

	} mFbo->unbindFramebuffer();
}

}
}