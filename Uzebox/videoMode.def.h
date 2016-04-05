/** 
 * ==============================================================================
 *
 * This file contains global defines for video mode (for asm & C)
 *
 * ===============================================================================
 */
#pragma once

#ifndef USE_TEXTURE_MAPPING
#define USE_TEXTURE_MAPPING 1
#endif

#if USE_TEXTURE_MAPPING
#define VMODE_ASM_SOURCE "videoMode.s"
#else
#define VMODE_ASM_SOURCE "videoModeNonTextured.s"
#endif

#define VMODE_C_PROTOTYPES "videoMode.h"
#define VMODE_FUNC sub_video_mode

#if USE_TEXTURE_MAPPING
#define SCALER_LUT_SIZE 193
#define SCREEN_WIDTH 120
#else
#define SCREEN_WIDTH 144
#endif
#define SCREEN_HEIGHT 64

#define VRAM_ADDR_SIZE 1
#define VRAM_PTR_TYPE char

#define FIRST_RENDER_LINE 36
#define FRAME_LINES SCREEN_HEIGHT

