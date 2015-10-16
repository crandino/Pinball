#include "App.h"
#include "Input.h"
#include "Window.h"
#include "Render.h"
#include "Physics.h"
#include "Point2d.h"
#include "p2Log.h"
#include "math.h"

//#ifdef _DEBUG
//#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
//#else
//#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
//#endif

/*

class A
{ void metodo1 }

class B
{ void metodo2 }

class C : public A, B
{ void metodo3 }

A* c1;
B* c2;
C* c3;
C mi_clase;

c1 = (A*)&mi_clase;  --> Només veu el metodo1
c2 = (B*)&mi_clase;  --> Només veu el metodo2
c3 = &mi_clase;      --> Veu els metodo1, metodo2 i metodo3

*/

Physics::Physics()
{
	name.create("physics");
	world = NULL;
	debug = true;
}

// Destructor
Physics::~Physics()
{
}

bool Physics::awake(pugi::xml_node& node)
{
	bool ret = true;
	
	time_step = (1.0f / node.attribute("step_per_second").as_float());
	velocity_iter = node.attribute("velocity_iter").as_int();
	position_iter = node.attribute("position_iter").as_int();

	return ret;
}

bool Physics::start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));	
	world->SetContactListener(this);
	// TODO 3: You need to make ModulePhysics class a contact listener
	// big static circle as "ground" in the middle of the screen
	uint width, height;
	app->win->getWindowSize(width, height);
	int x = width / 2;
	int y = height / 1.5f;
	int diameter = width / 2;

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	b->CreateFixture(&fixture);

	return true;
}

bool Physics::preUpdate()
{
	world->Step(time_step, velocity_iter, position_iter);

	// TODO: HomeWork
	/*
	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
	}
	*/

	return true;
}

PhysBody* Physics::createCircle(int x, int y, int radius)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	// TODO 4: add a pointer to PhysBody as UserData to the body
	// PhysBody está creat per mostrar objectes del Box2D a altres 
	// mòduls. Però no el Box2D directament.
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->body->SetUserData((void*)pbody);	
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* Physics::createRectangle(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->body->SetUserData(pbody);	
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* Physics::createChain(int x, int y, int* points, int size)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(int i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->body->SetUserData(pbody);	
	pbody->width = pbody->height = 0;

	return pbody;
}

// 
bool Physics::postUpdate()
{
	if(app->input->getKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return true;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					app->render->drawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							app->render->drawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					app->render->drawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							app->render->drawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					app->render->drawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					app->render->drawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}
		}
	}

	return true;
}


// Called before quitting
bool Physics::cleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void Physics::beginContact(b2Contact *contact)
{
	LOG("Collision!");
	// TODO 7: Call the listeners that are not NULL

	//PhysBody *bodyA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();

	//if (bodyA->module != NULL)
}

void PhysBody::getPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::getRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::contains(int x, int y) const
{
	// TODO 1: Write the code to return true in case the point
	// is inside ANY of the shapes contained by this body
	b2Fixture *fixt = body->GetFixtureList();
	b2Vec2 vec(PIXEL_TO_METERS(x),PIXEL_TO_METERS(y));

	for (fixt; fixt != NULL; fixt = fixt->GetNext())
	{
		if(fixt->GetShape()->TestPoint(body->GetTransform(), vec))
			return true;
	}

	return false;
}

int PhysBody::rayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	// TODO 2: Write code to test a ray cast between both points provided. If not hit return -1
	// if hit, fill normal_x and normal_y and return the distance between x1,y1 and it's colliding point
	int ret = -1;

	b2Fixture *fixt = body->GetFixtureList();
	b2RayCastInput input;
	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));

	b2RayCastOutput output;

	for (fixt; fixt != NULL; fixt = fixt->GetNext())
	{
		if (fixt->GetShape()->RayCast(&output, input, body->GetTransform(), 0))
		{
			normal_x = output.normal.x; 
			normal_y = output.normal.y;
			b2Vec2 hit_point = input.p1 + output.fraction * (input.p2 - input.p1);
			ret = METERS_TO_PIXELS(b2Distance(input.p1, hit_point));
		}
	}

	return ret;
}