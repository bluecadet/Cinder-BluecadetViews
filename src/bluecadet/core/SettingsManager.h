#pragma once

#include "cinder/Json.h"
#include "cinder/app/App.h"
#include "cinder/params/Params.h"

namespace bluecadet {
namespace core {

typedef std::shared_ptr<class SettingsManager> SettingsManagerRef;

class SettingsManager {
public:

	typedef std::function<void(const std::string & value)> CommandLineArgParserFn;

	SettingsManager();
	virtual ~SettingsManager();

	//! Overrides the shared instance. Use this method if you'd like to use a sub-class of SettingsManager.
	static void setInstance(SettingsManagerRef instance) {
		sInstance = instance;
	}

	//! Singleton; Instance can be changed by calling setInstance() (e.g. to use subclasses)
	static SettingsManagerRef getInstance() {
		if (!sInstance) {
			sInstance = SettingsManagerRef(new SettingsManager());
		}
		return sInstance;
	}



	//==================================================
	// Setup
	// 

	//! Set up the settings manager with the path to the main json and the applications settings. Should be called in prepareSettings().
	//! 
	//! jsonPath will default to getAssetPath("appSettings.json") if left empty.
	//! overrideCallback can be used to override arguments from the json file.
	//! 
	virtual void setup(ci::app::App::Settings * appSettings, ci::fs::path jsonPath = "", std::function<void(SettingsManager * manager)> overrideCallback = nullptr);


	//! Adds a callback to parse a command line argument by key.
	//! Keys are converted to lowercase and values are always passed as strings.
	//! Multiple parsers for the same key can be added and they are called in the order they were added in.
	virtual void addCommandLineParser(const std::string & key, CommandLineArgParserFn callback);


	//! Checks if the field exists in the loaded settings json.
	inline bool hasField(const std::string& field) { return mSettingsJson.hasChild(field); };


	//! Get the value of field from within the settings json. Will return an empty default value of type T if the field was not found.
	template <typename T> T getField(const std::string& field); // Implemented at end of this file


	//! Returns the app params, creates new params if necessary. 
	ci::params::InterfaceGlRef getParams();



	//==================================================
	// Global Properties
	// 

	// General
	bool			mConsoleWindowEnabled;
	int				mFps;
	std::string		mAppVersion;

	// Graphics
	bool			mFullscreen = true;
	bool			mBorderless = true;
	bool			mVerticalSync;

	// Touches
	bool			mNativeTouchEnabled = false;
	bool			mTuioTouchEnabled = true;
	bool			mMouseEnabled = true;

	 // Debugging
	bool			mDebugMode = false;
	bool			mDrawTouches = false;
	bool			mDrawScreenLayout = false;
	bool			mShowMouse = false;
	bool			mDrawMinimap = false;
	bool			mDrawStats = false;
	bool			mMinimizeParams = false;			//! Minimizes the params window
	bool			mCollapseParams = false;			//! Collapses all the default parameter groups like "App"
	bool			mZoomToggleHotkeyEnabled = true;	//! When true, will bind 0 to toggle zoom to 100%/fit
	bool			mDisplayIdHotkeysEnabled = false;	//! When true, will bind 1-9 to zoom directly to displays 1-9

	// CLI/runtime only args
	ci::ivec2		mWindowSize; //! The window size on launch
	ci::vec2		mCameraOffset; //! The offset of the camera on launch
	ci::ColorA		mClearColor; //! The color used when clearing the screen before draw(). Defaults to opaque black.

	// Analytics
	std::string		mAnalyticsAppName;
	std::string		mAnalyticsTrackingId;
	std::string		mAnalyticsClientId;

protected:
	static SettingsManagerRef sInstance;


	//! Parses json settings and applies them.
	virtual void parseJson(ci::JsonTree & json);

	//! Parses command line arguments, which can override json settings
	virtual void parseCommandLineArgs(const std::vector<std::string>& args);
	
	//! Applies parsed settings to ci::app::App::Settings
	virtual void applyToAppSettings(ci::app::App::Settings * settings);


	//! Set fields within the settings manager class if the setting is defined in the json
	template <typename T> void setFieldFromJsonIfExists(T* target, const std::string& jsonFieldName); // Implemented at end of this file

	//! Helpers to get string from primitive types and strings since we can't call to_string on strings
	template <typename T> inline std::string toString(T* target) { return std::to_string(*target); }


	//! Key-based callbacks that are called when a command line argument with that key is passed in
	std::map<std::string, std::vector<CommandLineArgParserFn>> mCommandLineArgsHandlers;


	//! Base appSettings json
	ci::JsonTree mSettingsJson;

	
};



   //==================================================
   // Template and inline implementations
   // 

template <typename T>
T SettingsManager::getField(const std::string& field) {
	try {
		if (!hasField(field)) {
			ci::app::console() << "SettingsManager: Could not find settings value for field name '" << field << "' in json file" << std::endl;
			return T();
		}
		return mSettingsJson.getValueForKey<T>(field);
	} catch (cinder::Exception e) {
		//		ci::app::console() << "SettingsManager: Could not find '" << field << "' in json file: " << e.what() << std::endl;
		return T();
	}
};

template <typename T>
void SettingsManager::setFieldFromJsonIfExists(T* target, const std::string& jsonFieldName) {
	try {
		if (!hasField(jsonFieldName)) {
			//            cinder::app::console() << "SettingsManager: Could not find settings value for field name '" << jsonFieldName << "' in json file" << std::endl;
			return;
		}
		*target = mSettingsJson.getValueForKey<T>(jsonFieldName);
		//		ci::app::console() << "SettingsManager: Set '" << jsonFieldName << "' to '" << SettingsManager::toString<T>(target) << "' from json file" << std::endl;
	} catch (cinder::Exception e) {
		//		ci::app::console() << "SettingsManager: Could not set '" << jsonFieldName << "' from json file: " << e.what() << std::endl;
	}
}

template <>
std::string inline SettingsManager::toString<std::string>(std::string* target) {
	return *target;
}




} // namespace utils
} // namespace bluecadet