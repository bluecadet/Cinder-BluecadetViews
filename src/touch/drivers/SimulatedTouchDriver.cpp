#include "SimulatedTouchDriver.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace touch {
namespace drivers {

SimulatedTouchDriver::SimulatedTouchDriver() :
	mBounds(Rectf()),
	mTimeline(nullptr),
	mSimLoopCue(nullptr),
	mIsRunning(false),
	mTouchesPerSecond(0),
	mMinTouchDuration(0),
	mMaxTouchDuration(1.0f),
	mMinDragDistance(0),
	mMaxDragDistance(200.0f),
	touchCounter(10000)
{
}

SimulatedTouchDriver::~SimulatedTouchDriver() {
}

void SimulatedTouchDriver::setup(ci::Rectf bounds, float touchesPerSecond) {
	mBounds = bounds;
	mTouchesPerSecond = touchesPerSecond;
	mTimeline = Timeline::create();
	mConnection = AppBase::get()->getSignalUpdate().connect(std::bind(&SimulatedTouchDriver::update, this));
}

void SimulatedTouchDriver::update() {
	if (mTimeline) {
		mTimeline->stepTo((float)getElapsedSeconds());
	}

	/*if (!mIsRunning && mTimeline && !mTimeline->empty()) {
		cout << "SimulatedTouchDriver: Waiting for " << to_string(mTimeline->getNumItems()) << " timeline items to finish..." << endl;
	}*/

	if (!mIsRunning && (!mTimeline || mTimeline->empty())) {
		// stop updates once all timeline items have finished
		mConnection.disable();
	}
}

void SimulatedTouchDriver::touchSimulationLoop() {
	if (mSimLoopCue) {
		mTimeline->remove(mSimLoopCue);
		mSimLoopCue = nullptr;
	}
    
	const float loopDelay = 1.0f;
	const float touchStartTime = mTimeline->getCurrentTime();

	// generate touches
	for (int i = 0; i < mTouchesPerSecond; ++i) {

		const int touchId = touchCounter++;
		mSimulatedTouches.insert(make_pair(touchId, SimulatedTouch(touchId)));
		SimulatedTouch & touch = mSimulatedTouches[touchId];

		const float duration = randFloat(mMinTouchDuration, mMaxTouchDuration);
		const float distance = randFloat(mMinDragDistance, mMaxDragDistance);

		touch.relPosition = vec2(randFloat(mBounds.x1, mBounds.x2), randFloat(mBounds.y1, mBounds.y2));
		const vec2 endPos = touch.relPosition.value() + randVec2() * distance;

		mTimeline->apply(&touch.relPosition, endPos, duration, easeInOutQuad).startFn([&] {
			TouchManager::getInstance()->addTouch(touch.id, touch.relPosition.value(), TouchType::Simulator, TouchPhase::Began);
		}).updateFn([&] {
			TouchManager::getInstance()->addTouch(touch.id, touch.relPosition.value(), TouchType::Simulator, TouchPhase::Moved);
		}).finishFn([&] {
			TouchManager::getInstance()->addTouch(touch.id, touch.relPosition.value(), TouchType::Simulator, TouchPhase::Ended);
			auto it = mSimulatedTouches.find(touch.id);
			if (it != mSimulatedTouches.end()) {
				mSimulatedTouches.erase(it);
			}
		}).delay(loopDelay * (float)i / (float)mTouchesPerSecond);
	}

	// recurse
	mSimLoopCue = mTimeline->add(std::bind(&SimulatedTouchDriver::touchSimulationLoop, this), touchStartTime + loopDelay);
}

void SimulatedTouchDriver::start() {
	if (mIsRunning) {
		return;
	}

	mIsRunning = true;

	if (!mConnection.isEnabled()) {
		mConnection.enable();
	}

	if (!mSimLoopCue) {
		touchSimulationLoop();
	}
}

void SimulatedTouchDriver::stop() {
	mIsRunning = false;

	if (mTimeline && mSimLoopCue) {
		mTimeline->remove(mSimLoopCue);
		mSimLoopCue = nullptr;
	}
}

bool SimulatedTouchDriver::isRunning() const {
	return mIsRunning;
}

}
}
}
