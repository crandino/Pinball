#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Module.h"
#include "DynArray.h"

struct SDL_Rect;
struct SDL_Texture;

class Player : public Module
{
public:

	Player();
	~Player();

	bool start();
	bool update(float dt);

private:

	uint					lifes;
	uint					score;
	DynArray<SDL_Rect>		frames;

	bool					playing;
	bool					gameover;

	SDL_Texture*			numbers;
	SDL_Texture*			intro_tex;
	SDL_Texture*			gameover_tex;
	
	
	void blitScore();
	void blitHiScore();
};



#endif !__PLAYER_H__