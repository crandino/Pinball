#include "p2Defs.h"
#include "p2Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "FileSystem.h"
#include "Physics.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"

#include "Box2D\Box2D\Box2D.h"


Scene::Scene() : Module()
{
	name.create("scene");
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::awake(pugi::xml_node &node)
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::start()
{
	app->audio->playMusic("sounds/music/pinball_theme.ogg");
	//app->audio->loadFx("sounds/fx/bonus.wav");

	circle = app->tex->loadTexture("textures/wheel.png");
	box = app->tex->loadTexture("textures/crate.png");
	rick = app->tex->loadTexture("textures/rick_head.png");
	pinball_ball = app->tex->loadTexture("textures/pinball_ball.png");
	pinball_level = app->tex->loadTexture("textures/pinball_level.png");

	int triangle1[16] = {
		122, 393,
		158, 452,
		156, 456,
		152, 459,
		117, 443,
		114, 440,
		114, 398,
		116, 394
	};
	
	int triangle2[16] = {
		237, 451,
		271, 395,
		276, 394,
		280, 398,
		280, 442,
		278, 445,
		245, 460,
		237, 457
	};

	int triangle3[16] = {
		352, 393,
		388, 452,
		386, 457,
		380, 459,
		347, 443,
		345, 440,
		345, 399,
		347, 394
	};

	int triangle4[16] = {
		478, 458,
		510, 443,
		514, 439,
		514, 396,
		509, 393,
		504, 395,
		471, 451,
		472, 456
	};

	int bottom_part[30] = {
		231, 529,
		231, 493,
		249, 484,
		250, 472,
		299, 450,
		307, 442,
		308, 433,
		312, 430,
		316, 433,
		318, 442,
		324, 449,
		374, 472,
		375, 484,
		393, 493,
		393, 528
	};

	walls.add(app->physics->createWall(0, 0, triangle1, 16));
	walls.add(app->physics->createWall(0, 0, triangle2, 16));
	walls.add(app->physics->createWall(0, 0, triangle3, 16));
	walls.add(app->physics->createWall(0, 0, triangle4, 16));
	walls.add(app->physics->createWall(0, 0, bottom_part, 30));

	return true;
}

// Called each loop iteration
bool Scene::preUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::update(float dt)
{
	// Load & Save keys
	if (app->input->getKey(SDL_SCANCODE_L) == KEY_DOWN)
		app->loadGame("save_game.xml");

	if (app->input->getKey(SDL_SCANCODE_S) == KEY_DOWN)
		app->saveGame("save_game.xml");

	// Pinball level
	app->render->blit(pinball_level, 0, 0);

	// RayCast
	if (app->input->getKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		app->input->getMousePosition(ray.x, ray.y);
	}

	// Creation of circles, boxes and Rick's heads.
	if (app->input->getKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(app->physics->createCircle(app->input->getMouseX(), app->input->getMouseY(), 25));
	}

	if (app->input->getKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		balls.add(app->physics->createBall(app->input->getMouseX(), app->input->getMouseY(), 6, pinball_ball));
	}

	if (app->input->getKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(app->physics->createRectangle(app->input->getMouseX(), app->input->getMouseY(), 100, 50));
	}

	if (app->input->getKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(app->physics->createChain(app->input->getMouseX(), app->input->getMouseY(), rick_head, 64));
	}

	iPoint mouse;
	app->input->getMousePosition(mouse.x, mouse.y);
	int ray_hit = ray.distanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions
	doubleNode<PhysBody*> *c = circles.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->getPosition(x, y);
		if (c->data->contains(app->input->getMouseX(), app->input->getMouseY()))
			app->render->blit(circle, x, y, NULL, 1.0f, c->data->getRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->getPosition(x, y);
		app->render->blit(box, x, y, NULL, 1.0f, c->data->getRotation());
		if (ray_on)
		{
			int hit = c->data->rayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if (hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->getPosition(x, y);
		app->render->blit(rick, x, y, NULL, 1.0f, c->data->getRotation());
		c = c->next;
	}

	c = balls.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->getPosition(x, y);
		app->render->blit(c->data->texture, x, y, NULL, 1.0f, c->data->getRotation());
		c = c->next;
	}

	// ray -----------------
	if (ray_on == true)
	{
		fVector destination(mouse.x - ray.x, mouse.y - ray.y);
		destination.normalize();
		destination *= ray_hit;

		app->render->drawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if (normal.x != 0.0f)
			app->render->drawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	if (app->input->getKey(SDL_SCANCODE_KP_PLUS) == KEY_UP)
		app->audio->volumeUp();

	if (app->input->getKey(SDL_SCANCODE_KP_MINUS) == KEY_UP)
		app->audio->volumeDown();

	return true;
}

// Called each loop iteration
bool Scene::postUpdate()
{
	bool ret = true;

	if(app->input->getKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::cleanUp()
{
	LOG("Freeing scene");

	return true;
}
