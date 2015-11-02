#ifndef __TIMER_H__
#define __TIMER_H__

#include "Motor2D\SDL\include\SDL_timer.h"

class Timer
{
public:

	// Constructor
	Timer()
	{
		Start();
	}

	void Start()
	{
		started_at = SDL_GetTicks();
	}

	Uint32 Read() const
	{
		return SDL_GetTicks() - started_at;
	}

	float ReadSec() const
	{
		return (SDL_GetTicks() - started_at) * 1000.f;
	}

private:

	Uint32	started_at;
};

#endif //__TIMER_H__