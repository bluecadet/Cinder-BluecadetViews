#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class StatsView> StatsViewRef;

class StatsView : public BaseView {

public:

	StatsView(ci::Font font);
	StatsView();
	virtual ~StatsView();

	void addStat(const std::string & name, std::function<std::string()> fn);
	void removeStat(const std::string & name);

protected:
	virtual void draw() override;
	float getRowHeight() const { return mFont.getSize(); }

	ci::Font mFont;
	ci::gl::TextureFontRef mTextureFont;

	std::map<std::string, std::function<std::string()>> mStatFunctions;
};

}  // namespace views
}  // namespace bluecadet