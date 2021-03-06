#ifndef RENDERER_H_
#define RENDERER_H_

#include <stdint.h>
#include "Engine.h"
#include "Defines.h"
#include "FixedMath.h"
#include "SpriteFrame.h"

#define NULL_QUEUE_ITEM 0xff
#define RENDER_QUEUE_CAPACITY 8
//#define ALT_WALL_COLOUR (1 << 7)
#define ALT_WALL_COLOUR UZE_RGB(255, 255, 255)

struct RenderQueueItem
{
	const SpriteFrame* frame;
	const uint8_t* data;
	int16_t x;
	uint8_t w;
	uint8_t next;
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

class Renderer
{
public:
	void init();
	void drawFrame();
	void queueSprite(const SpriteFrame* frame, const uint8_t* sprite, int16_t x, int16_t z);

	void drawGlyph(char glyph, uint8_t x, uint8_t y);
	void drawString(const char* str, uint8_t x, uint8_t y);
	void drawInt(int8_t val, uint8_t x, uint8_t y);


#ifdef DEFER_RENDER
	void drawDeferredFrame();
#endif

	int8_t damageIndicator;

	//void updateLevelColours(uint8_t* colours);

	inline bool isFrustrumClipped(int16_t x, int16_t z)
	{
		if((view.clipCos * (x - view.x) - view.clipSin * (z - view.z)) < -FIXED_ONE)
			return true;
		if((view.clipSin * (x - view.x) + view.clipCos * (z - view.z)) < -FIXED_ONE)
			return true;

		return false;
	}

private:
	void initWBuffer();
	void drawFloorAndCeiling();  

#if USE_TEXTURE_MAPPING
	void drawWall(int16_t _x2, int16_t _z2, int16_t _x1, int16_t _z1, uint8_t texture, uint8_t texcoord1, uint8_t texcoord2);
#else
	void drawWall(int16_t _x1, int16_t _z1, int16_t _x2, int16_t _z2, uint8_t wallColour = UZE_RGB(192, 192, 192));
#endif

	void drawQueuedSprite(uint8_t id);
	void drawWeapon();
	void drawDamage();
	void flipBuffers();

	void clearPixel(uint8_t x, uint8_t y);
	void setPixel(uint8_t x, uint8_t y);
	void clearOverlay();

#if USE_TEXTURE_MAPPING
	void updateTextureBank();
#endif

	struct 
	{
		int16_t x, z;
		int16_t rotCos, rotSin;
		int16_t clipCos, clipSin;
	} view;

	uint8_t renderQueueHead;
	RenderQueueItem renderQueue[RENDER_QUEUE_CAPACITY];

	uint8_t* targetDisplayBuffer;
	uint8_t* targetOverlayBuffer;
};

class BitPairReader
{
public:
	BitPairReader(uint8_t* ptr, uint16_t offset = 0) 
	{
		uint16_t byteOffset = offset >> 2;
		m_readOffset = (offset - (byteOffset << 2)) << 1;
		m_ptr = ptr + byteOffset;
		m_lastRead = pgm_read_byte(m_ptr);
	}
	
	uint8_t read()
	{
		uint8_t result = (m_lastRead & (3 << m_readOffset)) >> m_readOffset;
		m_readOffset += 2;
		if(m_readOffset == 8)
		{
			m_ptr++;
			m_lastRead = pgm_read_byte(m_ptr);
			m_readOffset = 0;
		}

		return result;
	}
private:
	uint8_t* m_ptr;
	uint8_t m_lastRead;
	uint8_t m_readOffset;
};


#endif
