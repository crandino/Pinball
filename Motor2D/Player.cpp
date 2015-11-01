#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "SDL\include\SDL.h"

Player::Player()
{
	name.create("player");
	playing = false;
}

Player::~Player()
{ }

bool Player::start()
{
	bool ret = true;

	// We set variables
	lifes = 3;
	score = 0;
	playing = false;
	gameover = false;

	// We set the corresponding frames for score numbers
	SDL_Rect r;
	for (uint i = 0; i < 10; i++)
	{
		r = { i * 8, 0, 8, 12 };
		frames.pushBack(r);
	}
		
	numbers = app->tex->loadTexture("textures/score_numbers.png");

	return ret;
}

bool Player::update(float dt)
{
	blitScore();
	blitHiScore();

	return true;
}

void Player::blitScore()
{
	uint divisor = 10000000;
	uint score_remain = score;
	uint num_to_render = 0;

	for (uint i = 0; i < 8; i++)
	{
		num_to_render = (int)(score_remain / divisor);
		app->render->blit(numbers, 32 + (i * 8), 490, &frames[num_to_render]);
		score_remain = score_remain - (divisor * num_to_render);
		divisor /= 10; 
	}
}

void Player::blitHiScore()
{
	
}