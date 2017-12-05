#pragma once

#include "cinder/app/App.h"
#include "cinder/Json.h"
#include "cinder/Log.h"
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

	//! Set up the settings manager with the path to an optional settings json. Should be called from prepareSettings() and before setup().
	//! 
	//! callback can be used to override arguments from the json file.
	//! 
	virtual void setup(ci::app::App::Settings * appSettings, ci::fs::path jsonPath = "", std::function<void(SettingsManager * manager)> callback = nullptr);


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
	bool			mConsole = true;					//! Use external console window
	std::string		mAppVersion = "";					//! Displays app version in the params

	// Graphics
	int				mFps = 60;
	bool			mFullscreen = true;
	bool			mBorderless = false;
	bool			mVerticalSync = true;

	// Display
	ci::ivec2		mDisplaySize = ci::ivec2(-1, -1);	//! The size of one display. Defaults to getWindowSize()
	int				mDisplayColumns = 1;				//! The number of display columns in a display matrix. Use by ScreenLayout class.
	int				mDisplayRows = 1;					//! The number of display rows in a display matrix. Use by ScreenLayout class.
	ci::ivec2		mBezelDims = ci::ivec2(0, 0);		//! The amount of bezel correction to add in both X and Y

	// Window
	ci::ivec2		mWindowSize;						//! The window size on launch
	ci::vec2		mCameraOffset;						//! The offset of the camera on launch
	ci::ColorA		mClearColor = ci::ColorA::black();	//! The color used when clearing the screen before draw(). Defaults to opaque black.

	// Touches
	bool			mNativeTouchEnabled = false;		//! Native touch coming from the OS
	bool			mTuioTouchEnabled = true;			//! TUIO touch events; It's recommended to disable native touch when TUIO is enabled to prevent duplicate events.
	bool			mMouseEnabled = true;				//! Treat mouse events as touch events.

	 // Debugging
	bool			mDebugEnabled = true;				//! Enable/disable all of the below debug features (except for hotkeys)
	bool			mShowTouches = false;				//! Visualizes all current touch data
	bool			mShowScreenLayout = false;			//! Visualizes the current screen layout and bezels
	bool			mShowCursor = false;				//! Show or hide the mouse cursor (toggle with C)
	bool			mShowMinimap = false;				//! Show or hide the mini-map (toggle with M)
	bool			mShowStats = false;					//! Show or hide the frame-rate graph/plot (toggle with S)
	bool			mMinimizeParams = false;			//! Minimizes the params window (toggle with F1)
	bool			mCollapseParams = false;			//! Collapses all the default parameter groups like "App"
	bool			mZoomToggleHotkeyEnabled = true;	//! When true, will bind 0 to toggle zoom to 100%/fit
	bool			mDisplayIdHotkeysEnabled = false;	//! When true, will bind 1-9 to zoom directly to displays 1-9

protected:
	static SettingsManagerRef sInstance;


	//! Parses json settings and applies them.
	virtual void parseJson(ci::JsonTree & json);

	//! Parses command line arguments, which can override json settings
	virtual void parseCommandLineArgs(const std::vector<std::string> & args);
	
	//! Applies parsed settings to ci::app::App::Settings
	virtual void applyToAppSettings(ci::app::App::Settings * settings);

	//! Set fields within the settings manager class if the setting is defined in the json
	template <typename T> void setFieldFromJsonIfExists(T * target, const std::string & jsonFieldName); // Implemented at end of this file

	//! Helpers to get string from primitive types and strings since we can't call to_string on strings
	template <typename T> inline std::string toString(T * target) { return std::to_string(*target); }


	//! Key-based callbacks that are called when a command line argument with that key is passed in
	std::map<std::string, std::vector<CommandLineArgParserFn>> mCommandLineArgsHandlers;


	//! Base settings json
	ci::JsonTree mSettingsJson;

	
};


//==================================================
// Template and inline implementations
// 

template <typename T>
T SettingsManager::getField(const std::string & field) {
	try {
		if (!hasField(field)) {
			CI_LOG_W("Field '" << field << "' could not be found");
			return T();
		}
		return mSettingsJson.getValueForKey<T>(field);
	} catch (cinder::Exception e) {
		CI_LOG_EXCEPTION("Could not find '" << field << "' in json file", e);
		return T();
	}
};

template <typename T>
void SettingsManager::setFieldFromJsonIfExists(T * target, const std::string & jsonFieldName) {
	try {
		if (!hasField(jsonFieldName)) {
			CI_LOG_W("Could not find settings value for field name '" << jsonFieldName << "' in json file");
			return;
		}
		*target = mSettingsJson.getValueForKey<T>(jsonFieldName);
	} catch (cinder::Exception e) {
		CI_LOG_EXCEPTION("Could not set '" << jsonFieldName << "' in json file", e);
	}
}

template <>
std::string inline SettingsManager::toString<std::string>(std::string * target) {
	return *target;
}


} // namespace utils
} // namespace bluecadet
