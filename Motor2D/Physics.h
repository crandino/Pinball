#include "Module.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum BODY_TYPE
{
	DYNAMIC,
	STATIC,
	KINEMATIC
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : body(NULL), listener(NULL), texture(NULL)
	{}

	void getPosition(int& x, int &y) const;
	float getRotation() const;
	bool contains(int x, int y) const;
	int rayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
	void push(float x, float y);
	void setPosition(int x, int y);
	void setLinearSpeed(int x, int y);

public:
	int				width, height;
	b2Body*					 body;
	Module*				 listener;	
	SDL_Texture*		  texture;
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

	// CRZ 
	PhysBody* createBall(int x, int y, int radius, SDL_Texture* texture);
	PhysBody* createWall(int x, int y, int *points, int size);
	PhysBody* createBouncer(int x, int y, int radius, float density, float restitution);
	PhysBody* createBouncer(int x, int y, int *points, int size, float density, float restitution);

	PhysBody* createPropulsor(int x, int y, SDL_Texture*);
	PhysBody* createRoulette(int x, int y, int width, int height, SDL_Texture*);
	
	void createFlippers();	
	PhysBody *createLeftFlipper(b2Vec2 rotation_point, float32 lower_angle, float32 upper_angle);
	PhysBody *createRightFlipper(b2Vec2 rotation_point, float32 lower_angle, float32 upper_angle);
	b2RevoluteJoint *createFlipperJoint(b2Body *rotor, b2Body *stick, float32 &lower_angle, float32 &upper_angle);

	void activateLeftFlippers();
	void activateRightFlippers();
	void deactivateLeftFlippers();
	void deactivateRightFlippers();

	b2PolygonShape *polyFromPoints(b2PolygonShape *shape, int *points, int size);
	void beginContact(b2Contact *contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

	DList<PhysBody*>			right_flippers;
	DList<PhysBody*>			left_flippers;
	DList<b2RevoluteJoint*>		right_joints_flippers;
	DList<b2RevoluteJoint*>		left_joints_flippers;
	b2PrismaticJoint*			propulsor_joint;

private:

	bool				 debug;
	b2World*			 world;

	b2MouseJoint*  mouse_joint;
	b2Body* 			ground;
	b2Body*		  body_clicked;

	float32			 time_step;
	int32		 velocity_iter;
	int32		 position_iter;

	SDL_Texture*	left_flip_tex;
	SDL_Texture*	right_flip_tex;
	
};