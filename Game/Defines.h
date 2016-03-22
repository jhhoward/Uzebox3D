#ifndef DEFINES_H_
#define DEFINES_H_

#define USE_SIMPLE_COLLISIONS

#define DISPLAYWIDTH 144
#define DISPLAYHEIGHT 128

#define HALF_DISPLAYWIDTH (DISPLAYWIDTH >> 1)
#define HALF_DISPLAYHEIGHT (DISPLAYHEIGHT >> 1)

// bbgggrrr
#define UZE_RGB(r, g, b) ((r >> 5) | ((g >> 5) << 3) | ((b >> 6) << 6))
#define RGB332(r, g, b) ( ((r) & 7) | (((g) & 7) << 3) | (((b) & 3) << 6) )

#ifndef NULL
#define NULL 0
#endif

#define ENABLE_FOG 0

// WIN32 specific
#ifdef _WIN32
#define ZOOM_SCALE 1
#define ASPECT_RATIO 2

#define PROGMEM
#define PSTR
#define pgm_read_byte(x) (*((uint8_t*)x))
#define pgm_read_word(x) (*((uint16_t*)x))

#define pgm_read_ptr(x) (*((uintptr_t*)x))

#include <stdio.h>
#define WARNING(msg, ...) printf((msg), __VA_ARGS__)
#define ERROR(msg) printf(msg)
#else
#define WARNING(msg, ...)
#define pgm_read_ptr(x) pgm_read_word(x)

#endif
// end

#if !defined(max) && !defined(min)
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define min3(a, b, c) (min(min(a, b), c))
#define max3(a, b, c) (max(max(a, b), c))
#define sign(x) ((x) < 0 ? -1 : 1)
#define mabs(x) ((x) < 0 ? -(x) : (x))

#define CELL_SIZE_SHIFT 6
#define CELL_SIZE (1 << CELL_SIZE_SHIFT)

#if 0
#define CELL_TO_WORLD(x) ((x) * CELL_SIZE)
#define WORLD_TO_CELL(x) ((x) / CELL_SIZE)
#else
#define CELL_TO_WORLD(x) ((x) << CELL_SIZE_SHIFT)
#define WORLD_TO_CELL(x) ((x) >> CELL_SIZE_SHIFT)
#endif

#define TEXTURE_SIZE 16
#define TEXTURE_STRIDE 4

// ~60 degree field of view (these values in wacky 256 unit format)
#define FOV 44
#define HALF_FOV 22
#define CULLING_FOV 35
#define DRAW_DISTANCE (CELL_SIZE * 20)

//#define NEAR_PLANE 73
//#define NEAR_PLANE 104
//#define NEAR_PLANE (LCDWIDTH * (0.5/tan(PI*(FOV / 2)/180)))
#define NEAR_PLANE_MULTIPLIER 222
#define NEAR_PLANE (DISPLAYWIDTH * NEAR_PLANE_MULTIPLIER / 256)

#define CLIP_PLANE 20
#define CAMERA_SCALE 1
//#define WALL_HEIGHT 1.0f
#define MOVEMENT 7
//#define TURN 1 //3
#define TURN 2
#define MIN_WALL_DISTANCE 8
#define MAX_DOORS 12

#define DOOR_FRAME_TEXTURE 19

#define MAX_ACTIVE_ACTORS 2 // 5
#define MAX_ACTIVE_ITEMS 10

#define EMPTY_ITEM_SLOT 0xff
#define DYNAMIC_ITEM_ID 0xfe

#define ACTOR_HITBOX_SIZE 16
#define MIN_ACTOR_DISTANCE 32

#define FIRST_FONT_GLYPH 32
#define LAST_FONT_GLYPH 95
#define FONT_WIDTH 3
#define FONT_HEIGHT 5
#define FONT_GLYPH_BYTE_SIZE 2

#define FOG_FUDGE 2.5

#endif
