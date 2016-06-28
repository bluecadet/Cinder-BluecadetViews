#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include <algorithm>

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class StatsView> StatsViewRef;

class StatsView : public BaseView {

public:
	typedef std::map<std::string, std::function<std::string()>> FnStats;
	typedef std::map<std::string, bool*> BoolStats;
	typedef std::map<std::string, int*> IntStats;
	typedef std::map<std::string, float*> FloatStats;
	typedef std::map<std::string, double*> DoubleStats;

	StatsView();
	~StatsView();

	template <typename T>
	void addStat(std::string name, T stat) {
		auto it = std::find(mAllStats.begin(), mAllStats.end(), name);
		if (it != mAllStats.end()) {
			cout << "StatsView: Replacing existing stat with name " << name << endl;
			removeStat<T>(name);
		}

		auto& stats = getStats<T>();
		stats[name] = stat;
		mAllStats.push_back(name);
	}

	void addStatFn(std::string name, std::function<std::string()> stat) {
		addStat<std::function<std::string()>>(name, stat);
	}

	template <typename T>
	void removeStat(std::string name) {
		auto allStatsIt = std::find(mAllStats.begin(), mAllStats.end(), name);
		if (allStatsIt == mAllStats.end()) {
			cout << "StatsView: Can't find stat with name " << name << " in all stats" << endl;
			return;
		}
		mAllStats.erase(allStatsIt);

		auto& stats = getStats<T>();
		auto statsIt = stats.find(name);
		if (statsIt == stats.end()) {
			cout << "StatsView: Can't find stat with name " << name << " in type stats" << endl;
			return;
		}
		stats.erase(statsIt);
	}

protected:
	void drawContent() override;
	std::string getValueString(const std::string& name) const;

	ci::Font mFont;
	ci::ColorA mFontColor;

	std::vector<std::string> mAllStats;
	FnStats mFnStats;
	BoolStats mBoolStats;
	IntStats mIntStats;
	FloatStats mFloatStats;
	DoubleStats mDoubleStats;

	template <typename T> std::map<std::string, T>& getStats() { return std::map<std::string, T>(); };
	template <> FnStats& getStats<std::function<std::string()>>() { return mFnStats; }
	template <> BoolStats& getStats<bool*>() { return mBoolStats; }
	template <> IntStats& getStats<int*>() { return mIntStats; }
	template <> FloatStats& getStats<float*>() { return mFloatStats; }
	template <> DoubleStats& getStats<double*>() { return mDoubleStats; }
};

}
}