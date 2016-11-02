#pragma once

#include "cinder/Json.h"
#include "cinder/app/App.h"
#include "cinder/params/Params.h"

namespace bluecadet {
namespace core {

typedef std::shared_ptr<class SettingsManager> SettingsManagerRef;

class SettingsManager {
public:

	typedef std::function<void(const std::string& value)> CommandLineArgParserFn;

	SettingsManager();
	~SettingsManager();

	//! Singleton
	static SettingsManagerRef getInstance() {
		static SettingsManagerRef instance = nullptr;
		if (!instance) instance = SettingsManagerRef(new SettingsManager());
		return instance;
	}



	//==================================================
	// Setup
	// 

	//! Adds a callback to parse a command line argument by key.
	//! Keys are converted to lowercase and values are always passed as strings.
	//! Multiple parsers for the same key can be added and they are called in the order they were added in.
	void addCommandLineParser(const std::string& key, CommandLineArgParserFn callback);


	//! Set up the settings manager with the path to the main json and the applications settings.
	//! Call this method in prepareSettings() before your main app's setup() method.
	//! Command line arguments that are automatically parsed are:
	//!		debug=[true/false]
	//!		size = w,h
	//!		fullscreen = [true/false]
	//!		borderless = [true/false]
	//!		vsync = [true/false]
	//!		console = [true/false]
	//!		cursor = [true/false] or mouse = [true/false]
	void setup(const ci::fs::path& jsonPath, ci::app::App::Settings* appSettings);


	//! Checks if the field exists in the loaded settings json.
	inline bool hasField(const std::string& field) { return mAppSettingsDoc.hasChild(field); };


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
	bool			mVerticalSync;
	ci::ColorA		mClearColor; //! The color used when clearing the screen before draw(). Defaults to opaque black.

	// Debugging
	bool			mDebugMode;
	bool			mDrawTouches;
	bool			mDrawScreenLayout;
	bool			mFullscreen;
	bool			mBorderless;
	ci::ivec2		mWindowSize;
	bool			mShowMouse;
	bool			mDrawMinimap;
	bool			mDrawStats;

	// Analytics
	std::string		mAnalyticsAppName;
	std::string		mAnalyticsTrackingId;
	std::string		mAnalyticsClientId;

protected:
	//! Set fields within the settings manager class if the setting is defined in the json
	template <typename T> void setFieldFromJsonIfExists(T* target, const std::string& jsonFieldName); // Implemented at end of this file

	void parseCommandLineArgs(const std::vector<std::string>& args);
    
    //! Helpers to get string from primitive types and strings since we can't call to_string on strings
    template <typename T> inline std::string toString(T* target) { return std::to_string(*target); }
    
	//! Key-based callbacks that are called when a command line argument with that key is passed in
	std::map<std::string, std::vector<CommandLineArgParserFn>> mCommandLineArgsHandlers;

	//! Base appSettings json
	ci::JsonTree mAppSettingsDoc;

}; // SettingsManager



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
		return mAppSettingsDoc.getValueForKey<T>(field);
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
		*target = mAppSettingsDoc.getValueForKey<T>(jsonFieldName);
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
