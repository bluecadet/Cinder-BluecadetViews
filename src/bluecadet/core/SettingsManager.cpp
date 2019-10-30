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

SettingsManager::SettingsManager() {}
SettingsManager::~SettingsManager() {}

void SettingsManager::setup(ci::app::App::Settings * appSettings, ci::fs::path jsonPath, bool autoCreateJson,
							std::function<void(SettingsManager * manager)> callback) {
	mJsonPath = jsonPath;

	if (mJsonPath.empty()) {
		CI_LOG_I("Setting json path to default: '" << getDefaulJsonPath() << "'");
		mJsonPath = getDefaulJsonPath();
	}

	mapFields();

	load(jsonPath, autoCreateJson);

	if (callback) {
		callback(this);
	}

	setupCommandArgs();
	parseCommandArgs(appSettings->getCommandLineArgs());
	applyToAppSettings(appSettings);
}

void SettingsManager::mapFields() {
	// General
	mapField("settings.general.console", &mConsole).commandArgs({"console"});
	mapField("settings.general.version", &mAppVersion);

	// Display
	mapField("settings.display.index", &mDisplayIndex).commandArgs({"display", "display_index", "displayIndex"});
	mapField("settings.display.size", &mDisplaySize);
	mapField("settings.display.columns", &mDisplayColumns);
	mapField("settings.display.rows", &mDisplayRows);
	mapField("settings.display.bezel", &mBezelDims).commandArgs({"bezel", "display_bezel", "displayBezel"});

	// Window
	mapField("settings.window.fps", &mFps).commandArgs({"fps"});
	mapField("settings.window.vsync", &mVerticalSync).commandArgs({"vsync"});
	mapField("settings.window.fullscreen", &mFullscreen).commandArgs({"fullscreen"});;
	mapField("settings.window.borderless", &mBorderless).commandArgs({"borderless"});;
	mapField("settings.window.pos", &mWindowPos).commandArgs({"pos", "window_pos", "windowPos"});
	mapField("settings.window.size", &mWindowSize).commandArgs({"size", "window_size", "windowSize"});
	mapField("settings.window.clearColor", &mClearColor);

	// Camera
	mapField("settings.camera.offset", &mCameraOffset).commandArgs({"offset", "camera_offset", "cameraOffset"});
	mapField("settings.camera.zoom", &mCameraZoom).commandArgs({"zoom", "camera_zoom", "cameraZoom"});

	// Touch
	mapField("settings.touch.mouse", &mMouseEnabled).commandArgs({"mouse"});
	mapField("settings.touch.tuio", &mTuioTouchEnabled).commandArgs({"tuio"});
	mapField("settings.touch.native", &mNativeTouchEnabled).commandArgs({"native", "touch"});
	mapField("settings.touch.supportMultipleNativeTouchScreens", &mSupportMultipleNativeTouchScreens)
		.commandArgs({"supportMultipleNativeTouchScreens", "support_multiple_native_touch_screens"});;

	// Debug
	mapField("settings.debug.debugEnabled", &mDebugEnabled).commandArgs({"debug"});
	mapField("settings.debug.showStats", &mShowStats).commandArgs({"stats"});
	mapField("settings.debug.showMinimap", &mShowMinimap).commandArgs({"minimap"});
	mapField("settings.debug.showTouches", &mShowTouches).commandArgs({"show_touches", "showTouches"});
	mapField("settings.debug.showScreenLayout", &mShowScreenLayout).commandArgs({"show_layout", "showLayout"});
	mapField("settings.debug.showCursor", &mShowCursor).commandArgs({"cursor", "show_cursor", "showCursor"});
	mapField("settings.debug.minimizeParams", &mMinimizeParams).commandArgs({"minimize_params", "minimizeParams"});
	mapField("settings.debug.collapseParams", &mCollapseParams).commandArgs({"collapse_params", "collapseParams"});
	mapField("settings.debug.zoomToggleHotkey", &mZoomToggleHotkeyEnabled).commandArgs({"zoom_toggle_hotkey", "zoomToggleHotkey"});
	mapField("settings.debug.displayIdHotkeys", &mDisplayIdHotkeysEnabled).commandArgs({"display_id_hotkey", "displayIdHotkey"});
	mapField("settings.debug.touchSimulator.enabled", &mTouchSimEnabled).commandArgs({"touch_sim", "touchSim"});
}

void SettingsManager::setupCommandArgs() {
	for (auto & mapping : mFieldMappings) {
		for (auto & arg : mapping.second.commandArgNames) {
			addCommandArg(arg, [&] (const std::string & arg) {
				mapping.second.readCommandArg(arg);
			});
		}
	}
}

void SettingsManager::applyToAppSettings(ci::app::App::Settings * settings) {
	const float pixelScale = Display::getMainDisplay()->getContentScale();

	// Default window size to main display size if no custom size has been determined
	if (mWindowSize == ivec2(INT_MIN)) {
		mWindowSize = vec2(Display::getMainDisplay()->getSize()) * pixelScale;
	}

	// Apply pre-launch settings
#ifdef CINDER_MSW
	settings->setConsoleWindowEnabled(mConsole);
#endif

	if (mFps > 0) {
		settings->setFrameRate(mFps);
	} else {
		settings->disableFrameRate();
	}

	settings->setWindowSize(mWindowSize);
	settings->setBorderless(mBorderless);
	settings->setFullScreen(mFullscreen);

	if (mNativeTouchEnabled) {
		settings->setMultiTouchEnabled(true);
	}

	if (mDisplayIndex != 0) {
		const auto displays = Display::getDisplays();
		if (mDisplayIndex >= 0 && mDisplayIndex < displays.size()) {
			settings->setDisplay(displays.at(mDisplayIndex));
		}
	}

	// Default window position to centered in display if no custom pos has been set
	if (mWindowPos == ivec2(INT_MIN)) {
		ivec2 windowSizeInPx = vec2(settings->getWindowSize()) * pixelScale;
		ivec2 windowPos = (Display::getMainDisplay()->getSize() - windowSizeInPx) / 2;
		windowPos = glm::max(windowPos, ivec2(0));
		settings->setWindowPos(windowPos);
	} else {
		settings->setWindowPos(vec2(mWindowPos) * pixelScale);
	}
}

void SettingsManager::addCommandArg(const std::string & key, CommandArgParserFn callback) {
	string lowercaseKey = key;
	std::transform(lowercaseKey.begin(), lowercaseKey.end(), lowercaseKey.begin(), ::tolower);

	auto callbackListIt = mCommandArgsHandlers.find(lowercaseKey);

	if (callbackListIt == mCommandArgsHandlers.end()) {
		mCommandArgsHandlers[lowercaseKey] = vector<CommandArgParserFn>();
		callbackListIt = mCommandArgsHandlers.find(lowercaseKey);
	}

	callbackListIt->second.push_back(callback);
}

void SettingsManager::addCommandArgs(const std::set<std::string> & keys, CommandArgParserFn callback) {
	for (const auto key : keys) {
		addCommandArg(key, callback);
	}
}

void SettingsManager::load(ci::fs::path jsonPath, bool autoCreateJson) {
	if (jsonPath.empty()) {
		jsonPath = mJsonPath;
	}

	if (jsonPath.empty()) {
		CI_LOG_D("No settings file specified. Using defaults.");
	} else if (!fs::exists(jsonPath)) {
		if (autoCreateJson) {
			CI_LOG_I("Creating settings json at '" << jsonPath << "'");
			save(jsonPath);
		} else {
			CI_LOG_E("No settings file found at '" << jsonPath << "'");
		}

	} else {
		CI_LOG_I("Loading settings from '" << jsonPath << "'");

		try {
			mJson = JsonTree(loadFile(jsonPath));
			deserialize(mJson);

		} catch (Exception e) {
			CI_LOG_EXCEPTION("Could not parse json", e);
		}
	}
}

void SettingsManager::save(ci::fs::path jsonPath) {
	if (jsonPath.empty()) {
		jsonPath = mJsonPath;
	}

	if (!jsonPath.empty()) {
		CI_LOG_I("Saving settings to '" << jsonPath << "'");

		try {
			auto json = serialize();
			json.write(jsonPath, JsonTree::WriteOptions().indented(true));

		} catch (Exception e) {
			CI_LOG_EXCEPTION("Could not save json", e);
		}
	} else {
		CI_LOG_D("No settings file specified. Using defaults.");
	}
}

void SettingsManager::parseCommandArgs(const std::vector<std::string> & args) {
	string allArgsStr;

	for (auto arg : args) {
		int splitIndex = (int)arg.find_first_of("=");
		if (splitIndex != std::string::npos) {
			allArgsStr += " " + arg;

			std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
			std::string key = arg.substr(0, splitIndex);
			std::string value = arg.substr(splitIndex + 1, arg.size() - splitIndex - 1);
			auto callbackListIt = mCommandArgsHandlers.find(key);
			if (callbackListIt == mCommandArgsHandlers.end()) continue;

			for (auto callback : callbackListIt->second) {
				callback(value);
			}
		}
	}

	if (!allArgsStr.empty()) {
		CI_LOG_I("Launching with command line args: '" + allArgsStr + "'");
	}
}

void SettingsManager::deserialize(ci::JsonTree & json) {
	for (const auto & fieldName : mFieldMappingNames) {
		mergeField(fieldName, json);
	}
	mSignalSettingsLoaded.emit();
}

ci::JsonTree SettingsManager::serialize() {
	ci::JsonTree json;

	for (auto & fieldMapping : mFieldMappings) {
		auto fields = text::split<string, list<string>>(fieldMapping.first, '.');

		// grab value and pop from fields
		const string & key = fields.back();
		JsonTree value = fieldMapping.second.toJson(key);
		fields.pop_back();

		ci::JsonTree * parent = &json;

		// create parent hierarchy
		while (!fields.empty()) {
			const auto field = fields.front();

			if (!parent->hasChild(field)) {
				parent->addChild(JsonTree::makeObject(field));
			}

			parent = &(parent->getChild(field));

			fields.pop_front();
		}

		// add child
		parent->addChild(value);
	}

	return json;
}

void SettingsManager::mergeField(const std::string & fieldName, const ci::JsonTree & json) {
	try {
		if (!hasJsonValue(fieldName)) {
			CI_LOG_W("Could not find settings value for field name '" << fieldName << "' in json file");
			return;
		}
		auto it = mFieldMappings.find(fieldName);
		if (it == mFieldMappings.end()) {
			CI_LOG_E("No mapping exists for field name '" << fieldName << "'");
			return;
		}

		auto & mapping = it->second;

		mapping.readJson(json);

	} catch (cinder::Exception e) {
		CI_LOG_EXCEPTION("Could not set '" << fieldName << "' in json file", e);
	}
}

ci::params::InterfaceGlRef SettingsManager::createParams() {
	auto params = ci::params::InterfaceGl::create("Settings", toPixels(ci::ivec2(250, 250)));
	params->addParam("Show Layout", &mShowScreenLayout).group("App").key("l");
	params->addParam("Show Minimap", &mShowMinimap).group("App").key("m");
	params->addParam("Show Stats", &mShowStats).group("App").key("s");

	params->addParam("Show Touches", &mShowTouches).group("App").key("t");
	params->addParam("Show Cursor", &mShowCursor)
		.updateFn([&] { mShowCursor ? ci::app::AppBase::get()->showCursor() : ci::app::AppBase::get()->hideCursor(); })
		.key("c")
		.group("App");

	static int boundIndex = 0;
	params
		->addParam("View Bounds", {"None", "Visible", "All"},
				   [&](int i) {
					   boundIndex = i;
					   bluecadet::views::BaseView::sDrawDebugInfo = boundIndex >= 1;
					   bluecadet::views::BaseView::sDrawDebugInfoWhenInvisible = boundIndex >= 2;
				   },
				   [&] { return boundIndex; })
		.key("b")
		.group("App");

	if (mMinimizeParams) {
		params->minimize();
	}

	params->addSeparator();
	params->addButton("Load", [=]() { load(); });
	params->addButton("Save", [=]() { save(); });
	params->addText("Version " + mAppVersion);

	for (const auto & fieldName : mFieldMappingNames) {
		auto it = mFieldMappings.find(fieldName);
		if (it != mFieldMappings.end()) {
			auto & fieldMapping = it->second;
			fieldMapping.attachToParams(params);
		}
	}

	if (mCollapseParams) {
		params->setOptions("App", "opened=false");
	}

	return params;
}

}  // namespace core
}  // namespace bluecadet
