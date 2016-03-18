/*
header:
	num rooms [1]
	room data offset [2]
	num objects [1]
	object data offset [2]
	num vertices [1]
	vertex data offset [2]
vertex data:
	x	[2]
	y	[2]
room data:
	room header:
		num walls[1]
		wall header: 
			vertex index [1]
			flags [1]
			portal [1]
object data:
	type [1]
	room [1]
	x [2]
	y [2]
	direction [1]
*/

/*
ALT layout
level header:
	num rooms [1]
	room data offset [2]
	num objects [1]
	object data offset [2]
room data: (x numRooms)
	room ptr table [2 * numRooms]
	room header:
		num verts[1]
		wall data: (x numVerts)
			x [2]
			y [2]
			colour [1]
			flags [1]
			portal destination [1]
object data: (x numObjects)
	type [1]
	room [1]
	x [2]
	y [2]
	direction [1]
*/

/*
level size = 6 + numRooms * (3 + average verts * 7) + 7 * numObjects
*/

class Level
{
public:
	void Init(void* level)
	{
		levelData = (uint8_t*)level;
	}
	
	RoomData* GetRoom(uint8_t roomIndex)
	{
		uint16_t* roomsTable = (uint16_t*)(levelData + pgm_read_word(&levelHeader->roomDataOffset));
		return (RoomData*)(levelData + pgm_read_word(&roomsTable[roomIndex]));
	}
	
	Vector2 GetVertex(uint8_t vertexIndex)
	{
		Vector2* vertexData = (Vector2*)(levelData + pgm_read_word(&levelHeader->vertexDataOffset));
		return Vector2(pgm_read_word(&vertexData[vertexIndex].x), pgm_read_word(&vertexData[vertexIndex].y));
	}
	
	inline uint8_t GetNumRooms()
	{
		return pgm_read_byte(&levelHeader->numRooms);
	}
	
	ObjectData* GetObject(uint8_t objectIndex)
	{
		return (ObjectData*)(levelData + pgm_read_word(&levelHeader->objectDataOffset) + (objectIndex * sizeof(ObjectData));
	}

	inline uint8_t GetNumObjects()
	{
		return pgm_read_byte(&levelHeader->numObjects);
	}

	union
	{
		uint8_t* levelData;
		LevelDataHeader* levelHeader;
	};
};

struct LevelDataHeader
{
	uint8_t numRooms;
	uint16_t roomDataOffset;
	uint8_t numVertices;
	uint16_t vertexDataOffset;
	uint8_t numObjects;
	uint16_t objectDataOffset;
};

struct RoomData
{
	uint8_t GetVertex(uint8_t index)
	{
		uint8_t* vertexData = ((uint8_t*)(this) + sizeof(RoomData));
		return pgm_read_byte(&vertexData[index]);
	}
	WallData* GetWallData(uint8_t index)
	{
		WallData* wallData = (WallData*)((uint8_t*)(this) + sizeof(RoomData) + GetNumWalls() * sizeof(Vector2));
		return &wallData[index];
	}
	
	inline uint8_t GetNumWalls()	{ return pgm_read_byte(&numWalls); }
	uint8_t numWalls;
};

struct WallData
{
	inline uint8_t GetColour() { return pgm_read_byte(&colour); }
	inline uint8_t GetFlags() { return pgm_read_byte(&flags); }
	inline uint8_t GetPortalDestination() { return pgm_read_byte(&portalDestination); }

	uint8_t colour;
	uint8_t flags;
	uint8_t portalDestination;
};

struct ObjectData
{
	uint8_t type;
	uint8_t room;
	Vector2 position;
	angle_t rotation;
};

/////////////

struct DrawContext
{
	uint8_t roomIndex;
	uint8_t scissorLeft, scissorRight;
};

class Renderer
{
	struct
	{
		Vector2 position;
		angle_t rotation;
	} View;
	
	// Queue of rooms to draw
	DrawContext roomQueue[MAX_ROOM_QUEUE_SIZE];
};

enum
{
	LevelColour_Ceiling = 0,
	LevelColour_Floor,
	LevelColour_TopWallEdge,
	LevelColour_BottomWallEdge,
	LevelColour_Wall,
	LevelColour_AltWall,
};

uint8_t LevelColours[] PROGMEM =
{
	UZE_RGB(127, 127, 127),		// Ceiling
	UZE_RGB(64, 64, 64),		// Floor
	UZE_RGB(32, 32, 32),		// Top wall edge
	UZE_RGB(32, 32, 32),		// Bottom wall edge
	UZE_RGB(192, 192, 192),		// Wall
	UZE_RGB(255, 255, 255),		// Alt wall
};

uint8_t colourLUT[512];

void UpdateLevelColours(uint8_t* colours)
{
	uint8_t* ptr = colourLUT;

	// Top half
	for(int n = 0; n < 64; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Ceiling]);
	(*ptr++) = pgm_read_byte(&colours[Level_Colour_TopWallEdge]);
	for(int n = 0; n < 63; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Wall]);
	
	// Top half alt
	for(int n = 0; n < 64; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Ceiling]);
	(*ptr++) = pgm_read_byte(&colours[Level_Colour_TopWallEdge]);
	for(int n = 0; n < 63; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_AltWall]);

	// Bottom half
	for(int n = 0; n < 64; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Floor]);
	(*ptr++) = pgm_read_byte(&colours[Level_Colour_BottomWallEdge]);
	for(int n = 0; n < 63; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Wall]);
	
	// Bottom half alt
	for(int n = 0; n < 64; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Floor]);
	(*ptr++) = pgm_read_byte(&colours[Level_Colour_BottomWallEdge]);
	for(int n = 0; n < 63; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_AltWall]);
}

void TransformVertex(Vector2& worldSpaceVertex, Vector2& viewSpaceVertex)
{

}

void QueueRoom(uint8_t roomIndex, uint8_t scissorLeft, uint8_t scissorRight)
{
	// Check it has not already been added to the queue by a different path
	for(int n = 0; n < roomQueueSize; n++)
	{
		if(roomQueue[n].roomIndex == roomIndex && roomQueue[n].scissorLeft == scissorLeft)
			return;
	}

	if(roomQueueSize == MAX_ROOM_QUEUE_SIZE)
		return;
	roomQueue[roomQueueSize].roomIndex = roomIndex;
	roomQueue[roomQueueSize].scissorLeft = roomIndex;
	roomQueue[roomQueueSize].scissorRight = roomIndex;
	
	roomQueueSize++;
}

void Draw()
{
	QueueRoom(player->CurrentRoom(), 0, DISPLAYWIDTH);
	
	for(int n = 0; n < roomQueueSize; n++)
	{
		currentContext = &roomQueue[n];
		currentRoom = GetRoom(currentContext->roomIndex);
		DrawRoom();
	}
}
	
void DrawRoom()
{
	uint8_t numWalls = currentRoom->NumWalls();
	uint8_t lastWall = numWalls - 1;

	Vector2 worldSpaceVertex;
	currentRoom->GetVertex(0, worldSpaceVertex);
	
	Vector2 firstVertex;	
	TransformVertex(worldSpaceVertex, firstVertex);
	
	Vector2 prevVertex = firstVertex;
	Vector2 viewSpaceVertex;
	
	for(int n = 0; n < numWalls; n++)
	{
		if(n == lastWall)
		{
			viewSpaceVertex = firstVertex;
		}
		else
		{
			currentRoom->GetVertex(n + 1, worldSpaceVertex);
			TransformVertex(worldSpaceVertex, viewSpaceVertex);
		}

		DrawWall(n, prevVertex, viewSpaceVertex);
		
		prevVertex = viewSpaceVertex;
	}
}

void DrawWall(uint8_t wallIndex, Vector2& vert1, Vector2& vert2)
{
	// Back face
	if(vert1.x > vert2.x)
		return;
	// Behind
	if(vert1.y < 0 && vert2.y < 0)
		return;
	
	// Out of scissor region
	if(vert1.x < currentContext->scissorLeft && vert2.x < currentContext->scissorLeft)
		return;
	if(vert1.x > currentContext->scissorRight && vert2.x > currentContext->scissorRight)
		return;
		
	uint8_t wallFlags = currentRoom->GetWallFlags(wallIndex);
	if(wallFlags & WallFlags_Portal)
	{
		uint8_t scissorLeft = max(currentContext->scissorLeft, vert1.x);
		uint8_t scissorRight = min(currentContext->scissorRight, vert2.x);
		
		if(scissorLeft < scissorRight)
		{
			QueueRoom(currentRoom->GetWallPortal(wallIndex), scissorLeft, scissorRight);
		}
	}
	else
	{
		// rendering code
	}
}

/////////////

void UpdateScreenBuffer()
{
	for(int y = 0; y < DISPLAYHEIGHT; y++)
	{
		for(int x = 0; x < DISPLAYWIDTH; x++)
		{
			int offset = y < 64 ? y : 383 - y;
			uint8_t colour = colourLUT[offset + displayBuffer[x]];
			PutPixel(x, y, colour);
		}
	}
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

Vector2 LineIntersectionPoint(Vector2 ps1, Vector2 pe1, Vector2 ps2, Vector2 pe2)
{
	// Get A,B,C of first line - points : ps1 to pe1
	float A1 = pe1.y-ps1.y;
	float B1 = ps1.x-pe1.x;
	float C1 = A1*ps1.x+B1*ps1.y;

	// Get A,B,C of second line - points : ps2 to pe2
	float A2 = pe2.y-ps2.y;
	float B2 = ps2.x-pe2.x;
	float C2 = A2*ps2.x+B2*ps2.y;

	// Get delta and check if the lines are parallel
	float delta = A1*B2 - A2*B1;
	if(delta == 0)
	 throw new System.Exception("Lines are parallel");

	// now return the Vector2 intersection point
	return Vector2((B2*C1 - B1*C2)/delta, (A1*C2 - A2*C1)/delta);
}

	
/*	deltaX = b->x - a->x;
	deltaY = b->y - a->y;
	deltaX = FIXED_TO_INT(deltaX);
	deltaY = FIXED_TO_INT(deltaY);

	restLengthSqr = PARTICLE_COLLISION_RADIUS * PARTICLE_COLLISION_RADIUS;
	deltaLengthSqr = (deltaX * deltaX) + (deltaY * deltaY);

	if(deltaLengthSqr > restLengthSqr)
		return false;

	mult = (INT_TO_FIXED(restLengthSqr) / (deltaLengthSqr + restLengthSqr)) - FIXED_HALF;


	deltaX *= mult;
	deltaY *= mult;

	a->x -= deltaX;
	a->y -= deltaY;
	b->x += deltaX;
	b->y += deltaY;

*/
/*
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
	float det = (-s2_x * s1_y + s1_x * s2_y);
	
	if(det == 0)
		return 0;
	
    //s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / det;
    //t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / det;

	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y));
	t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x));
	
    //if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	if(s >= 0 && s <= det && t >= 0 && t <= det)
    {
        // Collision detected
        if (i_x != NULL)
//            *i_x = p0_x + (t * s1_x);
            *i_x = p0_x + (t * s1_x) / det;
        if (i_y != NULL)
//            *i_y = p0_y + (t * s1_y);
            *i_y = p0_y + (t * s1_y) / det; 
        return 1;
    }

    return 0; // No collision
}
*/
/*
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y));
	t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x));

	float det = (-s2_x * s1_y + s1_x * s2_y);
	
	if(det > 0)
	{
		if(s >= 0 && s <= det && t >= 0 && t <= det)
		{
			// Collision detected
			if (i_x != NULL)
				*i_x = p0_x + (t * s1_x) / det;
			if (i_y != NULL)
				*i_y = p0_y + (t * s1_y) / det; 
			return 1;
		}
	}
	else if(det < 0)
	{
		if(s <= 0 && s >= det && t <= 0 && t >= det)
		{
			// Collision detected
			if (i_x != NULL)
				*i_x = p0_x + (t * s1_x) / det;
			if (i_y != NULL)
				*i_y = p0_y + (t * s1_y) / det; 
			return 1;
		}
	}
	
    return 0; // No collision
}
*/
/*
/////
int get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = p1_x - p0_x;
    s10_y = p1_y - p0_y;
    s32_x = p3_x - p2_x;
    s32_y = p3_y - p2_y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0)
        return 0; // Collinear
    bool denomPositive = denom > 0;

    s02_x = p0_x - p2_x;
    s02_y = p0_y - p2_y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive)
        return 0; // No collision

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive)
        return 0; // No collision

    if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
        return 0; // No collision
    // Collision detected
    t = t_numer / denom;
    if (i_x != NULL)
        *i_x = p0_x + (t * s10_x);
    if (i_y != NULL)
        *i_y = p0_y + (t * s10_y);

    return 1;
}
*/
//////////
// Should be complete:

int get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = p1_x - p0_x;
    s10_y = p1_y - p0_y;
    s32_x = p3_x - p2_x;
    s32_y = p3_y - p2_y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0)
	{
		if(s10_x == 0 && s10_y != 0 && s32_x != 0 && s32_y == 0)
		{
			if((p0_x < p2_x && p0_x < p3_x) || (p0_x > p2_x && p0_x > p3_x))
				return 0;
			if((p2_y < p0_y && p2_y < p1_y) || (p2_y > p0_y && p2_y > p1_y))
				return 0;
			if(*i_x != NULL)
				*i_x = p0_x;
			if(*i_y != NULL)
				*i_y = p2_y;
			return 1;
		}
		if(s10_x != 0 && s10_y == 0 && s32_x == 0 && s32_y != 0)
		{
			if((p2_x < p0_x && p2_x < p1_x) || (p2_x > p0_x && p2_x > p1_x))
				return 0;
			if((p0_y < p2_y && p0_y < p3_y) || (p0_y > p2_y && p0_y > p3_y))
				return 0;
			if(*i_x != NULL)
				*i_x = p2_x;
			if(*i_y != NULL)
				*i_y = p0_y;
			return 1;
		}
		
        return 0; // Collinear
	}
    bool denomPositive = denom > 0;

    s02_x = p0_x - p2_x;
    s02_y = p0_y - p2_y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive)
        return 0; // No collision

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive)
        return 0; // No collision

    if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
        return 0; // No collision
    // Collision detected
    t = t_numer / denom;
    if (i_x != NULL)
        *i_x = p0_x + (t * s10_x);
    if (i_y != NULL)
        *i_y = p0_y + (t * s10_y);

    return 1;
}

char get_line_intersection(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, Vector2& out)
{
	Vector2 s10, s32, s02;
	int s_numer, t_numer, denom;
	s10.x = p1.x - p0.x;
	s10.y = p1.y - p0.y;
	s32.x = p3.x - p2.x;
	s32.y = p3.y - p2.y;

	denom = s10.x * s32.y - s32.x * s10.y;
	if (denom == 0)
	{
		if(s10.x == 0 && s10.y != 0 && s32.x != 0 && s32.y == 0)
		{
			if((p0.x < p2.x && p0.x < p3.x) || (p0.x > p2.x && p0.x > p3.x))
				return 0;
			if((p2.y < p0.y && p2.y < p1.y) || (p2.y > p0.y && p2.y > p1.y))
				return 0;
			out.x = p0.x;
			out.y = p2.y;
			return 1;
		}
		if(s10.x != 0 && s10.y == 0 && s32.x == 0 && s32.y != 0)
		{
			if((p2.x < p0.x && p2.x < p1.x) || (p2.x > p0.x && p2.x > p1.x))
				return 0;
			if((p0.y < p2.y && p0.y < p3.y) || (p0.y > p2.y && p0.y > p3.y))
				return 0;
			out.x = p2.x;
			out.y = p0.y;
			return 1;
		}
		
		return 0; // Collinear
	}
	bool denomPositive = denom > 0;

	s02.x = p0.x - p2.x;
	s02.y = p0.y - p2.y;
	s_numer = s10.x * s02.y - s10.y * s02.x;
	if ((s_numer < 0) == denomPositive)
		return 0; // No collision

	t_numer = s32.x * s02.y - s32.y * s02.x;
	if ((t_numer < 0) == denomPositive)
		return 0; // No collision

	if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
		return 0; // No collision
	// Collision detected
	out.x = p0.x + (t_numer * s10.x) / denom;
	out.y = p0.y + (t_numer * s10.y) / denom;

	return 1;
}

#define RGB332(r, g, b) ( ((r) & 7) | (((g) & 7) << 3) | (((b) & 3) << 6) )
