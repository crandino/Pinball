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
	SDL_Texture*			numbers;
	
	void blitScore();
};



#endif !__PLAYER_H__