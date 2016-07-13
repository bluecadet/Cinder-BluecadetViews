#include "StatsView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {
namespace debug {

StatsView::StatsView() : BaseView(),
mFont("Arial", 80.0f),
mFontColor(ColorA(1.0f, 1.0f, 1.0f, 0.75f))
{
}

StatsView::~StatsView() {
}

void StatsView::draw() {
	vec2 pos(0);
	const float rowHeight = mFont.getSize();
	for (auto& name : mAllStats) {
		const std::string str = name + ": " + getValueString(name);
		gl::drawString(str, pos, mFontColor, mFont);
		pos.y += rowHeight;
	}
}
std::string StatsView::getValueString(const std::string& name) const {
	{
		auto it = mFnStats.find(name);
		if (it != mFnStats.end()) {
			return it->second();
		}
	}
	{
		auto it = mBoolStats.find(name);
		if (it != mBoolStats.end()) {
			return to_string(*it->second);
		}
	}
	{
		auto it = mIntStats.find(name);
		if (it != mIntStats.end()) {
			return to_string(*it->second);
		}
	}
	{
		auto it = mFloatStats.find(name);
		if (it != mFloatStats.end()) {
			return to_string(*it->second);
		}
	}
	{
		auto it = mDoubleStats.find(name);
		if (it != mDoubleStats.end()) {
			return to_string(*it->second);
		}
	}

	return "";
}

}
}
}