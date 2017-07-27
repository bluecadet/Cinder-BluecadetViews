#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/TextView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::text;

class TextViewSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	TextViewRef	mTextView;
};

void TextViewSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->setup(settings, "", [](SettingsManager * manager) {
		manager->mFullscreen = false;
		manager->mWindowSize = ivec2(1280, 720);
		manager->mMinimizeParams = false;
		manager->mCollapseParams = true;
	});
}

void TextViewSampleApp::setup() {

	BaseApp::setup();

	// Optional: configure the size and background of your root view
	getRootView()->setBackgroundColor(Color::gray(0.5f));

	// Sample content
	mTextView = TextViewRef(new TextView());

	mTextView->setWidth(500.0f); // This will cause the text to break and flow automatically. This is the same as calling setSize(400, 0)
	//mTextView->setHeight(500.0f); // This will cause any text beyond 400 px to be cut off vertically

	// Background color is independent of text styles
	mTextView->setBackgroundColor(ColorA(1.0f, 0, 0.5f, 0.75f));

	// Set all styles before setting your text
	mTextView->setTextColor(Color(1.0f, 1.0f, 1.0f));
	mTextView->setFontSize(32.0f);
	mTextView->setTextAlign(TextAlign::Center);

	// All styles will be applied to text now
	mTextView->setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n\nNec vero alia sunt quaerenda contra Carneadeam illam sententiam. Atque haec coniunctio confusioque virtutum tamen a philosophis ratione quadam distinguitur. ");

	// Configure debug parameters
	static int clipMode = mTextView->getClipMode();
	static vector<string> clipModes = {"Clip", "No Clip"};

	static int layoutMode = mTextView->getLayoutMode();
	static vector<string> layoutModes = {"Word Wrap", "No Wrap", "Single Line"};

	static int textAlign = mTextView->getCurrentStyle().mTextAlign;
	static vector<string> textAligns = {"Left", "Right", "Center"};

	auto params = SettingsManager::getInstance()->getParams();
	params->setSize(params->getSize() + ivec2(0, 200));
	
	params->addSeparator();
	params->addParam("Clip Mode", clipModes, &clipMode).updateFn([&]() { mTextView->setClipMode((StyledTextLayout::ClipMode)clipMode); });
	params->addParam("Layout Mode", layoutModes, &layoutMode).updateFn([&]() { mTextView->setLayoutMode((StyledTextLayout::LayoutMode)layoutMode); });
	params->addParam("Text Align", textAligns, &textAlign).updateFn([&]() { mTextView->setTextAlign((TextAlign)textAlign); });
	params->addParam<bool>("Trim Text Size", [&](bool v) { mTextView->setSizeTrimmingEnabled(v); }, [&] { return mTextView->getSizeTrimmingEnabled(); });
	params->addSeparator();
	params->addButton("Set to Auto-Width", [&] { mTextView->setWidth(0); });
	params->addButton("Set to Auto-Height", [&] { mTextView->setHeight(0); });
	params->addSeparator();
	params->addParam<float>("View Width", [&](float v) { mTextView->setWidth(v); }, [&] { return mTextView->getWidth(); });
	params->addParam<float>("View Height", [&](float v) { mTextView->setHeight(v); }, [&] { return mTextView->getHeight(); });
	params->addSeparator();
	params->addParam<float>("Padding Top", [&](float v) { mTextView->setPaddingTop(v); }, [&] { return mTextView->getPaddingTop(); });
	params->addParam<float>("Padding Right", [&](float v) { mTextView->setPaddingRight(v); }, [&] { return mTextView->getPaddingRight(); });
	params->addParam<float>("Padding Bottom", [&](float v) { mTextView->setPaddingBottom(v); }, [&] { return mTextView->getPaddingBottom(); });
	params->addParam<float>("Padding Left", [&](float v) { mTextView->setPaddingLeft(v); }, [&] { return mTextView->getPaddingLeft(); });
	params->addSeparator();
	params->addParam<bool>("Leading Disabled", [&](bool v) { mTextView->setLeadingDisabled(v); }, [&] { return mTextView->getLeadingDisabled(); });
	params->addParam<float>("Leading Offset", [&](float v) { mTextView->setLeadingOffset(v); }, [&] { return mTextView->getCurrentStyle().mLeadingOffset; }).step(1);
	params->addSeparator();
	params->addParam<float>("Max Width (read only)", [&](float v) {}, [&] { return mTextView->getMaxWidth(); });
	params->addParam<float>("Max Height (read only)", [&](float v) {}, [&] { return mTextView->getMaxHeight(); });
	params->addParam<float>("Text Width (read only)", [&](float v) {}, [&] { return mTextView->getTextWidth(); });
	params->addParam<float>("Text Height (read only)", [&](float v) {}, [&] { return mTextView->getTextHeight(); });
	params->addSeparator();

	getRootView()->addChild(mTextView);
}

void TextViewSampleApp::update() {
	BaseApp::update();

	mTextView->setPosition((vec2(getWindowSize()) - mTextView->getSize()) * 0.5f);

	static bool initialized = false;
	if (!initialized) {
		initialized = true;
		auto params = SettingsManager::getInstance()->getParams();
		params->setPosition(vec2(getWindowWidth() - params->getWidth() - params->getPosition().x, params->getPosition().y));
	}
}

CINDER_APP(TextViewSampleApp, RendererGl(RendererGl::Options().msaa(4)), TextViewSampleApp::prepareSettings);
