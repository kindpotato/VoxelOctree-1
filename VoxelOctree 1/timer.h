#pragma once
#include <chrono>

//This thing doesn't work for shit
class Timer {
	std::chrono::duration<double> desiredPeriod;
	unsigned int interval;
	std::chrono::steady_clock::time_point begin, lastTime;
public:
	unsigned int step;
	Timer(double framerate);
	void tick();
	void setDisplayInterval(unsigned int i);
};