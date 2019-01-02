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
	void mouseMove(MouseEvent event) override;
	TextViewRef	mTextView;
};

void TextViewSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->setup(settings, "", [](SettingsManager * manager) {
		manager->mFullscreen = false;
		manager->mWindowSize = ivec2(1280, 720);
	});
}

void TextViewSampleApp::setup() {

	BaseApp::setup();

	// Get the params out of the way
	SettingsManager::getInstance()->getParams()->minimize();

	// Optional: configure the size and background of your root view
	getRootView()->setBackgroundColor(Color::gray(0.0f));
	getRootView()->setSize(ScreenLayout::getInstance()->getAppSize());

	// Sample content
	mTextView = TextViewRef(new TextView());

	// Setting only the width will cause the text to break and flow automatically. This is the same as calling setSize(400, -1)
	mTextView->setWidth(400.0f);
	//mTextView->setHeight(400.0f); // this would cause any text beyond 400 px to be cut off vertically

	// Background color is independent of text styles
	//mTextView->setBackgroundColor(ColorA(1, 0, 1, 0.75f));

	//mTextView->setBlendMode(BaseView::BlendMode::PREMULT);
	//mTextView->setAlpha(0.1f);

	// Set all styles before setting your text
	mTextView->setTextColor(Color(1.0f, 1.0f, 1.0f));
	mTextView->setFontSize(32.0f);
	mTextView->setTextAlign(TextAlign::Center);

	TokenParserMapRef customTokenParsers = make_shared<TokenParserMap>();
	(*customTokenParsers)[L"<em>"] = [](StringType token, const int options, std::vector<StyledText> &segments, std::stack<Style> &styles) {
		Style style = Style(styles.top()).color(ColorA(1.0f, 0.0f, 0.0f, 1.0f));
		styles.push(style);
	};

	// All styles will be applied to text now
	mTextView->setText("Lorem ipsum dolor sit amet, <em>consectetur</em> adipiscing elit. Nec vero alia sunt quaerenda contra Carneadeam illam sententiam. Atque haec coniunctio confusioque virtutum tamen a philosophis ratione quadam distinguitur. ", customTokenParsers);

	// Set new styles for additional text
	mTextView->setTextColor(Color(0.25f, 0.25f, 1.0f), false);
	//mTextView->setTextColor(Color(0.25f, 0.25f, 1.0f), true); // this would apply the text color to all existing text
	mTextView->appendText("<em>Sit hoc ultimum bonorum,</em> quod nunc a me defenditur; Duo Reges: constructio interrete. Quid est, quod ab ea absolvi et perfici debeat? Nunc haec primum fortasse audientis servire debemus. Scaevola tribunus plebis ferret ad plebem vellentne de ea re quaeri. Sed in rebus apertissimis nimium longi sumus.", customTokenParsers);

	getRootView()->addChild(mTextView);
}

void TextViewSampleApp::mouseMove(MouseEvent event) {
	float x = event.getX();
	mTextView->setWidth(x - mTextView->getPosition().value().x);
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(TextViewSampleApp, RendererGl(RendererGl::Options().msaa(4)), TextViewSampleApp::prepareSettings);
