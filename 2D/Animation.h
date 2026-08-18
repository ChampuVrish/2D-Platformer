#pragma once

#include "Timer.h"

class Animation
{
	Timer timer;
	int frameCount;
public:
	Animation() : timer(0), frameCount(0) {}
	Animation(int framecount, float length) : timer(length), frameCount(framecount) {}

	float getlength() const { return timer.getLength(); }
	int currentframe() const
	{
		return static_cast<int>(timer.getTime() / timer.getLength() * frameCount);
	}

	void step(float deltaTime)
	{
		timer.step(deltaTime);
	}

	bool isDone() const { return timer.isTimeout(); }
};