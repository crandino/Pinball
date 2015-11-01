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

	pinball_ball_tex = app->tex->loadTexture("textures/pinball_ball.png");
	pinball_level = app->tex->loadTexture("textures/pinball_level.png");
	propulsor_tex = app->tex->loadTexture("textures/propulsor.png");

	walls.add(app->physics->createWall(0, 0, triangle1, sizeof(triangle1) / sizeof(int) ));
	walls.add(app->physics->createWall(0, 0, triangle2, sizeof(triangle2) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, triangle3, sizeof(triangle3) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, triangle4, sizeof(triangle4) / sizeof(int)));
	//walls.add(app->physics->createWall(0, 0, bottom_part, sizeof(bottom_part) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, left_L, sizeof(left_L) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, left_R, sizeof(left_R) / sizeof(int)));
	walls.add(app->physics->createWall(0, 0, contour, sizeof(contour) / sizeof(int)));

	app->physics->createFlippers();
	propulsor = app->physics->createPropulsor(313, 484, propulsor_tex);
	ball = app->physics->createBall(313, 476, 6, pinball_ball_tex);

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

	//if (app->input->getKey(SDL_SCANCODE_M) == KEY_DOWN)
	//{
	//	app->physics->flip_joint->EnableMotor(true);
	//	app->physics->flip_joint->SetMaxMotorTorque(20.0f);
	//	app->physics->flip_joint->SetMotorSpeed(360 * DEGTORAD);
	//}
		
	// Pinball level rendering
	app->render->blit(pinball_level, 0, 0);

	//// Flippers rendering
	int x, y;
	doubleNode<PhysBody*> *flip_item = app->physics->left_flippers.getFirst();
	while (flip_item != NULL)
	{
		flip_item->data->getPosition(x, y);
		app->render->blit(flip_item->data->texture, x, y, NULL, 1.0f, flip_item->data->getRotation());
		flip_item = flip_item->next;
	}	

	flip_item = app->physics->right_flippers.getFirst();
	while (flip_item != NULL)
	{
		flip_item->data->getPosition(x, y);
		app->render->blit(flip_item->data->texture, x, y, NULL, 1.0f, flip_item->data->getRotation());
		flip_item = flip_item->next;
	}

	//Point for update propulsor & ball
	Point2d<int> upd;

	//Propulsor
	propulsor->getPosition(upd.x, upd.y);
	app->render->blit(propulsor->texture, upd.x, upd.y);

	//Ball
	ball->getPosition(upd.x, upd.y);
	app->render->blit(ball->texture, upd.x, upd.y, NULL, 1.0f, ball->getRotation());

	// RayCast
	if (app->input->getKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		app->input->getMousePosition(ray.x, ray.y);
	}

	//Create Balls
	if (app->input->getKey(SDL_SCANCODE_5) == KEY_DOWN)
		app->physics->createBall(app->input->getMouseX(), app->input->getMouseY(), 6, pinball_ball_tex);

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
