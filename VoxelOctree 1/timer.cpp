#pragma once
#include "timer.h"
#include <thread>
#include <iostream>

Timer::Timer(double framerate) {
	desiredPeriod = std::chrono::duration <double>(1. / (framerate));
	begin = std::chrono::steady_clock::now();
	lastTime = std::chrono::steady_clock::now();
	step = 1;
	interval = 200;
}

void Timer::setDisplayInterval(unsigned int i) {
	interval = i;
}

void Timer::tick() {
	auto period = (std::chrono::steady_clock::now() - begin);
	if (period < desiredPeriod)
		std::this_thread::sleep_for(desiredPeriod - period);



	++step;
	if (step % interval == 0) {
		auto now = std::chrono::steady_clock::now();
		double micros = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count();
		std::cout << "FPS: " << (double)interval/(micros/1E6) << std::endl;
		lastTime = now;
	}
	begin = std::chrono::high_resolution_clock::now();
}