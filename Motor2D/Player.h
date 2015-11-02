#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Module.h"
#include "DynArray.h"
#include "SDL\include\SDL.h"

struct SDL_Texture;

class Player : public Module
{
public:

	Player();
	~Player();

	bool start();
	bool update(float dt);

	uint					score;

private:

	uint					lifes;
	uint					hi_score;
	DynArray<SDL_Rect>		frames;

	bool					playing;
	bool					gameover;

	PhysBody*				ball;

	SDL_Texture*			numbers;
	SDL_Texture*			numbers_hi;
	SDL_Texture*			intro_tex;
	SDL_Texture*			gameover_tex;
	SDL_Texture*			pinball_ball_tex;
	SDL_Texture*			life_tex;
	
	SDL_Rect				play_button;
	SDL_Rect				replay_button;

	void blitScore();
	void blitLifes();
};



#endif !__PLAYER_H__