#include "SimulatedTouchDriver.h"

#include "cinder/Rand.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace touch {
namespace drivers {

SimulatedTouchDriver::SimulatedTouchDriver() :
	mBounds(Rectf()),
	mTimeline(nullptr),
	mIsRunning(false),
	mTouchesPerSecond(0),
	mMinTouchDuration(0),
	mMaxTouchDuration(1.0f),
	mMinDragDistance(0),
	mMaxDragDistance(200.0f),
	mTouchCounter(10000),
	mTouchesToSpawn(0),
	mPrevUpdateTime(0)
{
}

SimulatedTouchDriver::~SimulatedTouchDriver() {
	stop();
	if (mConnection.isEnabled()) {
		mConnection.disable();
	}
	if (mTimeline) {
		mTimeline->clear();
	}
}

void SimulatedTouchDriver::setup(ci::Rectf bounds, float touchesPerSecond) {
	mBounds = bounds;
	mTouchesPerSecond = touchesPerSecond;
	mTimeline = Timeline::create();
	mConnection = AppBase::get()->getSignalUpdate().connect(std::bind(&SimulatedTouchDriver::update, this));
}

void SimulatedTouchDriver::update() {

	const double currTime = getElapsedSeconds();
	const double deltaTime = min(currTime - mPrevUpdateTime, 15.0 / (double)getFrameRate());
	mPrevUpdateTime = currTime;

	if (mIsRunning) {
		mTouchesToSpawn += mTouchesPerSecond * deltaTime;
	}

	if (mTimeline) {
		mTimeline->stepTo((float)currTime);
	}

	if (!mIsRunning && mTouchesToSpawn == 0) {
		// stop updates once all timeline items have finished
		mConnection.disable();
		return;
	}

	// generate touches
	while (mTouchesToSpawn >= 1.0f) {

		const int touchId = mTouchCounter++;
		mSimulatedTouches.insert(make_pair(touchId, SimulatedTouch(touchId)));
		SimulatedTouch & touch = mSimulatedTouches[touchId];

		const float duration = randFloat(mMinTouchDuration, mMaxTouchDuration);
		const float distance = randFloat(mMinDragDistance, mMaxDragDistance);

		touch.relPosition = vec2(randFloat(mBounds.x1, mBounds.x2), randFloat(mBounds.y1, mBounds.y2));
		const vec2 endPos = touch.relPosition.value() + randVec2() * distance;

		mTimeline->apply(&touch.relPosition, endPos, duration, easeInOutQuad).startFn([&] {
			TouchManager::get()->addTouch(touch.id, touch.relPosition.value(), TouchType::Simulator, TouchPhase::Began);
		}).updateFn([&] {
			TouchManager::get()->addTouch(touch.id, touch.relPosition.value(), TouchType::Simulator, TouchPhase::Moved);
		}).finishFn([&] {
			TouchManager::get()->addTouch(touch.id, touch.relPosition.value(), TouchType::Simulator, TouchPhase::Ended);
			mSimulatedTouches.erase(touch.id);
		});

		mTouchesToSpawn -= 1.0f;
	}
}

void SimulatedTouchDriver::start() {
	if (mIsRunning) {
		return;
	}

	mIsRunning = true;
	mPrevUpdateTime = getElapsedSeconds();

	if (!mConnection.isEnabled()) {
		mConnection.enable();
	}
}

void SimulatedTouchDriver::stop() {
	mIsRunning = false;
}

bool SimulatedTouchDriver::isRunning() const {
	return mIsRunning;
}

}
}
}
