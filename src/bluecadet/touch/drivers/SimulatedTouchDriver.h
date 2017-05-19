//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//	Developers: Benjamin Bojko
//  Contents: Code for SimulatedTouchDriver driver to be used with the touch manager class 
//----------------------------------------------------------------------------

#pragma once
#include "cinder/app/App.h"
#include "../TouchManager.h"

namespace bluecadet {
namespace touch {
namespace drivers {

class SimulatedTouchDriver {
public:
	SimulatedTouchDriver();
	~SimulatedTouchDriver();

	void setup(ci::Rectf bounds, float touchesPerSecond = 0.0f);
	void start();
	void stop();
	bool isRunning() const;

	float getTouchesPerSecond() const { return mTouchesPerSecond; }
	void setTouchesPerSecond(float value) { mTouchesPerSecond = value; }

	float getMinTouchDuration() const { return mMinTouchDuration; }
	void setMinTouchDuration(const float value) { mMinTouchDuration = value; }

	float getMaxTouchDuration() const { return mMaxTouchDuration; }
	void setMaxTouchDuration(const float value) { mMaxTouchDuration = value; }

	float getMinDragDistance() const { return mMinDragDistance; }
	void setMinDragDistance(const float value) { mMinDragDistance = value; }

	float getMaxDragDistance() const { return mMaxDragDistance; }
	void setMaxDragDistance(const float value) { mMaxDragDistance = value; }

	ci::Rectf getBounds() const { return mBounds; }
	void setBounds(const ci::Rectf value) { mBounds = value; }

private:
	struct SimulatedTouch {
		SimulatedTouch(const SimulatedTouch & touch) : id(touch.id) {}
		SimulatedTouch(const int id) : id(id) {}
		SimulatedTouch() : id(-1) {}
		const int id;
		ci::Anim<ci::vec2> relPosition;
	};

	void update();
	void touchSimulationLoop(); //! Called regularly when connected

	bool mIsRunning;

	ci::Rectf mBounds;
	float mTouchesPerSecond;
	float mMinTouchDuration;
	float mMaxTouchDuration;
	float mMinDragDistance;
	float mMaxDragDistance;

	int touchCounter;
	ci::signals::Connection	mConnection;
	ci::TimelineRef	mTimeline;
	ci::CueRef mSimLoopCue;

	std::map<int, SimulatedTouch> mSimulatedTouches;
};

}
}
}
