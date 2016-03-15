#pragma once

#include "Vector2.h"
#include "LevelEditor.h"
#include <stdint.h>
#include <vector>

using namespace std;

#define DEFAULT_WALL_COLOUR UZE_RGB(192, 192, 192)
#define NO_PORTAL_DESTINATION 0xff

struct Room;

struct Wall
{
	Wall() : colour(DEFAULT_WALL_COLOUR), flags(0), portalDestination(NO_PORTAL_DESTINATION) {}
	uint8_t colour;
	uint8_t flags;
	uint8_t portalDestination;
};

struct Room
{
	vector<Wall> walls;
	vector<int> vertices;
};

class Level
{
public:
	Level();

	void draw();
	void createRoom(Vector2 position, int size);
	void drawRoom(int roomIndex);
	void drawVertex(Vector2 position, uint8_t colour);
	int findHoverVertex(Vector2 position);
	void findHoverWall(Vector2 position, int& roomIndex, int& wallIndex);

	void mergePotentialVertices(int vertexIndex);
	void replaceVertexReference(int target, int replacement);
	void deleteVertex(int toDelete);
	void deleteRoom(int toDelete);
	void checkForNewPortals();
	void checkForInvalidRooms();
	void fixInvalidPortals();
	int countVertexUsage(int vertexIndex);
	void splitWall(int roomIndex, int wallIndex);

	bool findWall(int firstVertex, int secondVertex, int& roomIndex, int& wallIndex);

	vector<Room*> rooms;
	vector<Vector2> vertices;

	void save(char* filename);
	void load(char* filename);
	void saveHeader(char* filename);

private:
	bool isHovering(Vector2 a, Vector2 b, Vector2 p);
};
