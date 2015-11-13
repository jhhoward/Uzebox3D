#include "Engine.h"
#include "Renderer.h"
#include "FixedMath.h"
#include "TileTypes.h"

#include "Data_Pistol.h"
#include "Data_Knife.h"
#include "Data_Machinegun.h"
#if 0
#include "Data_Walls.h"
#include "Data_Decorations.h"
#include "Data_BlockingDecorations.h"
#include "Data_Items.h"
#include "Data_Font.h"
#endif

#include <stdio.h>

uint8_t LevelColours[] PROGMEM =
{
	UZE_RGB(127, 127, 127),		// Ceiling
	UZE_RGB(64, 64, 64),		// Floor
	UZE_RGB(32, 32, 32),		// Top wall edge
	UZE_RGB(32, 32, 32),		// Bottom wall edge
	UZE_RGB(192, 192, 192),		// Wall
	UZE_RGB(255, 255, 255),		// Alt wall
};

void Renderer::init()
{
	updateLevelColours(LevelColours);
	drawWeapon();
}

void Renderer::updateLevelColours(uint8_t* colours)
{
	uint8_t* ptr = colourTable;

	// Top half
	for(int n = 0; n < 64; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Ceiling]);
	(*ptr++) = pgm_read_byte(&colours[LevelColour_TopWallEdge]);
	for(int n = 0; n < 63; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Wall]);
	
	// Top half alt
	for(int n = 0; n < 64; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Ceiling]);
	(*ptr++) = pgm_read_byte(&colours[LevelColour_TopWallEdge]);
	for(int n = 0; n < 63; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_AltWall]);

	// Bottom half
	for(int n = 0; n < 64; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Floor]);
	(*ptr++) = pgm_read_byte(&colours[LevelColour_BottomWallEdge]);
	for(int n = 0; n < 63; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Wall]);
	
	// Bottom half alt
	for(int n = 0; n < 64; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_Floor]);
	(*ptr++) = pgm_read_byte(&colours[LevelColour_BottomWallEdge]);
	for(int n = 0; n < 63; n++)
		(*ptr++) = pgm_read_byte(&colours[LevelColour_AltWall]);
}


void Renderer::drawDamage()
{
	if(damageIndicator > 0)
	{
		damageIndicator --;
#if 0
		for(int x = 0; x < DISPLAYWIDTH; x++)
		{
			setPixel(x, 0);
			setPixel(x, DISPLAYHEIGHT - 1);
		}
		for(int y = 0; y < DISPLAYHEIGHT; y++)
		{
			setPixel(0, y);
			setPixel(DISPLAYWIDTH - 1, y);
		}
#endif
	}
}

void Renderer::drawWeapon()
{
	SpriteFrame* frame;
	uint8_t* data;
	
	engine.player.weapon.type = WeaponType_Pistol;

	switch(engine.player.weapon.type)
	{
	case WeaponType_Knife:
		frame = (SpriteFrame*) &Data_knifeSprite_frames[engine.player.weapon.frame];
		data = (uint8_t*) Data_knifeSprite;
		break;
	case WeaponType_Pistol:
		frame = (SpriteFrame*) &Data_pistolSprite_frames[engine.player.weapon.frame];
		data = (uint8_t*) Data_pistolSprite;
		break;
	case WeaponType_MachineGun:
		frame = (SpriteFrame*) &Data_machinegunSprite_frames[engine.player.weapon.frame];
		data = (uint8_t*) Data_machinegunSprite;
		break;
	}
	

	BitPairReader reader((uint8_t*)data, pgm_read_word(&frame->offset));
	uint8_t frameWidth = pgm_read_byte(&frame->width);
	uint8_t frameHeight = pgm_read_byte(&frame->height);
	uint8_t x = HALF_DISPLAYWIDTH - 8 + pgm_read_byte(&frame->xOffset);
	uint8_t mask = 1 << (x & 7);
	uint8_t y = HALF_DISPLAYHEIGHT - 1;
	uint8_t* overlayPtr = overlayBuffer + (y * DISPLAYWIDTH + x) / 8;

	for(int8_t i = 0; i < frameWidth; i++)
	{
		y = HALF_DISPLAYHEIGHT - 1;

		for(int8_t j = frameHeight - 1; j >= 0; j--)
		{
			uint8_t pixel = reader.read();

			if(pixel >= 2)
			{
				*overlayPtr |= mask;
			}
			y--;
			overlayPtr -= (DISPLAYWIDTH / 8);
		}

		x++;
		overlayPtr += frameHeight * (DISPLAYWIDTH / 8);

		mask <<= 1;
		if(!mask)
		{
			mask = 1;
			overlayPtr++;
		}
	}

/*	for(int n = 0; n < DISPLAYWIDTH * DISPLAYHEIGHT / 16; n++)
	{
		overlayBuffer[n] = 170;
	}*/
}

void Renderer::drawFrame()
{
	renderQueueHead = NULL_QUEUE_ITEM;
	for(int8_t n = 0; n < RENDER_QUEUE_CAPACITY; n++)
	{
		renderQueue[n].data = NULL;
	}

	view.x = engine.player.x;
	view.z = engine.player.z;
	view.rotCos = FixedMath::Cos(-engine.player.direction);
	view.rotSin = FixedMath::Sin(-engine.player.direction);
	view.clipCos = FixedMath::Cos(-engine.player.direction + DEGREES_90 / 2);
	view.clipSin = FixedMath::Sin(-engine.player.direction + DEGREES_90 / 2);

	view.cellX = WORLD_TO_CELL(engine.player.x);
	view.cellZ = WORLD_TO_CELL(engine.player.z);
	//initWBuffer();
	ClearVram();

	drawBufferedCells();
	drawDoors();


#if 0
	for(int8_t n = 0; n < MAX_ACTIVE_ACTORS; n++)
	{
		if(engine.actors[n].type != ActorType_Empty && !engine.actors[n].flags.frozen)
		{
			engine.actors[n].draw();
		}
	}

	for(int8_t n = 0; n < MAX_ACTIVE_ITEMS; n++)
	{
		if(engine.map.items[n].type != 0)
		{
			int16_t x = CELL_TO_WORLD(engine.map.items[n].x) + CELL_SIZE / 2, z = CELL_TO_WORLD(engine.map.items[n].z) + CELL_SIZE / 2;
			queueSprite((SpriteFrame*) &Data_itemSprites_frames[(engine.map.items[n].type - Tile_FirstItem)], (uint8_t*)Data_itemSprites, x, z);
		}
	}
#endif

#if 0
	for(uint8_t item = renderQueueHead; item != NULL_QUEUE_ITEM; item = renderQueue[item].next)
	{
		drawQueuedSprite(item);
	}

	drawWeapon();
	drawDamage();

	// Draw HUD
	uint8_t hudHeight = DISPLAYHEIGHT - FONT_HEIGHT;
	drawGlyph('+' - FIRST_FONT_GLYPH, 0, hudHeight);
	drawInt(engine.player.hp, (FONT_WIDTH + 1) * 3, hudHeight);
	drawGlyph('*' - FIRST_FONT_GLYPH, DISPLAYWIDTH - (FONT_WIDTH + 1) * 4, hudHeight);
	drawInt(engine.player.weapon.ammo, DISPLAYWIDTH - (FONT_WIDTH + 1), hudHeight);
	//drawString(PSTR("*99"), DISPLAYWIDTH - (FONT_WIDTH + 1) * 3, DISPLAYHEIGHT - FONT_HEIGHT);
	/*int y = 4;
	drawString(PSTR("* CAN I PLAY, DADDY?"), 0, y); y += FONT_HEIGHT + 1;
	drawString(PSTR("  DON'T HURT ME!"), 0, y); y += FONT_HEIGHT + 1;
	drawString(PSTR("  BRING 'EM ON!"), 0, y); y += FONT_HEIGHT + 1;
	drawString(PSTR("  I AM DEATH"), 0, y); y += FONT_HEIGHT + 1;
	drawString(PSTR("       INCARNATE!"), 0, y); y += FONT_HEIGHT + 1;*/
#endif
}

void Renderer::drawBufferedCells()
{
	int8_t xd, zd;
	int8_t x1, z1, x2, z2;

	if(view.rotCos > 0)
	{
		x1 = engine.map.bufferX;
		x2 = x1 + MAP_BUFFER_SIZE;
		xd = 1;
	}
	else
	{
		x2 = engine.map.bufferX - 1;
		x1 = x2 + MAP_BUFFER_SIZE;
		xd = -1;
	}
	if(view.rotSin < 0)
	{
		z1 = engine.map.bufferZ;
		z2 = z1 + MAP_BUFFER_SIZE;
		zd = 1;
	}
	else
	{
		z2 = engine.map.bufferZ - 1;
		z1 = z2 + MAP_BUFFER_SIZE;
		zd = -1;
	}

	if(mabs(view.rotCos) < mabs(view.rotSin))
	{
		for(int8_t z = z1; z != z2; z += zd)
		{
			for(int8_t x = x1; x != x2; x+= xd)
			{
				drawCell(x, z);
			}
		}
	}
	else
	{
		for(int8_t x = x1; x != x2; x+= xd)
		{
			for(int8_t z = z1; z != z2; z += zd)
			{
				drawCell(x, z);
			}
		}
	}
}

void Renderer::initWBuffer()
{
	for (uint8_t i=0; i<DISPLAYWIDTH; i++)
		displayBuffer[i] = 0;
}


void Renderer::drawCell(int8_t cellX, int8_t cellZ)
{
	// clip cells out of frustum view
	if(isFrustrumClipped(cellX, cellZ))
		return;

	uint8_t tile = engine.map.getTileFast(cellX, cellZ);
	if (tile == 0)
		return;

	int16_t worldX = CELL_TO_WORLD(cellX);
	int16_t worldZ = CELL_TO_WORLD(cellZ);

#if 0
	if(tile >= Tile_FirstDecoration && tile <= Tile_LastDecoration)
	{
		queueSprite((SpriteFrame*) &Data_decorations_frames[tile - Tile_FirstDecoration], (uint8_t*)Data_decorations, worldX + CELL_SIZE / 2, worldZ + CELL_SIZE / 2);
		return;
	}
	if(tile >= Tile_FirstBlockingDecoration && tile <= Tile_LastBlockingDecoration)
	{
		queueSprite((SpriteFrame*) &Data_blockingDecorations_frames[tile - Tile_FirstBlockingDecoration], (uint8_t*)Data_blockingDecorations, worldX + CELL_SIZE / 2, worldZ + CELL_SIZE / 2);
		return;
	}
#endif

	if(tile >= Tile_FirstWall && tile <= Tile_LastWall)
	{
		uint8_t textureId = tile - Tile_FirstWall; //engine.map.getTextureId(cellX, cellZ);

		if (view.z < worldZ)
		{
			if (view.x > worldX)
			{
				// north west quadrant
				if (view.z < worldZ)
				{
					if(!engine.map.isSolid(cellX, cellZ - 1))
						drawWall(worldX, worldZ, worldX + CELL_SIZE, worldZ);  // south wall door
				}
				if (view.x > worldX + CELL_SIZE)
				{
					if(!engine.map.isSolid(cellX+1, cellZ))
						drawWall(worldX + CELL_SIZE, worldZ, worldX + CELL_SIZE, worldZ + CELL_SIZE, ALT_WALL_COLOUR);  // east wall
				}
			}
			else
			{
				// north east quadrant
				if (view.z < worldZ)
				{
					if(!engine.map.isSolid(cellX, cellZ-1))
						drawWall(worldX, worldZ, worldX + CELL_SIZE, worldZ);  // south wall
				}
				if (view.x < worldX)
				{
					if(!engine.map.isSolid(cellX-1, cellZ))
						drawWall(worldX, worldZ + CELL_SIZE, worldX, worldZ, ALT_WALL_COLOUR);  // west wall
				}
			}
		}
		else
		{
			if (view.x > worldX)
			{
				// south west quadrant
				if (view.z > worldZ + CELL_SIZE)
				{
					if(!engine.map.isSolid(cellX, cellZ+1))
						drawWall(worldX + CELL_SIZE, worldZ + CELL_SIZE, worldX, worldZ + CELL_SIZE);  // north wall
				}
				if (view.x > worldX + CELL_SIZE)
				{
					if(!engine.map.isSolid(cellX+1, cellZ))
						drawWall(worldX + CELL_SIZE, worldZ, worldX + CELL_SIZE, worldZ + CELL_SIZE, ALT_WALL_COLOUR);  // east wall
				}
			}
			else
			{
				// south east quadrant
				if (view.z > worldZ + CELL_SIZE)
				{
					if(!engine.map.isSolid(cellX, cellZ+1))
						drawWall(worldX + CELL_SIZE, worldZ + CELL_SIZE, worldX, worldZ + CELL_SIZE);  // north wall
				}
				if (view.x < worldX)
				{
					if(!engine.map.isSolid(cellX-1, cellZ))
						drawWall(worldX, worldZ + CELL_SIZE, worldX, worldZ, ALT_WALL_COLOUR);  // west wall
				}
			}
		}
	}
}

// draws one side of a cell
void Renderer::drawWall(int16_t _x1, int16_t _z1, int16_t _x2, int16_t _z2, uint8_t wallColour)
{
	// find position of wall edges relative to eye

	int16_t z2 = (int16_t)(FIXED_TO_INT(view.rotCos * (int32_t)(_x1-view.x)) - FIXED_TO_INT(view.rotSin * (int32_t)(_z1-view.z)));
	int16_t x2 = (int16_t)(FIXED_TO_INT(view.rotSin * (int32_t)(_x1-view.x)) + FIXED_TO_INT(view.rotCos * (int32_t)(_z1-view.z)));
	int16_t z1 = (int16_t)(FIXED_TO_INT(view.rotCos * (int32_t)(_x2-view.x)) - FIXED_TO_INT(view.rotSin * (int32_t)(_z2-view.z)));
	int16_t x1 = (int16_t)(FIXED_TO_INT(view.rotSin * (int32_t)(_x2-view.x)) + FIXED_TO_INT(view.rotCos * (int32_t)(_z2-view.z)));

	// clip to the front pane
	if ((z1<CLIP_PLANE) && (z2<CLIP_PLANE))
		return;
	if (z1 < CLIP_PLANE)
	{
		x1 += (CLIP_PLANE-z1) * (x2-x1) / (z2-z1);
		z1 = CLIP_PLANE;
	}
	else if (z2 < CLIP_PLANE)
	{
		x2 += (CLIP_PLANE-z2) * (x1-x2) / (z1-z2);
		z2 = CLIP_PLANE;
	}

	// apply perspective projection
	int16_t vx1 = (int16_t)(x1 * NEAR_PLANE * CAMERA_SCALE / z1);  
	int16_t vx2 = (int16_t)(x2 * NEAR_PLANE * CAMERA_SCALE / z2); 

	// transform the end points into screen space
	int16_t sx1 = (int16_t)((DISPLAYWIDTH / 2) + vx1);
	int16_t sx2 = (int16_t)((DISPLAYWIDTH / 2) + vx2) - 1;

	// clamp to the visible portion of the screen
	int16_t firstx = max(sx1, 0);
	int16_t lastx = min(sx2, DISPLAYWIDTH-1);
	if (lastx < firstx)
		return;

	int16_t w1 = (int16_t)((CELL_SIZE * NEAR_PLANE * CAMERA_SCALE) / z1);
	int16_t w2 = (int16_t)((CELL_SIZE * NEAR_PLANE * CAMERA_SCALE) / z2);
	int16_t dx = sx2 - sx1;
	int16_t werror = dx >> 1;
	int16_t w = w1;
	int16_t dw, wstep;

	if(w1 < w2)
	{
		dw = w2 - w1;
		wstep = 1;
	}
	else
	{
		dw = w1 - w2;
		wstep = -1;
	}

	for (int16_t x=sx1; x<=sx2; x++)
	{
		if(x == sx2)
			wallColour ^= ALT_WALL_COLOUR;

		uint8_t wallHeight = min(w, HALF_DISPLAYHEIGHT);

		if (x >= 0 && x < DISPLAYWIDTH && wallHeight > (displayBuffer[x] & 0x7F))
		{        
			displayBuffer[x] = wallHeight | wallColour;
		}

		werror -= dw;

		if(dx > 0)
		{
			while(werror < 0)
			{
				w += wstep;
				werror += dx;
			}
		}
	}
}

void Renderer::drawDoors()
{
	for(int n = 0; n < MAX_DOORS; n++)
	{
		Door& door = engine.map.doors[n];
		uint8_t textureId = door.texture;

		if(!engine.map.isValid(door.x, door.z))
		{
			continue;
		}
		
		if(door.type == DoorType_SecretPushWall)
		{
			int16_t doorX = CELL_TO_WORLD(door.x);
			int16_t doorZ = CELL_TO_WORLD(door.z);

			switch(door.state)
			{
			case DoorState_PushNorth:
				doorZ -= door.open;
				break;
			case DoorState_PushEast:
				doorX += door.open;
				break;
			case DoorState_PushSouth:
				doorZ += door.open;
				break;
			case DoorState_PushWest:
				doorX -= door.open;
				break;
			}

			if(view.x < doorX)
			{
				drawWall(doorX, doorZ + CELL_SIZE, doorX, doorZ);
			}
			else if(view.x > doorX)
			{
				drawWall(doorX + CELL_SIZE, doorZ, doorX + CELL_SIZE, doorZ + CELL_SIZE);
			}
			if(view.z > doorZ + CELL_SIZE)
			{
				drawWall(doorX + CELL_SIZE, doorZ + CELL_SIZE, doorX, doorZ + CELL_SIZE);
			}
			else if(view.z < doorZ)
			{
				drawWall(doorX, doorZ, doorX + CELL_SIZE, doorZ);
			}
		}
		else
		{
			int offset = door.open;
			if(offset >= 16)
			{
				continue;
			}

			int16_t worldX = CELL_TO_WORLD(door.x);
			int16_t worldZ = CELL_TO_WORLD(door.z);

			if((door.type & 0x1) == 0)
			{
				worldX += CELL_SIZE / 2;
				if(view.x < worldX)
				{
					drawWall(worldX, worldZ + CELL_SIZE, 
						worldX, worldZ + offset * 2);
				}
				else
				{
					drawWall(worldX, worldZ + offset * 2, 
						worldX, worldZ + CELL_SIZE);
				}
			}
			else
			{
				worldZ += CELL_SIZE / 2;
				if(view.z > worldZ)
				{
					drawWall(worldX + CELL_SIZE, worldZ, 
						worldX + offset * 2, worldZ);
				}
				else
				{
					drawWall(worldX + offset * 2, worldZ, 
						worldX + CELL_SIZE, worldZ);
				}
			}
		}
	}
}

void Renderer::queueSprite(SpriteFrame* frame, uint8_t* spriteData, int16_t _x, int16_t _z)
{
#if 0
	int cellX = WORLD_TO_CELL(_x);
	int cellZ = WORLD_TO_CELL(_z);

	if(isFrustrumClipped(cellX, cellZ))
		return;

	int16_t zt = (int16_t)(FIXED_TO_INT(view.rotCos * (int32_t)(_x-view.x)) - FIXED_TO_INT(view.rotSin * (int32_t)(_z-view.z)));
	int16_t xt = (int16_t)(FIXED_TO_INT(view.rotSin * (int32_t)(_x-view.x)) + FIXED_TO_INT(view.rotCos * (int32_t)(_z-view.z)));

	// clip to the front plane
	if (zt < CLIP_PLANE)
		return;

	// apply perspective projection
	int16_t vx = (int16_t)(xt * NEAR_PLANE * CAMERA_SCALE / zt);  

	if(vx <= -DISPLAYWIDTH || vx >= DISPLAYWIDTH)
		return;

	int16_t w = (int16_t)((CELL_SIZE * NEAR_PLANE * CAMERA_SCALE) / zt);
	int16_t x = vx + HALF_DISPLAYWIDTH;

	if(w > 255)
		w = 255;

	// TODO: cull if off screen (x)
	uint8_t newItem = NULL_QUEUE_ITEM;
	for(int n = 0; n < RENDER_QUEUE_CAPACITY; n++)
	{
		if(renderQueue[n].data == NULL)
		{
			newItem = n;
			break;
		}
	}

	if(newItem == NULL_QUEUE_ITEM)
	{
		if(w > renderQueue[renderQueueHead].w)
		{
			newItem = renderQueueHead;
			renderQueueHead = renderQueue[renderQueueHead].next;
		}
		else
		{
			//WARNING("Out of queue space!\n");
			return;
		}
	}

	renderQueue[newItem].x = x;
	renderQueue[newItem].w = w;
	renderQueue[newItem].frame = frame;
	renderQueue[newItem].data = spriteData;

	if(renderQueueHead == NULL_QUEUE_ITEM)
	{
		renderQueueHead = newItem;
		renderQueue[newItem].next = NULL_QUEUE_ITEM;
		return;
	}
	else
	{
		if(w < renderQueue[renderQueueHead].w)
		{
			renderQueue[newItem].next = renderQueueHead;
			renderQueueHead = newItem;
		}
		else
		{
			for(uint8_t item = renderQueueHead; item != NULL_QUEUE_ITEM; item = renderQueue[item].next)
			{
				if(renderQueue[item].next == NULL_QUEUE_ITEM)
				{
					renderQueue[item].next = newItem;
					renderQueue[newItem].next = NULL_QUEUE_ITEM;
					break;
				}
				else if(w < renderQueue[renderQueue[item].next].w)
				{
					renderQueue[newItem].next = renderQueue[item].next;
					renderQueue[item].next = newItem;
					break;
				}
			}
		}
	}
#endif
}

void Renderer::drawQueuedSprite(uint8_t id)
{
#if 0
	uint8_t frameWidth = pgm_read_byte(&renderQueue[id].frame->width);
	uint8_t frameHeight = pgm_read_byte(&renderQueue[id].frame->height);
	int16_t halfW = renderQueue[id].w >> 1;
	int y2 = (HALF_DISPLAYHEIGHT) + halfW;
	int y1 = y2 - (renderQueue[id].w * frameHeight) / (CELL_SIZE / 2);

	int16_t w = renderQueue[id].w;

	int16_t dx = (w * frameWidth) / (CELL_SIZE / 2);

	int16_t sx1 = renderQueue[id].x - halfW + (w * pgm_read_byte(&renderQueue[id].frame->xOffset)) / (CELL_SIZE / 2);
	int16_t sx2 = sx1 + dx;
	int16_t uerror = dx;
	int8_t u = 0;
	int8_t du = frameWidth, ustep = 1;
	int8_t v;

	for (int x = sx1; x <= sx2; x++)
	{
		if (x >= 0 && x < DISPLAYWIDTH && w > wbuffer[x])
		{        
			int verror = halfW;

			BitPairReader textureReader((uint8_t*) renderQueue[id].data, pgm_read_word(&renderQueue[id].frame->offset) + frameHeight * u);
			uint8_t texData = textureReader.read();

			v = 0;

			for(int y = y2; y >= y1 && y >= 0 && v < frameHeight; y--)
			{
				if(y < DISPLAYHEIGHT)
				{
					switch(texData)
					{
					case 0:
						break;
					case 1:
						clearPixel(x, y);
						break;
					case 2:
						setPixel(x, y);
						break;
					case 3:
#if defined(EMULATE_UZEBOX)
						drawPixel(x, y, 2);
#else
						if((x ^ y) & 1)
						{
							clearPixel(x, y);
						}
						else
						{
							setPixel(x, y);
						}
#endif
						break;
					}
				}
				verror -= 15;

				while(verror < 0)
				{
					texData = textureReader.read();
					verror += w;
					v++;
				}

			}
		}

		uerror -= du;

		if(dx > 0)
		{
			while(u < frameWidth - 1 && uerror < 0)
			{
				u += ustep;
				uerror += dx;
			}
		}
	}
#endif
}

void Renderer::drawGlyph(char glyph, uint8_t x, uint8_t y)
{
#if 0
	uint8_t* ptr = (uint8_t*) (Data_font + glyph * FONT_GLYPH_BYTE_SIZE);
	uint8_t readMask = 1;
	uint8_t read = pgm_read_byte(ptr++);

	for(int i = 0; i < FONT_WIDTH; i++)
	{
		for(int j = 0; j < FONT_HEIGHT; j++)
		{
			uint8_t colour = (read & readMask) ? 0 : 1;
			drawPixel(x + i, y + j, colour);
			readMask <<= 1;
			if(readMask == 0)
			{
				readMask = 1;
				read = pgm_read_byte(ptr++);
			}
		}
//		clearPixel(x + i, y);
	//	clearPixel(x + i, y + FONT_HEIGHT + 1);
	}
	for(int j = 0; j < FONT_HEIGHT; j++)
	{
		clearPixel(x + FONT_WIDTH, y + j);
	}
#endif
}

void Renderer::drawString(const char* str, uint8_t x, uint8_t y)
{
#if 0
	char* ptr = (char*) str;
	char current = 0;
	uint8_t startX = x;

	do
	{
		current = pgm_read_byte(ptr);
		ptr++;

		if(current >= FIRST_FONT_GLYPH && current <= LAST_FONT_GLYPH)
		{
			drawGlyph(current - FIRST_FONT_GLYPH, x, y);
		}

		x += FONT_WIDTH + 1;

		if(current == '\n')
		{
			x = startX;
			y += FONT_HEIGHT + 1;
		}
	} while(current);
#endif
}

void Renderer::drawInt(int8_t val, uint8_t x, uint8_t y)
{
#if 0
	unsigned char c, i;

	for(i = 0; i < 3; i++)
	{
		c = val % 10;
		if(val > 0 || i == 0) 
		{
			drawGlyph(c + '0' - FIRST_FONT_GLYPH, x, y);
		}
		else
		{
			drawGlyph(' ' - FIRST_FONT_GLYPH, x, y);
		}
		x -= FONT_WIDTH + 1;
		val = val / 10;
	}
#endif
}

