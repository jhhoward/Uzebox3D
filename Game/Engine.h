#ifndef ENGINE_H_
#define ENGINE_H_

#ifdef _WIN32
#include "../Windows/SDLPlatform.h"
#elif !defined(PLATFORM_UZEBOX)
#define PLATFORM_GAMEBUINO 1
#endif

#ifdef PLATFORM_GAMEBUINO
#include "GamebuinoPlatform.h"
#endif

#ifdef PLATFORM_UZEBOX
#include "UzeboxPlatform.h"
#endif

#include "Platform.h"
#include "Renderer.h"
#include "Player.h"
#include "Actor.h"
//#include "Menu.h"
#include "Level.h"

enum
{
	GameState_Menu,
	GameState_PauseMenu,
	GameState_Loading,
	GameState_Playing,
	GameState_Dead,
	GameState_FinishedLevel,
	GameState_StartingLevel
};

enum Difficulty
{
	Difficulty_Baby,
	Difficulty_Easy,
	Difficulty_Medium,
	Difficulty_Hard
};

class Engine
{
public:
	void init();
	void update();
	void draw();
	void startLevel(bool resetPlayer = true);
	void startingLevel();
	Actor* spawnActor(uint8_t spawnId, uint8_t actorType, int8_t cellX, int8_t cellZ);
	
	Renderer renderer;
	Player player;
	Level level;
	//Menu menu;
	Actor actors[MAX_ACTIVE_ACTORS];

	int16_t frameCount;
	uint8_t gameState;
	uint8_t difficulty;
	uint8_t ticks;
};

extern Engine engine;

#endif
