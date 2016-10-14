#include "StatsView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {
namespace debug {

StatsView::StatsView() : StatsView(ci::Font("Arial", 16.0f)) {}

StatsView::StatsView(ci::Font font) : BaseView(),
mFont(font)
{
	mTextureFont = gl::TextureFont::create(mFont);
}

StatsView::~StatsView() {
}

void StatsView::addStat(const std::string& name, StatsFn fn)
{
	auto it = mStatFunctions.find(name);
	if (it != mStatFunctions.end()) {
		cout << "StatsView: Replacing existing stat with name " << name << endl;
		removeStat(name);
	}
	mStatFunctions[name] = fn;
}

void StatsView::removeStat(const std::string& name)
{
	const auto statsIt = mStatFunctions.find(name);
	if (statsIt == mStatFunctions.end()) {
		cout << "StatsView: Can't find stat with name " << name << endl;
		return;
	}
	mStatFunctions.erase(statsIt);
}

void StatsView::draw() {
	const float rowHeight = mFont.getSize();
	vec2 pos(0, mTextureFont->getAscent());

	for (const auto& it : mStatFunctions) {
		const string str = it.first + ": " + it.second();
		mTextureFont->drawString(str, pos);
		pos.y += rowHeight;
	}
}

}
}
}