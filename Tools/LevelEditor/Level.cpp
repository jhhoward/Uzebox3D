#include <stdio.h>
#include "Level.h"
#include "LevelEditor.h"

#define UNSELECTED_WALL_COLOUR UZE_RGB(64, 128, 64)
#define SELECTED_WALL_COLOUR UZE_RGB(192, 255, 192)
#define HOVER_WALL_COLOUR UZE_RGB(128, 255, 128)

#define UNSELECTED_PORTAL_COLOUR UZE_RGB(64, 64, 128)
#define SELECTED_PORTAL_COLOUR UZE_RGB(192, 192, 255)
#define HOVER_PORTAL_COLOUR UZE_RGB(128, 128, 255)

#define HOVER_VERTEX_COLOUR UZE_RGB(192, 192, 192)
#define STANDARD_VERTEX_COLOUR UZE_RGB(128, 128, 128)
#define SELECTED_VERTEX_COLOUR UZE_RGB(255, 255, 255)

#define VERTEX_HOVER_BOX_SIZE VERTEX_BOX_SIZE * 2

void Level::drawVertex(Vector2 position, uint8_t colour)
{
	position = gEditor.transformPoint(position);
	gEditor.drawLine(position.x - VERTEX_BOX_SIZE, position.y - VERTEX_BOX_SIZE, position.x + VERTEX_BOX_SIZE, position.y - VERTEX_BOX_SIZE, colour);
	gEditor.drawLine(position.x + VERTEX_BOX_SIZE, position.y - VERTEX_BOX_SIZE, position.x + VERTEX_BOX_SIZE, position.y + VERTEX_BOX_SIZE, colour);
	gEditor.drawLine(position.x + VERTEX_BOX_SIZE, position.y + VERTEX_BOX_SIZE, position.x - VERTEX_BOX_SIZE, position.y + VERTEX_BOX_SIZE, colour);
	gEditor.drawLine(position.x - VERTEX_BOX_SIZE, position.y + VERTEX_BOX_SIZE, position.x - VERTEX_BOX_SIZE, position.y - VERTEX_BOX_SIZE, colour);
}

void Level::drawRoom(int roomIndex)
{
	Room* room = rooms[roomIndex];

	for(int v = 0; v < room->vertices.size(); v++)
	{
		int index = room->vertices[v];

		if(index == gEditor.Selection.selectedVertex)
		{
			drawVertex(vertices[index], SELECTED_VERTEX_COLOUR);
		}
		else if(index == gEditor.Selection.hoverVertex)
		{
			drawVertex(vertices[index], HOVER_VERTEX_COLOUR);
		}
		else
		{
			drawVertex(vertices[index], STANDARD_VERTEX_COLOUR);
		}
	}

	for(int v = 0; v < room->vertices.size(); v++)
	{
		Vector2 first = vertices[room->vertices[v]];
		Vector2 second = v == room->vertices.size() - 1 ? vertices[room->vertices[0]] : vertices[room->vertices[v + 1]];
		first = gEditor.transformPoint(first);
		second = gEditor.transformPoint(second);

		uint8_t colour = UNSELECTED_WALL_COLOUR;

		if(room->walls[v].portalDestination != NO_PORTAL_DESTINATION)
		{
			if(roomIndex == gEditor.Selection.selectedRoom && v == gEditor.Selection.selectedWall)
			{
				colour = SELECTED_PORTAL_COLOUR;
			}
			else if(roomIndex == gEditor.Selection.hoverRoom && v == gEditor.Selection.hoverWall)
			{
				colour = HOVER_PORTAL_COLOUR;
			}
			else
			{
				colour = UNSELECTED_PORTAL_COLOUR;
			}
		}
		else
		{
			if(roomIndex == gEditor.Selection.selectedRoom && v == gEditor.Selection.selectedWall)
			{
				colour = SELECTED_WALL_COLOUR;
			}
			else if(roomIndex == gEditor.Selection.hoverRoom && v == gEditor.Selection.hoverWall)
			{
				colour = HOVER_WALL_COLOUR;
			}
			else
			{
				colour = UNSELECTED_WALL_COLOUR;
			}
		}

		gEditor.drawLine(first.x, first.y, second.x, second.y, colour);
	}
}

Level::Level()
{
	createRoom(Vector2(0, 0), 20);
}

void Level::createRoom(Vector2 position, int size)
{
	Room* newRoom = new Room();

	vertices.push_back(gEditor.snapToGrid(Vector2(position.x - size, position.y - size)));
	vertices.push_back(gEditor.snapToGrid(Vector2(position.x + size, position.y - size)));
	vertices.push_back(gEditor.snapToGrid(Vector2(position.x + size, position.y + size)));
	vertices.push_back(gEditor.snapToGrid(Vector2(position.x - size, position.y + size)));

	newRoom->vertices.push_back(vertices.size() - 4);
	newRoom->vertices.push_back(vertices.size() - 3);
	newRoom->vertices.push_back(vertices.size() - 2);
	newRoom->vertices.push_back(vertices.size() - 1);

	newRoom->walls.push_back(Wall());
	newRoom->walls.push_back(Wall());
	newRoom->walls.push_back(Wall());
	newRoom->walls.push_back(Wall());

	rooms.push_back(newRoom);
}

void Level::draw()
{
	for(int r = 0; r < rooms.size(); r++)
	{
		drawRoom(r);
	}
}

int Level::findHoverVertex(Vector2 position)
{
	for(int n = 0; n < vertices.size(); n++)
	{
		if(position.x >= vertices[n].x - VERTEX_HOVER_BOX_SIZE && position.x <= vertices[n].x + VERTEX_HOVER_BOX_SIZE && position.y >= vertices[n].y - VERTEX_HOVER_BOX_SIZE && position.y <= vertices[n].y + VERTEX_HOVER_BOX_SIZE)
			return n;
	}
	return NO_SELECTION;
}

// Distance from line, sign indicates side of line
float DistanceFromLine(Vector2 a, Vector2 b, Vector2 p)
{
	float proj = (b.x - a.x) * (a.y - p.y) - (a.x - p.x) * (b.y - a.y);
	float mag = sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
	return proj / mag;
}

// Is on line if result between 0 and 1
float ProjectToLine(Vector2 a, Vector2 b, Vector2 p)
{
	float proj = (p.x - a.x) * (b.x - a.x) + (p.y - a.y) * (b.y - a.y);
	float magSqr = ((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
	return proj / magSqr;
}

bool Level::isHovering(Vector2 a, Vector2 b, Vector2 p)
{
	float distance = DistanceFromLine(a, b, p);
	float proj = ProjectToLine(a, b, p);

	if(proj >= 0.0f && proj <= 1.0f && distance >= -10.0f && distance < 10.0f)
		return true;
	return false;
}

void Level::findHoverWall(Vector2 position, int& roomIndex, int& wallIndex)
{
	roomIndex = NO_SELECTION;
	wallIndex = NO_SELECTION;

	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			Vector2 first = vertices[rooms[r]->vertices[v]];
			Vector2 second = v == rooms[r]->vertices.size() - 1 ? vertices[rooms[r]->vertices[0]] : vertices[rooms[r]->vertices[v + 1]];

			if(isHovering(first, second, position))
			{
				roomIndex = r;
				wallIndex = v;
				return;
			}
		}
	}
}

void Level::mergePotentialVertices(int vertexIndex)
{
	int indexToMerge = NO_SELECTION;

	for(int n = 0; n < vertices.size(); n++)
	{
		if(n != vertexIndex && vertices[n].x == vertices[vertexIndex].x && vertices[n].y == vertices[vertexIndex].y)
		{
			indexToMerge = n;
			break;
		}
	}

	if(indexToMerge != NO_SELECTION)
	{
		replaceVertexReference(indexToMerge, vertexIndex);
		deleteVertex(indexToMerge);
	}

	checkForNewPortals();
}


void Level::checkForNewPortals()
{
	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			if(rooms[r]->walls[v].portalDestination == NO_PORTAL_DESTINATION)
			{
				int first = rooms[r]->vertices[v];
				int second = v == rooms[r]->vertices.size() - 1 ? rooms[r]->vertices[0] : rooms[r]->vertices[v + 1];

				int portalRoomIndex, portalWallIndex;
				if(findWall(second, first, portalRoomIndex, portalWallIndex))
				{
					rooms[r]->walls[v].portalDestination = portalRoomIndex;
					rooms[portalRoomIndex]->walls[portalWallIndex].portalDestination = r;
				}
			}
		}
	}
}

bool Level::findWall(int firstVertex, int secondVertex, int& roomIndex, int& wallIndex)
{
	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			int first = rooms[r]->vertices[v];
			int second = v == rooms[r]->vertices.size() - 1 ? rooms[r]->vertices[0] : rooms[r]->vertices[v + 1];
			if(firstVertex == first && secondVertex == second)
			{
				roomIndex = r;
				wallIndex = v;
				return true;
			}
		}
	}
	return false;
}

void Level::replaceVertexReference(int target, int replacement)
{
	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			if(rooms[r]->vertices[v] == target)
			{
				rooms[r]->vertices[v] = replacement;
			}
		}
	}
}

int Level::countVertexUsage(int vertexIndex)
{
	int count = 0;

	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			if(rooms[r]->vertices[v] == vertexIndex)
				count++;
		}
	}
	return count;
}

void Level::deleteRoom(int toDelete)
{
	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			if(rooms[r]->walls[v].portalDestination == toDelete)
			{
				rooms[r]->walls[v].portalDestination = NO_PORTAL_DESTINATION;
			}
		}
	}

	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			if(rooms[r]->walls[v].portalDestination > toDelete)
			{
				rooms[r]->walls[v].portalDestination--;
			}
		}
	}
	
	delete rooms[toDelete];
	rooms.erase(rooms.begin() + toDelete);

	for(int v = 0; v < vertices.size(); v++)
	{
		int count = countVertexUsage(v);
		if(count == 0)
		{
			deleteVertex(0);
			v --;
		}
	}
}

void Level::checkForInvalidRooms()
{
	for(int r = 0; r < rooms.size(); r++)
	{
		if(rooms[r]->vertices.size() < 3)
		{
			deleteRoom(r);
			r--;
		}
	}
}

void Level::deleteVertex(int toDelete)
{
	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			if(rooms[r]->vertices[v] == toDelete)
			{
				rooms[r]->vertices.erase(rooms[r]->vertices.begin() + v);
				rooms[r]->walls.erase(rooms[r]->walls.begin() + v);
			}
		}
	}

	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			if(rooms[r]->vertices[v] > toDelete)
			{
				rooms[r]->vertices[v] = rooms[r]->vertices[v] - 1;
			}
		}
	}

	if(gEditor.Selection.hoverVertex == toDelete)
	{
		gEditor.Selection.hoverVertex = NO_SELECTION;
	}
	else if(gEditor.Selection.hoverVertex > toDelete)
	{
		gEditor.Selection.hoverVertex--;
	}

	vertices.erase(vertices.begin() + toDelete);

	checkForInvalidRooms();
	fixInvalidPortals();
}

void Level::fixInvalidPortals()
{
	for(int r = 0; r < rooms.size(); r++)
	{
		for(int v = 0; v < rooms[r]->vertices.size(); v++)
		{
			int portalDestination = rooms[r]->walls[v].portalDestination;

			if(portalDestination >= rooms.size())
			{
				rooms[r]->walls[v].portalDestination = portalDestination = NO_PORTAL_DESTINATION;
			}

			if(portalDestination != NO_PORTAL_DESTINATION)
			{
				int first = rooms[r]->vertices[v];
				int second = v == rooms[r]->vertices.size() - 1 ? rooms[r]->vertices[0] : rooms[r]->vertices[v + 1];
				bool portalIsValid = false;

				for(int w = 0; w < rooms[portalDestination]->vertices.size(); w++)
				{
					int firstCompare = rooms[portalDestination]->vertices[w];
					int secondCompare = w == rooms[portalDestination]->vertices.size() - 1 ? rooms[portalDestination]->vertices[0] : rooms[portalDestination]->vertices[w + 1];
					if(firstCompare == second && secondCompare == first)
					{
						portalIsValid = true;
					}
				}

				if(!portalIsValid)
				{
					rooms[r]->walls[v].portalDestination = NO_PORTAL_DESTINATION;
				}
			}
		}
	}
}

void Level::splitWall(int roomIndex, int wallIndex)
{
	int first = rooms[roomIndex]->vertices[wallIndex];
	int second = wallIndex == rooms[roomIndex]->vertices.size() - 1 ? rooms[roomIndex]->vertices[0] : rooms[roomIndex]->vertices[wallIndex + 1];

	if(rooms[roomIndex]->walls[wallIndex].portalDestination == NO_PORTAL_DESTINATION)
	{
		Vector2 newVertPos((vertices[first].x + vertices[second].x) / 2, (vertices[first].y + vertices[second].y) / 2);
		vertices.push_back(newVertPos);
		rooms[roomIndex]->vertices.insert(rooms[roomIndex]->vertices.begin() + wallIndex + 1, vertices.size() - 1);
		rooms[roomIndex]->walls.insert(rooms[roomIndex]->walls.begin() + wallIndex + 1, Wall());
	}
	else
	{
		Vector2 wallVector(vertices[second].x - vertices[first].x, vertices[second].y - vertices[first].y);
		float mult = sqrt((wallVector.x * wallVector.x) + (wallVector.y * wallVector.y)) / 10;
		Vector2 normal(-wallVector.y / mult, wallVector.x / mult);
		Vector2 newFirst = Vector2(vertices[first].x + normal.x, vertices[first].y + normal.y);
		Vector2 newSecond = Vector2(vertices[second].x + normal.x, vertices[second].y + normal.y);

		vertices.push_back(newFirst);
		vertices.push_back(newSecond);

		int newFirstIndex = vertices.size() - 2;
		int newSecondIndex = vertices.size() - 1;

		rooms[roomIndex]->walls[wallIndex].portalDestination = NO_PORTAL_DESTINATION;
		rooms[roomIndex]->vertices[wallIndex] = newFirstIndex;
		if(wallIndex == rooms[roomIndex]->vertices.size() - 1)
			rooms[roomIndex]->vertices[0] = newSecondIndex;
		else rooms[roomIndex]->vertices[wallIndex + 1] = newSecondIndex;

		fixInvalidPortals();
	}
}

char fileFormatHeader[] = "MAP1";

struct FileHeader
{
	char versionTag[4];
	uint8_t numRooms;
	int16_t roomDataOffset;
	uint8_t numVertices;
	int16_t vertexDataOffset;
	uint8_t numObjects;
	int16_t objectDataOffset;
};

void Level::save(char* filename)
{
	FILE* fs = NULL;

	fopen_s(&fs, filename, "wb");

	if(fs != NULL)
	{
		FileHeader header;

		memcpy(header.versionTag, fileFormatHeader, 4);

		header.numRooms = rooms.size();
		header.numVertices = vertices.size();
		header.numObjects = 0;
		header.vertexDataOffset = sizeof(FileHeader);
		header.roomDataOffset = sizeof(FileHeader) + vertices.size() * sizeof(Vector2);

		fwrite(&header, sizeof(FileHeader), 1, fs);

		for(int v = 0; v < vertices.size(); v++)
		{
			fwrite(&vertices[v], sizeof(Vector2), 1, fs);
		}

		for(int r = 0; r < rooms.size(); r++)
		{
			uint8_t vertCount = rooms[r]->vertices.size();
			fwrite(&vertCount, 1, 1, fs);

			for(int v = 0; v < vertCount; v++)
			{
				uint8_t index = rooms[r]->vertices[v];
				fwrite(&index, 1, 1 , fs);
			}
			for(int v = 0; v < vertCount; v++)
			{
				fwrite(&rooms[r]->walls[v], sizeof(Wall), 1 , fs);
			}
		}

		fclose(fs);
	}
}

void Level::load(char* filename)
{
	FILE* fs = NULL;

	fopen_s(&fs, filename, "rb");

	if(fs != NULL)
	{
		FileHeader header;
		fread(&header, sizeof(FileHeader), 1, fs);

		if(memcmp(fileFormatHeader, header.versionTag, 4) == 0)
		{
			// Clear old data
			for(int r = 0; r < rooms.size(); r++)
				delete rooms[r];
			rooms.clear();
			vertices.clear();

			for(int v = 0; v < header.numVertices; v++)
			{
				Vector2 temp;
				fread(&temp, sizeof(Vector2), 1, fs);
				vertices.push_back(temp);
			}

			for(int r = 0; r < header.numRooms; r++)
			{
				Room* room = new Room();
				uint8_t vertCount = 0;
				fread(&vertCount, 1, 1, fs);

				for(int v = 0; v < vertCount; v++)
				{
					uint8_t index = 0;
					fread(&index, 1, 1, fs);
					room->vertices.push_back(index);
				}

				for(int v = 0; v < vertCount; v++)
				{
					Wall wall;
					fread(&wall, sizeof(Wall), 1, fs);
					room->walls.push_back(wall);
				}

				rooms.push_back(room);
			}
		}

		fclose(fs);
	}
}

template <typename T>
void appendBuffer(vector<uint8_t>& stream, T& data)
{
	uint8_t* ptr = (uint8_t*)(&data);
	for(int n = 0; n < sizeof(T); n++)
	{
		stream.push_back(ptr[n]);
	}
}

void Level::saveHeader(char* filename)
{
	FILE* fs = NULL;

	fopen_s(&fs, filename, "w");

	if(fs != NULL)
	{
		FileHeader header;
		vector<uint8_t> dataStream;

		memcpy(header.versionTag, fileFormatHeader, 4);

		header.numRooms = rooms.size();
		header.numVertices = vertices.size();
		header.numObjects = 0;
		header.vertexDataOffset = sizeof(FileHeader);
		header.roomDataOffset = sizeof(FileHeader) + vertices.size() * sizeof(Vector2);

		appendBuffer(dataStream, header);

		for(int v = 0; v < vertices.size(); v++)
		{
			appendBuffer(dataStream, vertices[v]);
		}

		uint16_t roomDataStart = dataStream.size() + sizeof(uint16_t) * rooms.size();

		vector<uint8_t> roomDataStream;

		for(int r = 0; r < rooms.size(); r++)
		{
			uint16_t offset = roomDataStream.size() + roomDataStart;
			appendBuffer(dataStream, offset);

			uint8_t vertCount = rooms[r]->vertices.size();
			appendBuffer(roomDataStream, vertCount);

			for(int v = 0; v < vertCount; v++)
			{
				uint8_t index = rooms[r]->vertices[v];
				appendBuffer(roomDataStream, index);
			}
			for(int v = 0; v < vertCount; v++)
			{
				appendBuffer(roomDataStream, rooms[r]->walls[v]);
			}
		}

		dataStream.insert(dataStream.end(), roomDataStream.begin(), roomDataStream.end());

		fprintf(fs, "const uint8_t Data_Level[] PROGMEM = {\n\t");
		for(int n = 0; n < dataStream.size(); n++)
		{
			fprintf(fs, "0x%02x", dataStream[n]);
			
			if(n != dataStream.size() - 1)
			{
				fprintf(fs, ",");
				
				if(n > 0 && (n % 20) == 0)
				{
					fprintf(fs, "\n\t");
				}
			}
		}
		fprintf(fs, "\n};\n");


		fclose(fs);
	}
}

