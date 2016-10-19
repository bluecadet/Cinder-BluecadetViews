#include "SettingsManager.h"

#include <algorithm>
#include <string>	

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace core {

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
	mWindowSize = ivec2(0);

	// Analytics
	mAnalyticsAppName = "";
	mAnalyticsTrackingId = "";
	mAnalyticsClientId = "";
}
SettingsManager::~SettingsManager() {}

// Pull in the shared/standard app settings JSON
void SettingsManager::setup(const ci::fs::path& jsonPath, ci::app::App::Settings* appSettings) {
	// If the path exists, load it
	if (fs::exists(jsonPath)) {
		console() << "SettingsManager: Loading settings from '" << jsonPath << "'" << endl;

		try {
			DataSourceRef tree = loadFile(jsonPath);
			mAppSettingsDoc = (JsonTree)tree;

			// General
			setFieldFromJsonIfExists(&mConsoleWindowEnabled, "settings.general.consoleWindowEnabled");
			setFieldFromJsonIfExists(&mFps, "settings.general.FPS");
			setFieldFromJsonIfExists(&mAppVersion, "settings.general.appVersion");

			// Graphics
			setFieldFromJsonIfExists(&mVerticalSync, "settings.graphics.verticalSync");

			// Debug
			setFieldFromJsonIfExists(&mDebugMode, "settings.debug.debugMode");
			setFieldFromJsonIfExists(&mDrawMinimap, "settings.debug.drawMinimap");
			setFieldFromJsonIfExists(&mDrawTouches, "settings.debug.drawTouches");
			setFieldFromJsonIfExists(&mDrawScreenLayout, "settings.debug.drawScreenLayout");
			setFieldFromJsonIfExists(&mFullscreen, "settings.debug.fullscreen");
			setFieldFromJsonIfExists(&mBorderless, "settings.debug.borderless");
			setFieldFromJsonIfExists(&mShowMouse, "settings.debug.showMouse");

			// Analytics
			setFieldFromJsonIfExists(&mAnalyticsAppName, "settings.analytics.appName");
			setFieldFromJsonIfExists(&mAnalyticsTrackingId, "settings.analytics.trackingID");
			setFieldFromJsonIfExists(&mAnalyticsClientId, "settings.analytics.clientID");

		} catch (Exception e) {
			console() << "SettingsManager: ERROR: Could not load settings json: " << e.what() << endl;
		}
	} else if (!jsonPath.empty()) {
		console() << "SettingsManager: ERROR: Settings file does not exist at '" << jsonPath << "'" << std::endl;
	}

	// Parse arguments from command line
	addCommandLineParser("debug", [&](const string &value) { mDebugMode = value == "true"; });
	addCommandLineParser("fullscreen", [&](const string &value) { mFullscreen = value == "true"; });
	addCommandLineParser("borderless", [&](const string &value) { mBorderless = value == "true"; });
	addCommandLineParser("vsync", [&](const string &value) { mVerticalSync = value == "true"; });
	addCommandLineParser("console", [&](const string &value) { mConsoleWindowEnabled = value == "true"; });
	addCommandLineParser("cursor", [&](const string &value) { mShowMouse = value == "true"; });
	addCommandLineParser("mouse", [&](const string &value) { mShowMouse = value == "true"; });
	addCommandLineParser("size", [&](const string &value) {
		int commaIndex = (int)value.find(",");
		if (commaIndex != string::npos) {
			string wStr = value.substr(0, commaIndex);
			string hStr = value.substr(commaIndex + 1, value.size() - commaIndex - 1);
			mWindowSize = ivec2(stoi(wStr), stoi(hStr));
		}
	});
	
	parseCommandLineArgs(appSettings->getCommandLineArgs());

	// Default window size to main display size if no custom size has been determined
	if (mWindowSize == ivec2(0)) {
		mWindowSize = Display::getMainDisplay()->getSize();
	}
}

void SettingsManager::addCommandLineParser(const std::string &key, CommandLineArgParserFn callback)
{
	auto callbackListIt = mCommandLineArgsHandlers.find(key);

	if (callbackListIt == mCommandLineArgsHandlers.end()) {
		mCommandLineArgsHandlers[key] = vector<CommandLineArgParserFn>();
		callbackListIt = mCommandLineArgsHandlers.find(key);
	}

	callbackListIt->second.push_back(callback);
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
				callback(value);
			}
		}
	}
}

ci::params::InterfaceGlRef SettingsManager::getParams() {
	static ci::params::InterfaceGlRef params = nullptr;
	if (!params) {
		params = ci::params::InterfaceGl::create("Settings", ci::ivec2(250, 500));
		params->addParam("Show Minimap", &mDrawMinimap);
		params->addParam("Show Touches", &mDrawTouches);
		params->addParam("Show Layout", &mDrawScreenLayout);
		params->addParam("Show Cursor", &mShowMouse).updateFn([&] { mShowMouse ? ci::app::AppBase::get()->showCursor() : ci::app::AppBase::get()->hideCursor(); }).key("m");
	}
	return params;
}

}
}