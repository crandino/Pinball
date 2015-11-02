#include "p2Defs.h"
#include "p2Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "FileSystem.h"
#include "Physics.h"
#include "Player.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"

#include "Box2D\Box2D\Box2D.h"
#include "ChainPoints.h"

Scene::Scene() : Module()
{
	name.create("scene");
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

	pinball_level = app->tex->loadTexture("textures/pinball_level.png");
	propulsor_tex = app->tex->loadTexture("textures/propulsor.png");
	roulette_tex = app->tex->loadTexture("textures/roulette.png");

	app->physics->createFlippers();

	walls.add(app->physics->createWall(0, 0, triangle1, sizeof(triangle1) / sizeof(int) ));
	walls.add(app->physics->createWall(0, 0, triangle2, sizeof(triangle2) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, triangle3, sizeof(triangle3) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, triangle4, sizeof(triangle4) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, left_L, sizeof(left_L) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, left_R, sizeof(left_R) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, contour, sizeof(contour) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, left_mondongo, sizeof(left_mondongo) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, right_mondongo, sizeof(right_mondongo) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, telefon, sizeof(telefon) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, spliter1, sizeof(spliter1) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, spliter2, sizeof(spliter2) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, spliter3, sizeof(spliter3) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, guacamole, sizeof(guacamole) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, top_hat, sizeof(top_hat) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, bottom_part, sizeof(bottom_part) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, left_pipe, sizeof(left_pipe) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, right_pipe, sizeof(right_pipe) / sizeof(int)));

	bouncers.add(app->physics->createBouncer(404, 118, 11, 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(446, 112, 11, 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(433, 154, 11, 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(286, 270, 11, 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(341, 271, 11, 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(290, 305, 11, 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(339, 308, 11, 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(179, 116, 11, 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(0, 0, hypotenuse1, sizeof(hypotenuse1) / sizeof(int), 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(0, 0, hypotenuse2, sizeof(hypotenuse2) / sizeof(int), 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(0, 0, hypotenuse3, sizeof(hypotenuse3) / sizeof(int), 1.0f, 1.2f));
	bouncers.add(app->physics->createBouncer(0, 0, hypotenuse4, sizeof(hypotenuse4) / sizeof(int), 1.0f, 1.2f));
	
	propulsor = app->physics->createPropulsor(313, 484, propulsor_tex);
	roulette = app->physics->createRoulette(313, 124, 4, 34, roulette_tex);

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
		
	// Pinball level rendering
	app->render->blit(pinball_level, 0, 0);

	//// Flippers rendering
	int x, y;
	doubleNode<PhysBody*> *flip_item = app->physics->left_flippers.getFirst();
	while (flip_item != NULL)
	{
		flip_item->data->getPosition(x, y);
		app->render->blit(flip_item->data->texture, x, y, NULL, 1.0f, flip_item->data->getRotation(), 0, 0);
		flip_item = flip_item->next;
	}	

	flip_item = app->physics->right_flippers.getFirst();
	while (flip_item != NULL)
	{
		flip_item->data->getPosition(x, y);
		app->render->blit(flip_item->data->texture, x, y, NULL, 1.0f, flip_item->data->getRotation(), 0, 0);
		flip_item = flip_item->next;
	}

	//Point for update propulsor & ball
	Point2d<int> pos;

	//Propulsor
	propulsor->getPosition(pos.x, pos.y);
	app->render->blit(propulsor->texture, pos.x, pos.y);

	//Roulette
	roulette->getPosition(pos.x, pos.y);
	app->render->blit(roulette->texture, pos.x, pos.y, NULL, 1.0f, roulette->getRotation());

	// RayCast
	if (app->input->getKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		app->input->getMousePosition(ray.x, ray.y);
	}

	static float push_force = 0.0f;

	if (app->input->getKey(SDL_SCANCODE_DOWN) == KEY_DOWN || app->input->getKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		push_force += 175.0f;
		propulsor->push(0, push_force);
	}

	else if (app->input->getKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		push_force = -500.0f;
		propulsor->push(0, push_force);
	}

	else
		push_force = 0.0f;

	
	
	//Mouse
	iPoint mouse;
	app->input->getMousePosition(mouse.x, mouse.y);
	int ray_hit = ray.distanceTo(mouse);

	fVector normal(0.0f, 0.0f);

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
	for (doubleNode<PhysBody*>* tmp = bouncers.getFirst(); tmp != NULL; tmp = tmp->next)
	{
		if (bodyA == tmp->data)
			app->player->score += 10;

		else if (bodyB == tmp->data)
			app->player->score += 10;
	}
}


// Called before quitting
bool Scene::cleanUp()
{
	LOG("Freeing scene");

	return true;
}
