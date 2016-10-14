#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"

#include "../BaseView.h"

namespace bluecadet {
namespace views {
namespace debug {

typedef std::shared_ptr<class StatsView> StatsViewRef;

class StatsView : public BaseView {

public:
	typedef std::function<std::string()> StatsFn;

	StatsView(ci::Font font);
	StatsView();
	virtual ~StatsView();

	void addStat(const std::string& name, StatsFn fn);
	void removeStat(const std::string& name);

protected:
	virtual void draw() override;

	ci::Font mFont;
	ci::gl::TextureFontRef mTextureFont;

	std::map<std::string, StatsFn> mStatFunctions;
};

}
}
}