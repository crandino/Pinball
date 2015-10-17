#include "Module.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : body(NULL), module(NULL), texture(NULL)
	{}

	void getPosition(int& x, int &y) const;
	float getRotation() const;
	bool contains(int x, int y) const;
	int rayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	// TODO 6: Add a pointer to a module that might want to listen to a collision from this body
	Module *module;	
	SDL_Texture *texture;
};

// Module --------------------------------------
// TODO 3: Make module physics inherit from b2ContactListener
// then override void BeginContact(b2Contact* contact)
class Physics : public Module, b2ContactListener
{
public:
	Physics();
	~Physics();

	bool awake(pugi::xml_node&);
	bool start();
	bool preUpdate();
	bool postUpdate();
	bool cleanUp();

	PhysBody* createCircle(int x, int y, int radius);
	PhysBody* createRectangle(int x, int y, int width, int height);
	PhysBody* createRectangleSensor(int x, int y, int width, int height);
	PhysBody* createChain(int x, int y, int* points, int size);

	// CRZ 
	PhysBody* createBall(int x, int y, int radius, SDL_Texture*);

	void beginContact(b2Contact *contact);

private:

	bool debug;
	b2World* world;

	float32 time_step;
	int32 velocity_iter;
	int32 position_iter;
};