#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"
#include "TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

class BaseAppSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	void draw() override;

protected:
	TouchViewRef mButton;
	gl::BatchRef mBackgroundBatch;
};

void BaseAppSampleApp::prepareSettings(ci::app::App::Settings* settings)
{
	BaseApp::prepareSettings(settings);
	settings->setFullScreen(false);
	settings->setWindowSize(1024, 768);
	settings->setBorderless(false);
}

void BaseAppSampleApp::setup()
{
	BaseApp::setup();

	mButton = TouchViewRef(new TouchView());
	mButton->setup(vec2(100.0f, 100.0f));
	mButton->setPosition((vec2(getWindowSize()) - mButton->getSize()) * 0.5f);

	mRootView->addChild(mButton);

	// simple shader batch to draw gradient across background
	auto prog = gl::GlslProg::create(
		gl::GlslProg::Format().vertex(CI_GLSL(150,
			uniform vec2	uSize;
			uniform mat4	ciModelViewProjection;
			in vec4			ciPosition;
			out vec4		normPos;
			void main(void) {
				normPos = ciPosition;
				vec4 position = vec4(ciPosition.x * uSize.x, ciPosition.y * uSize.y, 0.0f, 1.0f);
				gl_Position = ciModelViewProjection * position;
			}
		)).fragment(CI_GLSL(150,
			uniform float	uTime;
			in vec4			normPos;
			out vec4		oColor;
			void main(void) {
				oColor = normPos;
				oColor.z = uTime;
			}
		))
	);
	prog->uniform("uTime", 0);
	prog->uniform("uSize", vec2(0, 0));

	auto rect = geom::Rect().rect(Rectf(0, 0, 1, 1));
	mBackgroundBatch = gl::Batch::create(rect, prog);
}

void BaseAppSampleApp::update()
{
	BaseApp::update();
	//mBackgroundBatch->getGlslProg()->uniform("uTime", sinf((float)M_PI * getElapsedSeconds()) * 0.5f + 0.5f);
	mBackgroundBatch->getGlslProg()->uniform("uSize", vec2(getWindowSize()));
}

void BaseAppSampleApp::draw()
{
	gl::clear();
	mBackgroundBatch->draw();
	BaseApp::draw(false);
}

CINDER_APP(BaseAppSampleApp, RendererGl, BaseAppSampleApp::prepareSettings);
