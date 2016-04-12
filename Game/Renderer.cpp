//#include "Platform.h"
#include "Engine.h"
#include "Renderer.h"
#include "FixedMath.h"
#include "TileTypes.h"

//#include "Data_Pistol.h"
#include "Data_Shotgun.h"
#include "Data_Marine.h"
//#include "Data_Knife.h"
//#include "Data_Machinegun.h"
#if 0
#include "Data_Walls.h"
#include "Data_Decorations.h"
#include "Data_BlockingDecorations.h"
#include "Data_Items.h"
#include "Data_Font.h"
#endif

#if USE_TEXTURE_MAPPING
#include "Data_Textures.h"
#include "W2Z.h"
#endif

#include <stdio.h>

/*uint8_t LevelColours[] PROGMEM =
{
	UZE_RGB(127, 127, 127),		// Ceiling
	UZE_RGB(64, 64, 64),		// Floor
	UZE_RGB(32, 32, 32),		// Top wall edge
	UZE_RGB(32, 32, 32),		// Bottom wall edge
	UZE_RGB(192, 192, 192),		// Wall
	UZE_RGB(255, 255, 255),		// Alt wall
};
*/

#if ENABLE_FOG
uint8_t FogBands[] =
{
	//0, 0, 0
	DISPLAYHEIGHT / 4,
	DISPLAYHEIGHT / 8,
	DISPLAYHEIGHT / 16
};
#endif

uint8_t PaletteColours[] PROGMEM =
{
	// White / grey
	RGB332(7, 7, 3), RGB332(5, 5, 2), RGB332(2, 2, 1),
	RGB332(6, 6, 3), RGB332(4, 4, 2), RGB332(2, 2, 1),
	RGB332(5, 5, 2), RGB332(3, 3, 1), RGB332(1, 1, 0),

	// Lime green
	RGB332(6, 7, 0), RGB332(4, 5, 0), RGB332(1, 2, 0),
	RGB332(5, 6, 0), RGB332(3, 4, 0), RGB332(1, 2, 0),
	RGB332(4, 5, 0), RGB332(2, 3, 0), RGB332(0, 1, 0),

};

uint8_t TextureColours[] PROGMEM =
{
#if 0
	UZE_RGB(255, 255, 255), UZE_RGB(255, 255, 255),
#else
	UZE_RGB(140, 140, 140), UZE_RGB(100, 100, 100),
	UZE_RGB(140, 140, 140), UZE_RGB(100, 100, 100),
	UZE_RGB(188, 0, 0), UZE_RGB(152, 0, 0),
	UZE_RGB(180, 112, 68), UZE_RGB(156, 96, 56),
	UZE_RGB(0, 108, 163), UZE_RGB(0, 49, 73),
	UZE_RGB(140, 140, 140), UZE_RGB(100, 100, 100),
	UZE_RGB(0, 108, 163), UZE_RGB(0, 49, 73),
	UZE_RGB(0, 108, 163), UZE_RGB(0, 49, 73),
	UZE_RGB(0, 108, 163), UZE_RGB(0, 49, 73),
	UZE_RGB(116, 76, 40), UZE_RGB(72, 56, 24),
	UZE_RGB(116, 76, 40), UZE_RGB(72, 56, 24),
	UZE_RGB(116, 76, 40), UZE_RGB(72, 56, 24),
#endif
};

#if USE_TEXTURE_MAPPING
void Renderer::updateTextureBank()
{
	uint8_t* bankPtr = textureBank;
	uint8_t* resPtr = (uint8_t*) Data_textures;
	uint8_t floorColour = RGB332(2, 2, 1);
	uint8_t ceilingColour = RGB332(1, 1, 1);

	for(int n = 0; n < 25; n++)
	{
		*bankPtr++ = ceilingColour;
		for(int v = 0; v < TEXTURE_SIZE; v++)
		{
			uint8_t texel = pgm_read_byte(resPtr);
			resPtr++;
			*bankPtr++ = texel;
		}
		*bankPtr++ = floorColour;
	}
}
#endif

void Renderer::init()
{
	//updateLevelColours(LevelColours);
	overlayColour = RGB332(0, 0, 0);

	targetDisplayBuffer = displayBuffer;
	targetOverlayBuffer = overlayBuffer;
	currentBuffer = 1;

	drawWeapon();
	flipBuffers();
	drawWeapon();

	int floorPaletteColour = 15;
	uint8_t skyColour = RGB332(1, 1, 1);

#if USE_TEXTURE_MAPPING
	updateTextureBank();
#else
	for(int n = 0; n < DISPLAYHEIGHT; n++)
	{
		if(n < DISPLAYHEIGHT / 2)
		{
			outerColours[n] = skyColour;
		}
		else
		{
#if ENABLE_FOG
			int height = n - DISPLAYHEIGHT / 2;
			if(height < FogBands[2])
			{
				outerColours[n] = RGB332(0, 0, 0);
			}
			else if(height < FogBands[1])
			{
				outerColours[n] = pgm_read_byte(&PaletteColours[floorPaletteColour + 2]);
			}
			else if(height < FogBands[0])
			{
				outerColours[n] = pgm_read_byte(&PaletteColours[floorPaletteColour + 1]);
			}
			else
#endif
			{
				outerColours[n] = pgm_read_byte(&PaletteColours[floorPaletteColour]);
			}
		}
	}
#endif
}

/*
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
*/

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

	/*switch(engine.player.weapon.type)
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
	}*/
	
	frame = (SpriteFrame*) &Data_shotgunSprite_frames[engine.player.weapon.frame];
	data = (uint8_t*) Data_shotgunSprite;

	BitPairReader reader((uint8_t*)data, pgm_read_word(&frame->offset));
	uint8_t frameWidth = pgm_read_byte(&frame->width);
	uint8_t frameHeight = pgm_read_byte(&frame->height);
	uint8_t x = HALF_DISPLAYWIDTH - 16 + pgm_read_byte(&frame->xOffset);
	uint8_t mask = 1 << (x & 7);
	uint8_t y = HALF_DISPLAYHEIGHT - 1;
	uint8_t* overlayPtr = targetOverlayBuffer + (y * DISPLAYWIDTH + x) / 8;

	for(int8_t i = 0; i < frameWidth; i++)
	{
		y = HALF_DISPLAYHEIGHT - 1;

		for(int8_t j = frameHeight - 1; j >= 0; j--)
		{
			uint8_t pixel = reader.read();

			if(pixel == 2)
			{
				*overlayPtr |= mask;
			}
			else if(pixel == 1)
			{
				*overlayPtr &= ~mask;
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

	initWBuffer();
	//ClearVram();

#if 1
	drawWall(32, -32, -32, -32, 1, 0, 8);
	drawWall(-32, 32, 32, 32, 2, 0, 8);
	drawWall(32, 32, 32, -32, 3, 0, 8);
	drawWall(-32, -32, -32, 32, 4, 0, 8);

/*	drawWall(256, 256, -256, 256, 1, 0, 64);
	drawWall(-256, -256, 256, -256, 0, 0, 64);
	drawWall(256, -256, 256, 256, 1, 0, 64);
	drawWall(-256, 256, -256, -256, 0, 0, 64);*/

	drawWall(0, -256, 256, 0, 1, 0, 45);
	drawWall(256, 0, 0, 256, 0, 0, 45);
	drawWall(0, 256, -256, 0, 1, 0, 45);
	drawWall(-256, 0, 0, -256, 0, 0, 45);

	
	/*drawWall(-300, -500, 300, -500, 0, 0, 75);
	drawWall(300, -500, 500, -300, 6, 0, 75);
	drawWall(500, -300, 500, 300, 7, 0, 75);
	drawWall(500, 300, 300, 500, 8, 0, 75);
	drawWall(300, 500, -300, 500, 0, 0, 75);
	drawWall(-300, 500, -500, 300, 6, 0, 75);
	//drawWall(-500, 300, -300, -500, 0, 0, 75);
	drawWall(-128, -300, 128, -300, 7, 0, 32);*/
/*	drawWall(-500, -500, 500, -500, 6);
	drawWall(500, 500, -500, 500, 6);
	drawWall(500, -500, 500, 500, 3);
	drawWall(-500, 500, -500, -500, 3);
	*/
#else
	drawWall(500, -500, 500, 500, 6);
	drawWall(-50, -50, -50, 50, 3);
#endif

	queueSprite(&Data_marineSprite_frames[0], Data_marineSprite, -100, -100);
	queueSprite(&Data_marineSprite_frames[0], Data_marineSprite, 100, 100);
	queueSprite(&Data_marineSprite_frames[0], Data_marineSprite, -100, 100);

	clearOverlay();
	for(uint8_t item = renderQueueHead; item != NULL_QUEUE_ITEM; item = renderQueue[item].next)
	{
		drawQueuedSprite(item);
	}

	drawWeapon();

	flipBuffers();
}

void Renderer::flipBuffers()
{
#if ENABLE_DOUBLE_BUFFER
	targetDisplayBuffer = currentBuffer == 0 ? displayBuffer : displayBuffer + (DISPLAYWIDTH * 2);
	targetOverlayBuffer = currentBuffer == 0 ? overlayBuffer : overlayBuffer + (DISPLAYWIDTH * DISPLAYHEIGHT / 16);
	currentBuffer = !currentBuffer;
#else
	currentBuffer = 0;
	targetDisplayBuffer = displayBuffer;
	targetOverlayBuffer = overlayBuffer;
#endif
}

void Renderer::initWBuffer()
{
	for (int i=0; i<DISPLAYWIDTH * 2; i++)
		targetDisplayBuffer[i] = 0;
}

#define TEXTURE_COORD_SHIFT 9
#define TEXTURE_COORD_MULTIPLIER (1 << TEXTURE_COORD_SHIFT)

#if USE_TEXTURE_MAPPING
void Renderer::drawWall(int16_t _x2, int16_t _z2, int16_t _x1, int16_t _z1, uint8_t texture, uint8_t _u1, uint8_t _u2)
#else
void Renderer::drawWall(int16_t _x2, int16_t _z2, int16_t _x1, int16_t _z1, uint8_t paletteColour)
#endif
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
		x1 += (int16_t) ( ((int32_t)(CLIP_PLANE-z1) * (int32_t)(x2-x1)) / (z2-z1) );
		_u1 += (int16_t) ( ((int32_t)(CLIP_PLANE-z1) * (int32_t)(_u2-_u1)) / (z2-z1) );
		z1 = CLIP_PLANE;
	}
	else if (z2 < CLIP_PLANE)
	{
		x2 += (int16_t) ( ((int32_t)(CLIP_PLANE-z2) * (int32_t)(x1-x2)) / (z1-z2) );
		_u2 += (int16_t) ( ((int32_t)(CLIP_PLANE-z2) * (int32_t)(_u1-_u2)) / (z1-z2) );
		z2 = CLIP_PLANE;
	}
	
	// apply perspective projection
	int16_t vx1 = (int16_t)(((int32_t)x1 * NEAR_PLANE * CAMERA_SCALE) / z1);  
	int16_t vx2 = (int16_t)(((int32_t)x2 * NEAR_PLANE * CAMERA_SCALE) / z2); 

	// transform the end points into screen space
	int16_t sx1 = (int16_t)((DISPLAYWIDTH / 2) + vx1);
	int16_t sx2 = (int16_t)((DISPLAYWIDTH / 2) + vx2) - 1;

	// cull
	if(sx2 < 0 || sx1 >= DISPLAYWIDTH || sx1 > sx2)
		return;

	int16_t w1 = (int16_t)((Z_SCALE_MULTIPLIER) / z1);
	int16_t w2 = (int16_t)((Z_SCALE_MULTIPLIER) / z2);

	int16_t u1 = (_u1 * TEXTURE_COORD_MULTIPLIER) / z1;
	int16_t u2 = (_u2 * TEXTURE_COORD_MULTIPLIER) / z2;
		
	// clamp to the visible portion of the screen
	// TODO: not rely on int32 precision
#if 1
	if(sx1 < 0)
	{
		w1 = w1 + ((int32_t)(w2 - w1) * (int32_t)(0 - sx1)) / (int32_t)(sx2 - sx1);
		u1 = u1 + ((int32_t)(u2 - u1) * (int32_t)(0 - sx1)) / (int32_t)(sx2 - sx1);

		sx1 = 0;
	}

	if(sx2 > DISPLAYWIDTH - 1)
	{
		w2 = w1 + ((int32_t)(w2 - w1) * (int32_t)((DISPLAYWIDTH - 1) - sx1)) / (int32_t)(sx2 - sx1);
		u2 = u1 + ((int32_t)(u2 - u1) * (int32_t)((DISPLAYWIDTH - 1) - sx1)) / (int32_t)(sx2 - sx1);
		sx2 = DISPLAYWIDTH - 1;
	}
#endif
	
	int16_t dx = sx2 - sx1;
	int16_t werror = dx >> 1;
	int16_t uerror = werror;
	int16_t w = w1;
	int16_t dw, wstep;
	int16_t du, ustep;
	int16_t u = u1;

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

	if(u1 < u2)
	{
		du = u2 - u1;
		ustep = 1;
	}
	else
	{
		du = u1 - u2;
		ustep = -1;
	}

	for (int16_t x=sx1; x<=sx2; x++)
	{
//		uint8_t wallHeight = min(w, HALF_DISPLAYHEIGHT);
		uint8_t wallHeight = min(w, MAX_WALL_BUFFER_HEIGHT);

//		if(x >= 0 && x < DISPLAYWIDTH - 1)
		if (wallHeight > targetDisplayBuffer[x * 2])
		{
			targetDisplayBuffer[x * 2] = wallHeight;
#if USE_TEXTURE_MAPPING
			/*float u = ((float)x - sx1) / (float)(sx2 - sx1);
			uint8_t texCoord = (uint8_t) (u * 8);
			texCoord = texCoord & 0x7;
			*/
			
			//float t = ((float)x - sx1) / (float)(sx2 - sx1);
			//float u1OverZ1 = (float)_u1 / (float)z1;
			//float u2OverZ2 = (float)_u2 / (float)z2;
			//float outU = u1OverZ1 + t * (u2OverZ2 - u1OverZ1);
			//float outU = (float)u / TEXTURE_COORD_MULTIPLIER;
			//float outZ = ((CELL_SIZE * NEAR_PLANE * CAMERA_SCALE) / (float)w); // put this in a lut
			//uint8_t texcoord = (uint8_t)(outU * outZ);
			int16_t outZ = pgm_read_word(&W2Z[w]);
			uint8_t texcoord = ((int16_t)(u * outZ)) >> (TEXTURE_COORD_SHIFT);
			targetDisplayBuffer[x * 2 + 1] = pgm_read_byte(&Data_textures_indices[texture * TEXTURE_SIZE + (texcoord & 7)]);
#else
#if ENABLE_FOG
			if(wallHeight < FogBands[2])
			{
				targetDisplayBuffer[x * 2 + 1] = RGB332(0, 0, 0);
			}
			else if(wallHeight < FogBands[1])
			{
				targetDisplayBuffer[x * 2 + 1] = pgm_read_byte(&PaletteColours[paletteColour + 2]);
			}
			else if(wallHeight < FogBands[0])
			{
				targetDisplayBuffer[x * 2 + 1] = pgm_read_byte(&PaletteColours[paletteColour + 1]);
			}
			else
#endif
			{
				targetDisplayBuffer[x * 2 + 1] = pgm_read_byte(&PaletteColours[paletteColour]);
			}
#endif
		}

		werror -= dw;
		uerror -= du;

		if(dx > 0)
		{
			while(werror < 0)
			{
				w += wstep;
				werror += dx;
			}
			while(uerror < 0)
			{
				u += ustep;
				uerror += dx;
			}
		}
	}
}

void Renderer::queueSprite(const SpriteFrame* frame, const uint8_t* spriteData, int16_t _x, int16_t _z)
{
	//if(isFrustrumClipped(_x, _z))
		//return;

	int16_t zt = (int16_t)(FIXED_TO_INT(view.rotCos * (int32_t)(_x-view.x)) - FIXED_TO_INT(view.rotSin * (int32_t)(_z-view.z)));
	int16_t xt = (int16_t)(FIXED_TO_INT(view.rotSin * (int32_t)(_x-view.x)) + FIXED_TO_INT(view.rotCos * (int32_t)(_z-view.z)));

	// clip to the front plane
	if (zt < CLIP_PLANE)
		return;

	// apply perspective projection
	int16_t vx = (int16_t)((int32_t)xt * NEAR_PLANE * CAMERA_SCALE / zt);  

	if(vx <= -DISPLAYWIDTH || vx >= DISPLAYWIDTH)
		return;

	int16_t w = (int16_t)((Z_SCALE_MULTIPLIER) / zt);
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
}

void Renderer::clearOverlay()
{
	uint8_t* ptr = targetOverlayBuffer;
	int count = DISPLAYWIDTH * DISPLAYHEIGHT / 16;

	while(count--)
	{
		*ptr++ = 0;
	}
}

void Renderer::clearPixel(uint8_t x, uint8_t y)
{
	if(x >= DISPLAYWIDTH || y >= DISPLAYHEIGHT / 2)
		return;

	uint8_t* ptr = targetOverlayBuffer + ((y * DISPLAYWIDTH + x) >> 3);
	uint8_t mask = ~(1 << (x & 7));
	*ptr &= mask;
}

void Renderer::setPixel(uint8_t x, uint8_t y)
{
	if(x >= DISPLAYWIDTH || y >= DISPLAYHEIGHT / 2)
		return;

	uint8_t* ptr = targetOverlayBuffer + ((y * DISPLAYWIDTH + x) >> 3);
	uint8_t mask = (1 << (x & 7));
	*ptr |= mask;
}

void Renderer::drawQueuedSprite(uint8_t id)
{
	uint8_t frameWidth = pgm_read_byte(&renderQueue[id].frame->width);
	uint8_t frameHeight = pgm_read_byte(&renderQueue[id].frame->height);
	int16_t halfW = renderQueue[id].w >> 1;
	int y2 = (HALF_DISPLAYHEIGHT / 2) + halfW;
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
		if (x >= 0 && x < DISPLAYWIDTH && w > targetDisplayBuffer[x << 1])
		{        
			int verror = halfW;

			BitPairReader textureReader((uint8_t*) renderQueue[id].data, pgm_read_word(&renderQueue[id].frame->offset) + frameHeight * u);
			uint8_t texData = textureReader.read();

			v = 0;

			for(int y = y2; y >= y1 && y >= 0 && v < frameHeight; y--)
			{
				if(y < DISPLAYHEIGHT / 2)
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
						if((x ^ y) & 1)
						{
							clearPixel(x, y);
						}
						else
						{
							setPixel(x, y);
						}
						break;
					}
				}
				verror -= 31;

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

