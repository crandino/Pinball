#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "Audio.h"
#include "Scene.h"
#include "Physics.h"
#include "p2Log.h"
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

	// Textures
	numbers = app->tex->loadTexture("textures/score_numbers.png");
	numbers_hi = app->tex->loadTexture("textures/hi_score_numbers.png");
	intro_tex = app->tex->loadTexture("textures/intro_splash.png");
	gameover_tex = app->tex->loadTexture("textures/gameover_splash.png");
	pinball_ball_tex = app->tex->loadTexture("textures/pinball_ball.png");
	life_tex = app->tex->loadTexture("textures/life.png");

	// Sounds
	flipper_sound = app->audio->loadFx("sounds/fx/flipper_sound.ogg");

	// We set variables
	lifes = 3;
	score = hi_score = 0;
	playing = gameover = false;

	// We set buttons
	play_button = { 242, 369, 155, 69 };
	replay_button = { 210, 343, 208, 48 };

	// We set the corresponding frames for score numbers
	SDL_Rect r;
	for (uint i = 0; i < 10; i++)
	{
		r = { i * 8, 0, 8, 12 };
		frames.pushBack(r);
	}

	ball = app->physics->createBall(313, 450, 6, pinball_ball_tex);
			
	return ret;
}

bool isInside(iPoint pos, SDL_Rect &r)
{
	if (pos.x >= r.x &&
		pos.x <= r.x + r.w &&
		pos.y >= r.y &&
		pos.y <= r.y + r.h)
		return true;
	return false;
}

bool Player::update(float dt)
{
	blitScore();
	blitLifes();

	iPoint pos(0, 0);
	//Ball
	ball->getPosition(pos.x, pos.y);
	app->render->blit(ball->texture, pos.x, pos.y, NULL, 1.0f, ball->getRotation());

	if (!playing && !gameover)
	{
		app->render->blit(intro_tex, 0, 0);
		if (app->input->getMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			app->input->getMousePosition(pos.x, pos.y);
			if (isInside(pos, play_button))
				playing = true;
		}	
	}

	if (!playing && gameover)
	{
		app->render->blit(gameover_tex, 0, 0);
		if (app->input->getMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			app->input->getMousePosition(pos.x, pos.y);
			if (isInside(pos, replay_button))
			{
				playing = true;
				gameover = false;
			}
		}
	}

	if (lifes == 0)
	{
		gameover = true;
		playing = false;
		lifes = 3;
		hi_score = score;
		score = 0;
	}
	
	if (playing)
	{
		static float push_force = 0.0f;

		if (app->input->getKey(SDL_SCANCODE_DOWN) == KEY_DOWN || app->input->getKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			push_force += 175.0f;
			app->scene->propulsor->push(0, push_force);
		}
		else if (app->input->getKey(SDL_SCANCODE_DOWN) == KEY_UP)
		{
			push_force = -500.0f;
			app->scene->propulsor->push(0, push_force);
		}
		else
			push_force = 0.0f;

		if (METERS_TO_PIXELS(ball->body->GetPosition().y) > 532)
		{
			ball->setLinearSpeed(0, 0);
			ball->setPosition(313, 450);
			lifes--;
		}

		if (app->input->getKey(SDL_SCANCODE_Z) == KEY_DOWN)
		{
			app->physics->activateLeftFlippers();
			app->audio->playFx(flipper_sound);
		}			

		if (app->input->getKey(SDL_SCANCODE_M) == KEY_DOWN)
		{
			app->physics->activateRightFlippers();
			app->audio->playFx(flipper_sound);
		}			

		if (app->input->getKey(SDL_SCANCODE_Z) == KEY_UP)
			app->physics->deactivateLeftFlippers();

		if (app->input->getKey(SDL_SCANCODE_M) == KEY_UP)
			app->physics->deactivateRightFlippers();
	}	

	return true;
}

void Player::blitScore()
{
	uint divisor = 10000000;
	uint score_remain = score;
	uint score_remain_hi = hi_score;
	uint num_to_render = 0;
	uint num_to_render_hi = 0;

	for (uint i = 0; i < 8; i++)
	{
		num_to_render = (int)(score_remain / divisor);
		num_to_render_hi = (int)(score_remain_hi / divisor);
		app->render->blit(numbers, 32 + (i * 8), 490, &frames[num_to_render]);
		app->render->blit(numbers_hi, 524 + (i * 8), 490, &frames[num_to_render_hi]);
		score_remain = score_remain - (divisor * num_to_render);
		score_remain_hi = score_remain_hi - (divisor * num_to_render_hi);
		divisor /= 10; 
	}
}

void Player::blitLifes()
{
	for (uint i = 0; i < lifes; i++)
	{
		app->render->blit(life_tex, 331 + (i * 10), 494);
	}
}