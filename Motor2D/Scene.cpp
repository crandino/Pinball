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
#include "ChainPoints.h"


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
	app->audio->loadFx("sounds/fx/bonus.wav");

	circle = app->tex->loadTexture("textures/wheel.png");
	box = app->tex->loadTexture("textures/crate.png");
	rick = app->tex->loadTexture("textures/rick_head.png");
	pinball_ball = app->tex->loadTexture("textures/pinball_ball.png");
	pinball_level = app->tex->loadTexture("textures/pinball_level.png");
	flipper_tex = app->tex->loadTexture("textures/flipper.png");
	propulsor_tex = app->tex->loadTexture("textures/propulsor.png");


	walls.add(app->physics->createWall(0, 0, triangle1, sizeof(triangle1) / sizeof(int) ));
	//walls.add(app->physics->createWall(0, 0, triangle2, sizeof(triangle2) / sizeof(int)));
	//walls.add(app->physics->createWall(0, 0, triangle3, sizeof(triangle3) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, triangle4, sizeof(triangle4) / sizeof(int)));
	//walls.add(app->physics->createWall(0, 0, bottom_part, sizeof(bottom_part) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, left_L, sizeof(left_L) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, left_R, sizeof(left_R) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, contour, sizeof(contour) / sizeof(int)));

	flip = app->physics->createFlipper(0, 0, flipper, sizeof(flipper) / sizeof(int), flipper_tex);
	propulsor = app->physics->createPropulsor(313, 534, propulsor_tex);
	

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

	// Fliper
	int x, y;
	flip->getPosition(x, y);
	app->render->blit(flipper_tex, x,y);

	int a, b;
	propulsor->getPosition(a, b);
	app->render->blit(propulsor_tex, a, b);

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

void Scene::onCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	app->audio->playFx(bonus_fx);

	/*
	if(bodyA)
	{
	bodyA->GetPosition(x, y);
	App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
	bodyB->GetPosition(x, y);
	App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}


// Called before quitting
bool Scene::cleanUp()
{
	LOG("Freeing scene");

	return true;
}
