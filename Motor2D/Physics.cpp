#include "App.h"
#include "Input.h"
#include "Window.h"
#include "Render.h"
#include "Physics.h"
#include "Point2d.h"
#include "p2Log.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Motor2D/Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Motor2D/Box2D/libx86/Release/Box2D.lib" )
#endif

Physics::Physics()
{
	name.create("physics");
	mouse_joint = NULL;
	world = NULL;
	debug = true;
}

// Destructor
Physics::~Physics()
{ }

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

	// Ground body is needed to create joints like mouse joint.
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
		
	return true;
}

bool Physics::preUpdate()
{
	world->Step(time_step, velocity_iter, position_iter);

	PhysBody *pb1, *pb2;

	for (b2Contact *c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->onCollision(pb1, pb2);
		}
	}

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

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);	
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
	b->SetUserData(pbody);	
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
	b->SetUserData(pbody);	
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* Physics::createBall(int x, int y, int radius, SDL_Texture *texture)
{
	b2BodyDef body_def;
	body_def.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body_def.type = b2_dynamicBody;

	b2Body *b = world->CreateBody(&body_def);
	b2CircleShape ball_shape;
	ball_shape.m_radius = PIXEL_TO_METERS(radius);

	b2FixtureDef fixture_def;
	fixture_def.shape = &ball_shape;
	fixture_def.density = 1.0f;
	b->CreateFixture(&fixture_def);

	PhysBody *pbody = new PhysBody();
	pbody->body = b;
	pbody->body->SetUserData(pbody);
	pbody->texture = texture;
	pbody->width = pbody->height = radius;
	
	return pbody;
}

PhysBody* Physics::createWall(int x, int y, int *points, int size)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (int i = 0; i < size / 2; ++i)
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

PhysBody* Physics::createFlipper(SDL_Texture* texture)
{
	// Dimensions for rotor and flipper
	b2Vec2 rotor_pos(476, 477);
	b2Vec2 stick_pos(455, 477);
	float radius = 5;
	int stick_w = 48;
	int stick_h = 10;
	
	// Rotor linked to the flipper
	b2BodyDef rotor_def;
	rotor_def.position.Set(PIXEL_TO_METERS(rotor_pos.x), PIXEL_TO_METERS(rotor_pos.y));
	rotor_def.type = b2_staticBody;
	b2Body *rotor = world->CreateBody(&rotor_def);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef rotor_fixture;
	rotor_fixture.shape = &shape;
	//rotor_fixture.density = 1.0f;
	rotor->CreateFixture(&rotor_fixture);

	// The flipper
	b2BodyDef stick_def;
	stick_def.position.Set(PIXEL_TO_METERS(stick_pos.x), PIXEL_TO_METERS(stick_pos.y));
	stick_def.type = b2_dynamicBody;
	b2Body *stick = world->CreateBody(&stick_def);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(stick_w) * 0.5f, PIXEL_TO_METERS(stick_h) * 0.5f);

	b2FixtureDef stick_fixture;
	stick_fixture.shape = &box;
	stick_fixture.density = 1.0f;
	stick->CreateFixture(&stick_fixture);

	// PhysBody declaration
	PhysBody* pbody = new PhysBody();
	pbody->body = stick;
	stick->SetUserData(pbody);
	pbody->texture = texture;
	pbody->width = stick_w * 0.5f;
	pbody->height = stick_h * 0.5f;

	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = rotor;
	revoluteJointDef.bodyB = stick;
	revoluteJointDef.collideConnected = false;
	revoluteJointDef.enableLimit = true;
	//revoluteJointDef.enableMotor = true;
	//revoluteJointDef.referenceAngle = 0.0f;
	revoluteJointDef.lowerAngle = -30.0f * DEGTORAD;
	revoluteJointDef.upperAngle = 50.0f * DEGTORAD;
	//revoluteJointDef.motorSpeed = 30.0f;
	revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
	revoluteJointDef.localAnchorB.Set(PIXEL_TO_METERS(stick_w / 2), PIXEL_TO_METERS(0));	

	flip_joint = (b2RevoluteJoint*)world->CreateJoint(&revoluteJointDef);

	return pbody;
}

PhysBody* Physics::createPropulsor(int x, int y, SDL_Texture* texture)
{
	b2BodyDef bd;
	bd.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	bd.type = b2_dynamicBody;

	b2Body *b = world->CreateBody(&bd);

	b2PolygonShape shape;

	int width = 16, height = 31;
	shape.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->texture = texture;
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	b2Vec2 vec(b->GetPosition());

	b2PrismaticJointDef jointDef;
	b2Vec2 worldAxis(1.0f, 0.0f);
	jointDef.bodyA = ground;
	jointDef.bodyB = b;
	jointDef.localAnchorA.Set(vec.x, vec.y);
	jointDef.localAxisA.Set(0, -1);
	jointDef.lowerTranslation = -1.0f;
	jointDef.upperTranslation = 1.0f;
	jointDef.enableLimit = true;
	jointDef.maxMotorForce = 10.0f;
	jointDef.motorSpeed = 1.0f;
	jointDef.enableMotor = true;

	propulsor_joint = (b2PrismaticJoint*)world->CreateJoint(&jointDef);

	return pbody;
}

// 
bool Physics::postUpdate()
{
	if(app->input->getKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return true;

	b2Vec2 mouse_position(0,0);
	body_clicked = NULL;
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

			if (app->input->getMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				mouse_position.x = PIXEL_TO_METERS(app->input->getMouseX());
				mouse_position.y = PIXEL_TO_METERS(app->input->getMouseY());
				if (f->GetShape()->TestPoint(b->GetTransform(), mouse_position) == true)
					body_clicked = f->GetBody();				
			}
		}
	}

	// If a body was selected we will attach a mouse joint to it
	// so we can pull it around
	// TODO 2: If a body was selected, create a mouse joint
	// using mouse_joint class property

	if (body_clicked != NULL)
	{
		b2MouseJointDef def;
		def.bodyA = ground;
		def.bodyB = body_clicked;
		def.target = mouse_position;
		def.dampingRatio = 0.5f;
		def.frequencyHz = 2.0f;
		def.maxForce = 100.0f * body_clicked->GetMass();

		mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
	}

	// TODO 3: If the player keeps pressing the mouse button, update
	// target position and draw a red line between both anchor points

	if (mouse_joint != NULL && app->input->getMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		b2Vec2 posA, posB;
		posA = mouse_joint->GetAnchorA();
		posB.x = PIXEL_TO_METERS(app->input->getMouseX());
		posB.y = PIXEL_TO_METERS(app->input->getMouseY());
		mouse_joint->SetTarget(posB);

		app->render->drawLine(METERS_TO_PIXELS(posA.x),
							  METERS_TO_PIXELS(posA.y),
						      METERS_TO_PIXELS(posB.x),
							  METERS_TO_PIXELS(posB.y),
							  255, 0, 0);
	}

	// TODO 4: If the player releases the mouse button, destroy the joint
	if (mouse_joint != NULL && app->input->getMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
	{
		if (mouse_joint != NULL)
		{
			world->DestroyJoint(mouse_joint);
			mouse_joint = NULL;
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
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (physA && physA->listener != NULL)
		physA->listener->onCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->onCollision(physB, physA);
	
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
	input.maxFraction = 1.0f;

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