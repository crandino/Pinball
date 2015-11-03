#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"

class PhysBody;

class Scene : public Module
{
public:

	Scene();
    ~Scene();

	// Called before render is available
	bool awake(pugi::xml_node&);

	// Called before the first frame
	bool start();

	// Called before all updates
	bool preUpdate();

	// Called each loop iteration
	bool update(float dt);

	// Called before all updates
	bool postUpdate();

	// Called before quitting
	bool cleanUp();

	void onCollision(PhysBody*, PhysBody*);

	// ----------------

	DList<PhysBody*>		walls;
	DList<PhysBody*>	 bouncers;
	PhysBody*		    propulsor;
	PhysBody*		     roulette;
	
	PhysBody *sensor;
	bool sensed;

	SDL_Texture *pinball_level;
	SDL_Texture *propulsor_tex;
	SDL_Texture*	roulette_tex;
	SDL_Texture*	hit_bouncer_type1;
	SDL_Texture*	hit_bouncer_type2;
	SDL_Texture*	hit_bouncer_type3;

	uint bonus_fx;
	iPoint ray;
	bool ray_on;
};

#endif // __SCENE_H__
