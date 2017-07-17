#include "SettingsManager.h"

#include "../views/BaseView.h"
#include "cinder/Log.h"

#include <algorithm>
#include <string>	

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace core {

SettingsManagerRef SettingsManager::sInstance = nullptr;

// Initialization
SettingsManager::SettingsManager() {

	// General
	mConsoleWindowEnabled = true;
	mFps = 60;
	mAppVersion = "";

	// Graphics
	mVerticalSync = true;
	mClearColor = ci::ColorA(0, 0, 0, 1.0f);

	// Debugging
	mDebugMode = true;
	mDrawMinimap = true;
	mDrawTouches = false;
	mDrawScreenLayout = false;
	mFullscreen = true;
	mBorderless = false;
	mShowMouse = true;
	mDrawStats = true;
	mWindowSize = ivec2(0);

	// Analytics
	mAnalyticsAppName = "";
	mAnalyticsTrackingId = "";
	mAnalyticsClientId = "";
}
SettingsManager::~SettingsManager() {}

// Pull in the shared/standard app settings JSON
void SettingsManager::setup(ci::app::App::Settings * appSettings, ci::fs::path jsonPath, std::function<void(SettingsManager * manager)> overrideCallback) {
	// Set default path
	if (jsonPath.empty()) {
		jsonPath = ci::app::getAssetPath("appSettings.json");
	}

	// If the path exists, load it
	if (fs::exists(jsonPath)) {
		CI_LOG_D("Loading settings from '" << jsonPath << "'");

		try {
			mSettingsJson = JsonTree(loadFile(jsonPath));

			parseJson(mSettingsJson);

		} catch (Exception e) {
			CI_LOG_EXCEPTION("Could not parse json", e);
		}
	} else if (!jsonPath.empty()) {
		CI_LOG_E("Settings file does not exist at '" << jsonPath << "'");
	}

	if (overrideCallback) {
		overrideCallback(this);
	}

	// Parse arguments from command line
	addCommandLineParser("debug", [&](const string &value) { mDebugMode = value == "true"; });
	addCommandLineParser("fullscreen", [&](const string &value) { mFullscreen = value == "true"; });
	addCommandLineParser("borderless", [&](const string &value) { mBorderless = value == "true"; });
	addCommandLineParser("vsync", [&](const string &value) { mVerticalSync = value == "true"; });
	addCommandLineParser("console", [&](const string &value) { mConsoleWindowEnabled = value == "true"; });
	addCommandLineParser("cursor", [&](const string &value) { mShowMouse = value == "true"; });
	addCommandLineParser("mouse", [&](const string &value) { mMouseEnabled = value == "true"; });
	addCommandLineParser("tuio", [&](const string &value) { mTuioTouchEnabled = value == "true"; });
	addCommandLineParser("native", [&](const string &value) { mNativeTouchEnabled = value == "true"; });
	addCommandLineParser("drawTouches", [&](const string &value) { mDrawTouches = value == "true"; });
	addCommandLineParser("draw_touches", [&](const string &value) { mDrawTouches = value == "true"; });
	addCommandLineParser("drawStats", [&](const string &value) { mDrawStats = value == "true"; });
	addCommandLineParser("draw_stats", [&](const string &value) { mDrawStats = value == "true"; });
	addCommandLineParser("minimizeParams", [&](const string &value) { mMinimizeParams = value == "true"; });
	addCommandLineParser("minimize_params", [&](const string &value) { mMinimizeParams = value == "true"; });
	addCommandLineParser("zoom_toggle_hotkey", [&](const string &value) { mZoomToggleHotkeyEnabled = value == "true"; });
	addCommandLineParser("display_id_hotkey", [&](const string &value) { mDisplayIdHotkeysEnabled = value == "true"; });
	addCommandLineParser("size", [&](const string &value) {
		int commaIndex = (int)value.find(",");
		if (commaIndex != string::npos) {
			string wStr = value.substr(0, commaIndex);
			string hStr = value.substr(commaIndex + 1, value.size() - commaIndex - 1);
			mWindowSize = ivec2(stoi(wStr), stoi(hStr));
		}
	});
	addCommandLineParser("offset", [&](const string &value) {
		int commaIndex = (int)value.find(",");
		if (commaIndex != string::npos) {
			string xStr = value.substr(0, commaIndex);
			string yStr = value.substr(commaIndex + 1, value.size() - commaIndex - 1);
			mCameraOffset = ivec2(stoi(xStr), stoi(yStr));
		}
	});

	parseCommandLineArgs(appSettings->getCommandLineArgs());
	applyToAppSettings(appSettings);
}

void SettingsManager::applyToAppSettings(ci::app::App::Settings * settings) {
	// Default window size to main display size if no custom size has been determined
	if (mWindowSize == ivec2(0)) {
		mWindowSize = Display::getMainDisplay()->getSize();
	}

	// Apply pre-launch settings
#ifdef CINDER_MSW
	settings->setConsoleWindowEnabled(mConsoleWindowEnabled);
#endif
	settings->setFrameRate((float)mFps);
	settings->setWindowSize(mWindowSize);
	settings->setBorderless(mBorderless);
	settings->setFullScreen(mFullscreen);

	if (mNativeTouchEnabled) {
		settings->setMultiTouchEnabled(true);
	}

	// Keep window top-left within display bounds
	if (settings->getWindowPos().x == 0 && settings->getWindowPos().y == 0) {
		ivec2 windowPos = (Display::getMainDisplay()->getSize() - settings->getWindowSize()) / 2;
		windowPos = glm::max(windowPos, ivec2(0));
		settings->setWindowPos(windowPos);
	}
}

void SettingsManager::addCommandLineParser(const std::string& key, CommandLineArgParserFn callback) {
	string lowercaseKey = key;
	std::transform(lowercaseKey.begin(), lowercaseKey.end(), lowercaseKey.begin(), ::tolower);

	auto callbackListIt = mCommandLineArgsHandlers.find(lowercaseKey);

	if (callbackListIt == mCommandLineArgsHandlers.end()) {
		mCommandLineArgsHandlers[lowercaseKey] = vector<CommandLineArgParserFn>();
		callbackListIt = mCommandLineArgsHandlers.find(lowercaseKey);
	}

	callbackListIt->second.push_back(callback);
}

void SettingsManager::parseJson(ci::JsonTree & json) {
	// General
	setFieldFromJsonIfExists(&mConsoleWindowEnabled, "settings.general.consoleWindowEnabled");
	setFieldFromJsonIfExists(&mFps, "settings.general.FPS");
	setFieldFromJsonIfExists(&mAppVersion, "settings.general.appVersion");

	// Graphics
	setFieldFromJsonIfExists(&mVerticalSync, "settings.graphics.verticalSync");
	setFieldFromJsonIfExists(&mFullscreen, "settings.graphics.fullscreen");
	setFieldFromJsonIfExists(&mBorderless, "settings.graphics.borderless");

	// Debug
	setFieldFromJsonIfExists(&mDebugMode, "settings.debug.debugMode");
	setFieldFromJsonIfExists(&mShowMouse, "settings.debug.showMouse");
	setFieldFromJsonIfExists(&mDrawStats, "settings.debug.drawStats");
	setFieldFromJsonIfExists(&mDrawMinimap, "settings.debug.drawMinimap");
	setFieldFromJsonIfExists(&mDrawTouches, "settings.debug.drawTouches");
	setFieldFromJsonIfExists(&mDrawScreenLayout, "settings.debug.drawScreenLayout");
	setFieldFromJsonIfExists(&mMinimizeParams, "settings.debug.minimizeParams");
	setFieldFromJsonIfExists(&mZoomToggleHotkeyEnabled, "settings.debug.zoomToggleHotkey");
	setFieldFromJsonIfExists(&mDisplayIdHotkeysEnabled, "settings.debug.displayIdHotkeys");

	// Touch
	setFieldFromJsonIfExists(&mMouseEnabled, "settings.touch.mouse");
	setFieldFromJsonIfExists(&mTuioTouchEnabled, "settings.touch.tuio");
	setFieldFromJsonIfExists(&mNativeTouchEnabled, "settings.touch.native");

	// Analytics
	setFieldFromJsonIfExists(&mAnalyticsAppName, "settings.analytics.appName");
	setFieldFromJsonIfExists(&mAnalyticsTrackingId, "settings.analytics.trackingID");
	setFieldFromJsonIfExists(&mAnalyticsClientId, "settings.analytics.clientID");
}

void SettingsManager::parseCommandLineArgs(const std::vector<std::string>& args) {
	for (auto arg : args) {
		int splitIndex = (int)arg.find_first_of("=");
		if (splitIndex != std::string::npos) {
			std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
			std::string key = arg.substr(0, splitIndex);
			std::string value = arg.substr(splitIndex + 1, arg.size() - splitIndex - 1);
			auto callbackListIt = mCommandLineArgsHandlers.find(key);
			if (callbackListIt == mCommandLineArgsHandlers.end()) continue;

			for (auto callback : callbackListIt->second) {
				CI_LOG_D("Command line arg: " + key + "=" + value);
				callback(value);
			}
		}
	}
}

ci::params::InterfaceGlRef SettingsManager::getParams() {
	static ci::params::InterfaceGlRef params = nullptr;
	if (!params) {
		params = ci::params::InterfaceGl::create("Settings", ci::ivec2(250, 250));
		params->addParam("Show Layout", &mDrawScreenLayout).group("App").key("l");
		params->addParam("Show Touches", &mDrawTouches).group("App").key("t");
		params->addParam("Show Minimap", &mDrawMinimap).group("App").key("m");
		params->addParam("Show Stats", &mDrawStats).group("App").key("s");
		params->addParam("Show Cursor", &mShowMouse).updateFn([&] { mShowMouse ? ci::app::AppBase::get()->showCursor() : ci::app::AppBase::get()->hideCursor(); }).key("c").group("App");
		params->addParam("Show Bounds", &bluecadet::views::BaseView::sDebugDrawBounds).group("App").key("b");
		params->addParam("Show Invisible Bounds", &bluecadet::views::BaseView::sDebugDrawInvisibleBounds).group("App");

		if (mMinimizeParams) {
			params->minimize();
		}
	}
	return params;
}

}
}