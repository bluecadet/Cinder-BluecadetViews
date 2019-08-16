#pragma once

#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"
#include "cinder/params/Params.h"

#include "ValueMapping.h"
#include "bluecadet/text/Text.h"

#include <set>

namespace bluecadet {
namespace core {

typedef std::shared_ptr<class SettingsManager> SettingsManagerRef;

class SettingsManager {
public:
	typedef std::function<void(const std::string & value)> CommandArgParserFn;

	typedef ci::signals::Signal<void()> SettingsLoadedSignal;

	SettingsManager();
	virtual ~SettingsManager();

	// Overrides the shared instance. Use this method if you'd like to use a sub-class of SettingsManager.
	static void setInstance(SettingsManagerRef instance) { sInstance = instance; }

	// Singleton; Instance can be changed by calling setInstance() (e.g. to use subclasses)
	static SettingsManagerRef getInstance() {
		if (!sInstance) {
			sInstance = std::make_shared<SettingsManager>();
		}
		return sInstance;
	}

	static ci::fs::path getDefaulJsonPath() {
		const auto filename = "settings.json";
		ci::fs::path path;
		if (ci::app::getAssetDirectories().empty()) {
			path = ci::app::getAppPath();
		} else {
			path = ci::app::getAssetDirectories().front();
		}
		path.append(ci::fs::path(filename));
		return path;
	}

	//==================================================
	// Setup

	// Set up the settings manager with the path to an optional settings json. Should be called from prepareSettings()
	// and before setup(). Callback can be used to override arguments from the json file.
	virtual void setup(ci::app::App::Settings * appSettings, ci::fs::path jsonPath = getDefaulJsonPath(),
					   bool autoCreateJson = true, std::function<void(SettingsManager * manager)> callback = nullptr);

	SettingsLoadedSignal & getSignalSettingsLoaded() { return mSignalSettingsLoaded; };

	// Loads settings from jsonPath (or mJsonPath if previously defined via setup()).
	// If autoCreateJson is true, then a new file will be created at jsonPath or mJsonPath.
	virtual void load(ci::fs::path jsonPath = "", bool autoCreateJson = false);

	// Saves all mapped settings to jsonPath (or mJsonPath if previously defined via setup()).
	virtual void save(ci::fs::path jsonPath = "");


	// Parses all mapped settings and applies them. This only changes internal values,
	// but doesn't apply any settings like the current app's window size.
	virtual void deserialize(ci::JsonTree & json);

	// Serializes all mapped settings into a JsonTree.
	virtual ci::JsonTree serialize();


	// Adds a callback to parse a command line argument by key. Keys are converted to lowercase and values are always
	// passed as strings. Multiple parsers for the same key can be added and they are called in the order they were
	// added in.
	virtual void addCommandArg(const std::string & key, CommandArgParserFn callback);

	// Shortcut to add multiple command line args with different keys but the same parser.
	void addCommandArgs(const std::set<std::string> & keys, CommandArgParserFn callback);


	// Checks if the field exists in the loaded settings json.
	inline bool hasJsonValue(const std::string & field) { return mJson.hasChild(field); };

	// Get the value of field from within the settings json. Will return an empty default value of
	// type T if the field was not found.
	template <typename T> T getJsonValue(const std::string & field);


	// Returns the app params, creates new params if necessary.
	inline ci::params::InterfaceGlRef getParams() {
		if (!mParams) {
			mParams = createParams();
		}
		return mParams;
	};

	//==================================================
	// Default Properties

	// General
	bool mConsole = true;				// Use external console window
	std::string mAppVersion = "1.0.0";  // Displays app version in the params

	// Graphics
	float mFps = 60.0f;
	bool mFullscreen = true;
	bool mBorderless = false;
	bool mVerticalSync = true;

	// Display
	int mDisplayIndex = 0;  // The index of the display to launch this app on. Reverts to main display if out of bounds.
	ci::ivec2 mDisplaySize = ci::ivec2(-1);  // The size of one display. Defaults to getWindowSize()
	ci::ivec2 mBezelDims = ci::ivec2(0);	 // The amount of bezel correction to add in both X and Y
	int mDisplayColumns = 1;				 // The number of display columns in a display matrix. Used by ScreenLayout.
	int mDisplayRows = 1;					 // The number of display rows in a display matrix. Used by ScreenLayout.

	// Window
	ci::ivec2 mWindowSize = ci::ivec2(INT_MIN);		// The window size on launch
	ci::ivec2 mWindowPos = ci::ivec2(INT_MIN);		// The window position on launch
	ci::ColorA mClearColor = ci::ColorA::black();	// The color used when clearing the screen before draw()

	// Camera
	ci::vec2 mCameraOffset = ci::ivec2(0);  // The offset of the camera on launch
	float mCameraZoom = 1.0f;				// The zoom of the camera at launch

	// Touches
	bool mMouseEnabled = true;						 // Treat mouse events as touch events.
	bool mNativeTouchEnabled = true;				 // Native touch coming from the OS
	bool mSupportMultipleNativeTouchScreens = true;  // Native touch with multi-touchscreen support
	bool mTuioTouchEnabled = false;					 // TUIO touch events; It's recommended to disable native touch
													 // when TUIO is enabled to prevent duplicate events.

	// Debugging
	bool mDebugEnabled = true;				   // Enable/disable all of the below debug features (except for
											   // hotkeys)
	bool mShowTouches = false;				   // Visualizes all current touch data
	bool mShowScreenLayout = false;			   // Visualizes the current screen layout and bezels
	bool mShowCursor = true;				   // Show or hide the mouse cursor (toggle with C)
	bool mShowMinimap = true;				   // Show or hide the mini-map (toggle with M)
	bool mShowStats = true;					   // Show or hide the frame-rate graph/plot (toggle with S)
	bool mMinimizeParams = false;			   // Minimizes the params window (toggle with F1)
	bool mCollapseParams = true;			   // Collapses all the default parameter groups like "App"
	bool mZoomToggleHotkeyEnabled = true;	   // When true, will bind 0 to toggle zoom to 100%/fit
	bool mDisplayIdHotkeysEnabled = false;	   // When true, will bind 1-9 to zoom directly to displays 1-9
	bool mTouchSimEnabled = false;			   // When true, will add touch simulator controls to params
	float mSimulatedTouchesPerSecond = 50.0f;  // Used only if mEnableTouchSim is true

protected:
	static SettingsManagerRef sInstance;

	// Registers a field that will be mapped from the JSON to a property. Call this method from a subclass if you intend
	// to add custom JSON fields and map them to class members.
	template <typename T> ValueMapping & mapField(const std::string & fieldName, T * target);

	// Checks if a field exists in the JSON and copies its value to this class in that value if it does.
	void mergeField(const std::string & fieldName, const ci::JsonTree & json);

	// Map JSON field names to class members. Called once at the beginning setup().
	virtual void mapFields();

	// Adds all default command line args and those created by all mapped fields.
	virtual void setupCommandArgs();

	// Parses command line arguments, which can override json settings
	virtual void parseCommandArgs(const std::vector<std::string> & args);

	// Applies parsed settings to ci::app::App::Settings
	virtual void applyToAppSettings(ci::app::App::Settings * settings);

	// Helpers to get string from primitive types and strings since we can't call to_string on strings
	template <typename T> inline std::string toString(T * target) { return std::to_string(*target); }


	// Creates the params object
	ci::params::InterfaceGlRef createParams();

	// Key-based callbacks that are called when a command line argument with that key is passed in
	std::map<std::string, std::vector<CommandArgParserFn>> mCommandArgsHandlers;

	// Base settings json
	ci::JsonTree mJson;
	ci::fs::path mJsonPath;

	// Mappings of JSON fields to variables
	std::map<std::string, ValueMapping> mFieldMappings;

	// Ordered list of field mapping names to preserve params sequence
	std::vector<std::string> mFieldMappingNames;

	// Default params
	ci::params::InterfaceGlRef mParams = nullptr;

	// Signals
	SettingsLoadedSignal mSignalSettingsLoaded;
};

//==================================================
// Template and inline implementations

template <typename T> ValueMapping & SettingsManager::mapField(const std::string & fieldName, T * property) {
	mFieldMappingNames.push_back(fieldName);
	return mFieldMappings.insert({fieldName, ValueMapping(fieldName, property)}).first->second;
}

template <typename T> T SettingsManager::getJsonValue(const std::string & field) {
	try {
		if (!hasJsonValue(field)) {
			CI_LOG_W("Field '" << field << "' could not be found");
			return T();
		}
		return mJson.getValueForKey<T>(field);
	} catch (cinder::Exception e) {
		CI_LOG_EXCEPTION("Could not read '" << field << "' from json file", e);
		return T();
	}
}

template <> std::string inline SettingsManager::toString<std::string>(std::string * target) {
	return *target;
}

}  // namespace core
}  // namespace bluecadet
