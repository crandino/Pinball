#ifndef __APP_H__
#define __APP_H__

#include "DList.h"
#include "Module.h"
#include "PugiXml\src\pugixml.hpp"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class FileSystem;
class Scene;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool awake();

	// Called before the first frame
	bool start();

	// Called each loop iteration
	bool update();

	// Called before quitting
	bool cleanUp();

	// Add a new module to handle
	void addModule(Module* module);

	int gerArgc() const;
	const char* gerArgv(int index) const;

	void loadGame(const char *file);
	void saveGame(const char *file) const;
	void *getSaveGames(DList<p2SString> &list_saved_games) const;

private:

	// Call modules before each loop iteration
	void prepareUpdate();

	// Call modules before each loop iteration
	void finishUpdate();

	// Call modules before each loop iteration
	bool preUpdate();

	// Call modules on each loop iteration
	bool doUpdate();

	// Call modules after each loop iteration
	bool postUpdate();

	// Return a node with the corresponding configuration node
	pugi::xml_node loadConfig(pugi::xml_document&) const;

	bool saveGameNow() const;
	bool loadGameNow();

public:

	uint				frames;
	float				dt;
	pugi::xml_document	config_file;
	pugi::xml_node		config;

	// Modules
	Window*				win;
	Input*				input;
	Render*				render;
	Textures*			tex;
	Audio*				audio;
	Scene*				scene;
	FileSystem*			fs;

private:

	DList<Module*>		modules;
	int					argc;
	char**				args;

	bool				want_to_load;
	mutable bool		want_to_save;
	p2SString			load_game;
	mutable p2SString	save_game;
};

extern App *app;

#endif