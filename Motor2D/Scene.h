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

	DList<PhysBody*>		 bouncers;
	DList<PhysBody*>   lights_sensors;
	PhysBody*				propulsor;
	PhysBody*				 roulette;
	
	PhysBody*				   sensor;
	bool					   sensed;

	SDL_Texture*		pinball_level;
	SDL_Texture*		propulsor_tex;
	SDL_Texture*		 roulette_tex;
	SDL_Texture*	hit_bouncer_type1;
	SDL_Texture*	hit_bouncer_type2;
	SDL_Texture*	hit_bouncer_type3;
	SDL_Texture*			   B_rect;
	SDL_Texture*			   O_rect;
	SDL_Texture*			   N_rect;
	SDL_Texture*		       U_rect;
	SDL_Texture*		       S_rect;
	SDL_Texture*		       B_star;
	SDL_Texture*		       O_star;
	SDL_Texture*		       N_star;
	SDL_Texture*		       U_star;
	SDL_Texture*		       S_star;

	uint bonus_fx;
	iPoint ray;
	bool ray_on;
};

#endif // __SCENE_H__
