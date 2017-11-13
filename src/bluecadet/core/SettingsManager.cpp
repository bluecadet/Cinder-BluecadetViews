#include "SettingsManager.h"

#include "../views/BaseView.h"

#include <algorithm>
#include <string>	

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace core {

SettingsManagerRef SettingsManager::sInstance = nullptr;

SettingsManager::SettingsManager() {
}
SettingsManager::~SettingsManager() {}

void SettingsManager::setup(ci::app::App::Settings * appSettings, ci::fs::path jsonPath, std::function<void(SettingsManager * manager)> callback) {

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
	} else {
		CI_LOG_D("No settings file specified. Using defaults.");
	}

	if (callback) {
		callback(this);
	}

	// Parse arguments from command line
	addCommandLineParser("debug", [&](const string &value) { mDebugEnabled = value == "true"; });
	addCommandLineParser("fullscreen", [&](const string &value) { mFullscreen = value == "true"; });
	addCommandLineParser("borderless", [&](const string &value) { mBorderless = value == "true"; });
	addCommandLineParser("vsync", [&](const string &value) { mVerticalSync = value == "true"; });
	addCommandLineParser("console", [&](const string &value) { mConsole = value == "true"; });
	addCommandLineParser("cursor", [&](const string &value) { mShowCursor = value == "true"; });
	addCommandLineParser("mouse", [&](const string &value) { mMouseEnabled = value == "true"; });
	addCommandLineParser("tuio", [&](const string &value) { mTuioTouchEnabled = value == "true"; });
	addCommandLineParser("native", [&](const string &value) { mNativeTouchEnabled = value == "true"; });
	addCommandLineParser("drawTouches", [&](const string &value) { mShowTouches = value == "true"; });
	addCommandLineParser("draw_touches", [&](const string &value) { mShowTouches = value == "true"; });
	addCommandLineParser("drawStats", [&](const string &value) { mShowStats = value == "true"; });
	addCommandLineParser("draw_stats", [&](const string &value) { mShowStats = value == "true"; });
	addCommandLineParser("minimizeParams", [&](const string &value) { mMinimizeParams = value == "true"; });
	addCommandLineParser("minimize_params", [&](const string &value) { mMinimizeParams = value == "true"; });
	addCommandLineParser("collapseParams", [&](const string &value) { mCollapseParams = value == "true"; });
	addCommandLineParser("collapse_params", [&](const string &value) { mCollapseParams = value == "true"; });
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
	settings->setConsoleWindowEnabled(mConsole);
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
	setFieldFromJsonIfExists(&mConsole, "settings.general.consoleWindowEnabled");
	setFieldFromJsonIfExists(&mConsole, "settings.general.console");
	setFieldFromJsonIfExists(&mAppVersion, "settings.general.version");

	// Display
	setFieldFromJsonIfExists(&mDisplaySize.x, "settings.display.size.x");
	setFieldFromJsonIfExists(&mDisplaySize.y, "settings.display.size.y");
	setFieldFromJsonIfExists(&mDisplayColumns, "settings.display.columns");
	setFieldFromJsonIfExists(&mDisplayRows, "settings.display.rows");

	// Window
	setFieldFromJsonIfExists(&mFps, "settings.window.fps");
	setFieldFromJsonIfExists(&mFps, "settings.window.FPS");
	setFieldFromJsonIfExists(&mVerticalSync, "settings.window.verticalSync");
	setFieldFromJsonIfExists(&mVerticalSync, "settings.window.vsync");
	setFieldFromJsonIfExists(&mFullscreen, "settings.window.fullscreen");
	setFieldFromJsonIfExists(&mBorderless, "settings.window.borderless");
	setFieldFromJsonIfExists(&mWindowSize.x, "settings.window.size.x");
	setFieldFromJsonIfExists(&mWindowSize.y, "settings.window.size.x");
	setFieldFromJsonIfExists(&mCameraOffset.x, "settings.window.cameraOffset.x");
	setFieldFromJsonIfExists(&mCameraOffset.y, "settings.window.cameraOffset.y");
	setFieldFromJsonIfExists(&mClearColor.r, "settings.window.clearColor.r");
	setFieldFromJsonIfExists(&mClearColor.g, "settings.window.clearColor.g");
	setFieldFromJsonIfExists(&mClearColor.b, "settings.window.clearColor.b");
	setFieldFromJsonIfExists(&mClearColor.a, "settings.window.clearColor.a");

	// Touch
	setFieldFromJsonIfExists(&mMouseEnabled, "settings.touch.mouse");
	setFieldFromJsonIfExists(&mTuioTouchEnabled, "settings.touch.tuio");
	setFieldFromJsonIfExists(&mNativeTouchEnabled, "settings.touch.native");

	// Debug
	setFieldFromJsonIfExists(&mDebugEnabled, "settings.debug.debugMode");
	setFieldFromJsonIfExists(&mDebugEnabled, "settings.debug.debugEnabled");
	setFieldFromJsonIfExists(&mShowStats, "settings.debug.showStats");
	setFieldFromJsonIfExists(&mShowMinimap, "settings.debug.showMinimap");
	setFieldFromJsonIfExists(&mShowTouches, "settings.debug.showTouches");
	setFieldFromJsonIfExists(&mShowScreenLayout, "settings.debug.showScreenLayout");
	setFieldFromJsonIfExists(&mShowCursor, "settings.debug.showMouse");
	setFieldFromJsonIfExists(&mShowCursor, "settings.debug.showCursor");
	setFieldFromJsonIfExists(&mMinimizeParams, "settings.debug.minimizeParams");
	setFieldFromJsonIfExists(&mCollapseParams, "settings.debug.collapseParams");
	setFieldFromJsonIfExists(&mZoomToggleHotkeyEnabled, "settings.debug.zoomToggleHotkey");
	setFieldFromJsonIfExists(&mDisplayIdHotkeysEnabled, "settings.debug.displayIdHotkeys");
}

void SettingsManager::parseCommandLineArgs(const std::vector<std::string>& args) {
	string allArgsStr;

	for (auto arg : args) {
		int splitIndex = (int)arg.find_first_of("=");
		if (splitIndex != std::string::npos) {
			allArgsStr += " " + arg;

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
	
	if (!allArgsStr.empty()) {
		CI_LOG_I("Launching with command line args: '" + allArgsStr + "'");
	}
}

ci::params::InterfaceGlRef SettingsManager::getParams() {
	static ci::params::InterfaceGlRef params = nullptr;
	if (!params) {
		params = ci::params::InterfaceGl::create("Settings", ci::ivec2(250, 250));
		params->addParam("Show Layout", &mShowScreenLayout).group("App").key("l");
		params->addParam("Show Minimap", &mShowMinimap).group("App").key("m");
		params->addParam("Show Stats", &mShowStats).group("App").key("s");

		params->addParam("Show Touches", &mShowTouches).group("App").key("t");
		params->addParam("Show Cursor", &mShowCursor).updateFn([&] { mShowCursor ? ci::app::AppBase::get()->showCursor() : ci::app::AppBase::get()->hideCursor(); }).key("c").group("App");
		
		static int boundIndex = 0;
		params->addParam("View Bounds", {"None", "Visible", "All"}, [&](int i) {
			boundIndex = i;
			bluecadet::views::BaseView::sDebugDrawBounds = boundIndex >= 1;
			bluecadet::views::BaseView::sDebugDrawInvisibleBounds = boundIndex >= 2;
		}, [&] { return boundIndex; }).key("b").group("App");

		if (mMinimizeParams) {
			params->minimize();
		}

		params->addText("Version " + mAppVersion);

		if (mCollapseParams) {
			params->setOptions("App", "opened=false");
		}
	}
	return params;
}

}
}