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
	mMaxTouchDuration(0),
	mMinDragDistance(0),
	mMaxDragDistance(0),
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

	const auto touchManager = TouchManager::getInstance();
	const float fps = AppBase::get()->getFrameRate();
	const float loopDelay = mTouchesPerSecond <= 0.0f ? 0.0f : max(1.0f / mTouchesPerSecond, 1.0f / fps);
	const int touchesPerFrame = (int)round(mTouchesPerSecond * loopDelay);
	const float touchStartTime = mTimeline->getCurrentTime();

	// generate touches
	for (int i = 0; i < touchesPerFrame; ++i) {
		const float touchDuration = randFloat(mMinTouchDuration, mMaxTouchDuration);
		const float dragDistance = randFloat(mMinDragDistance, mMaxDragDistance);
		const float dragAngle = randFloat(2.0f * (float)M_PI);
		const int numDragFrames = (int)roundf(fps * touchDuration);
		const int touchId = touchCounter++;
		const vec2 beginPos = vec2(randFloat(mBounds.x1, mBounds.x2), randFloat(mBounds.y1, mBounds.y2));
		const vec2 deltaPos = dragDistance * vec2(cosf(dragAngle), sinf(dragAngle));
		const vec2 endPos = beginPos + deltaPos;

		mTimeline->add([=] {
			touchManager->addTouch(touchId, beginPos, TouchType::Simulator, TouchPhase::Began);
		}, touchStartTime);
		for (int j = 0; j < numDragFrames; ++j) {
			const float progress = (float)j / (float)numDragFrames;
			const float delay = progress * touchDuration;
			const vec2 pos = beginPos + progress * deltaPos;
			mTimeline->add([=] {
				touchManager->addTouch(touchId, pos, TouchType::Simulator, TouchPhase::Moved);
			}, touchStartTime + delay);
		}
		mTimeline->add([=] {
			touchManager->addTouch(touchId, endPos, TouchType::Simulator, TouchPhase::Ended);
		}, touchStartTime + touchDuration);

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
