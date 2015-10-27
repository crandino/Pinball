#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "SDL\include\SDL.h"

Player::Player()
{
	name.create("player");
}

Player::~Player()
{ }

bool Player::start()
{
	bool ret = true;

	lifes = 3;
	score = 2658410;

	SDL_Rect r;
	for (uint i = 0; i < 10; i++)
	{
		r = { (0, i * 8, 8, 12) };
		frames.pushBack(r);
	}
		
	numbers = app->tex->loadTexture("textures/score_numbers.png");

	return ret;
}

bool Player::update(float dt)
{
	blitScore();
	return true;
}

void Player::blitScore()
{
	uint divisor = 10000000;
	uint score_copy = score;
	uint num_to_render = 0;

	for (uint i = 0; i < 8; i++)
	{
		num_to_render = (int)(score_copy / divisor);
		app->render->blit(numbers, 32 + (i * 8), 490, &frames[num_to_render]);
		divisor /= 10; score_copy /= 10;
	}
}