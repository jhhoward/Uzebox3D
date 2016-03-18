class LineTrace
{
public:
	LineTrace(RoomIndex startRoom, Vector2& startPoint, Vector2& endPoint) : currentRoom(startRoom), prevRoom(startRoom), start(startPoint), end(endPoint)
	{
		startToEnd.x = end.x - start.x;
		startToEnd.y = end.y - start.y;
	}
	
	bool trace(Vector2& collision)
	{
		bool hitPortal = false;
		
		do
		{
			hitPortal = false;
			RoomData* room = gLevel.getRoomData(currentRoom);
			
			for(int n = 0; n < room->getNumWalls(); n++)
			{
				WallData* wallData = room->getWallData(n);
				RoomIndex portalDestination = wallData->getPortalDestination();
				
				if(portalDestination == prevRoom)
					continue;
					
				Vector2 wallStart, wallEnd;
				room->getWallVertices(wallStart, wallEnd);
				
				if(traceAgainstWall(portalDestination != NO_PORTAL_DESTINATION, wallStart, wallEnd, collision)
				{
					if(portalDestination == NO_PORTAL_DESTINATION)
					{
						return true;
					}
					else
					{
						prevRoom = currentRoom;
						currentRoom = portalDestination;
						hitPortal = true;
						break;
					}
				}
			}
		} while(hitPortal);
		return false;
	}
	
	bool traceAgainstWall(bool isPortal, Vector2& a, Vector2& b, Vector2& collision)
	{
		Vector2 aToB;
		int16_t s_numer, t_numer, denom;
		aToB.x = b.x - a.x;
		aToB.y = b.y - a.y;

		denom = startToEnd.x * aToB.y - aToB.x * startToEnd.y;
		if (denom == 0)
		{
			if(startToEnd.x == 0 && startToEnd.y != 0 && aToB.x != 0 && aToB.y == 0)
			{
				if((end.x < a.x && end.x < b.x) || (end.x > a.x && end.x > b.x))
					return false;
				if((a.y < start.y && a.y < end.y) || (a.y > start.y && a.y > end.y))
					return false;
				if(!isPortal)
				{
					collision.x = end.x;
					collision.y = a.y;
				}
				return true;
			}
			if(startToEnd.x != 0 && startToEnd.y == 0 && aToB.x == 0 && aToB.y != 0)
			{
				if((a.x < start.x && a.x < end.x) || (a.x > start.x && a.x > end.x))
					return 0;
				if((end.y < a.y && end.y < b.y) || (end.y > a.y && end.y > b.y))
					return false;
				if(!isPortal)
				{
					collision.x = a.x;
					collision.y = end.y;
				}
				return true;
			}
			
			return false; // Collinear
		}
		
		bool denomPositive = denom > 0;
		Vector2 startToA;
		
		startToA.x = start.x - a.x;
		startToA.y = start.y - a.y;
		
		s_numer = startToEnd.x * startToA.y - startToEnd.y * startToA.x;
		if ((s_numer < 0) == denomPositive)
			return false; // No collision

		t_numer = aToB.x * startToA.y - aToB.y * startToA.x;
		if ((t_numer < 0) == denomPositive)
			return false; // No collision

		if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
			return false; // No collision
			
		if(!isPortal)
		{
			// Collision detected
			collision.x = start.x + (t_numer * startToEnd.x) / denom;
			collision.y = start.y + (t_numer * startToEnd.y) / denom;
		}

		return true;
	}
	
	RoomIndex currentRoom;
	RoomIndex prevRoom;
	Vector2 start;
	Vector2 end;
	Vector2 startToEnd;
}

void checkCollisionWithOffset(Vector2& from, Vector2& to, Vector2& offset)
{
	Vector2 withOffset(to.x + offset.x, to.y + offset.y);
	Vector2 collision;
	
	LineTrace trace(roomIndex, from, to);
	
	if(trace.trace(collision))
	{
		to.x = collision.x - offset.x;
		to.y = collision.y - offset.y;
	}
}

void moveAndCheckCollision(Vector2& from, Vector2& to, int16_t boxSize)
{
	checkCollisionWithOffset(from, to, Vector2(-boxSize, -boxSize));
	checkCollisionWithOffset(from, to, Vector2(boxSize, -boxSize));
	checkCollisionWithOffset(from, to, Vector2(boxSize, boxSize));
	checkCollisionWithOffset(from, to, Vector2(-boxSize, boxSize));

	LineTrace trace(roomIndex, from, to);

	Vector2 collision;
	if(!trace.trace(collision))
	{
		// Set new room and new position
	}
}
