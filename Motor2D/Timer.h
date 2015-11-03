#ifndef __TIMER_H__
#define __TIMER_H__

#include "SDL\include\SDL_timer.h"

class Timer
{
public:

	// Constructor
	Timer()
	{
		active = false;
		//Start();
	}

	void Start()
	{
		active = true;
		started_at = SDL_GetTicks();
	}

	Uint32 read() const
	{
		return SDL_GetTicks() - started_at;
	}

	float readSec() const
	{
		return (SDL_GetTicks() - started_at) / 1000.f;
	}

	void stop()
	{
		active = false;
	}

	bool isActive() const
	{
		return active;
	}

private:

	Uint32	started_at;
	bool active;
};

#endif //__TIMER_H__