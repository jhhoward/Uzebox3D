// SDL platform
#include <stdio.h>
#include "SDLPlatform.h"
#include "Engine.h"
#include "TextureScaleLUT.h"

SDLPlatform Platform;

uint8_t displayBuffer[2 * DISPLAYWIDTH * 2];
//uint8_t colourTable[512];
uint8_t overlayBuffer[2 * DISPLAYWIDTH * DISPLAYHEIGHT / 16];

uint8_t currentBuffer = 0;
uint8_t overlayColour = 0;

#if USE_TEXTURE_MAPPING
uint8_t textureBank[256];
#else
uint8_t outerColours[DISPLAYHEIGHT];
#endif

//uint8_t _displayBuffer[DISPLAYWIDTH];
//uint8_t _colourTable[512];

//uint8_t* displayBuffer = _displayBuffer;
//uint8_t* colourTable = _colourTable;

void SDLPlatform::init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer( DISPLAYWIDTH * ZOOM_SCALE * ASPECT_RATIO, DISPLAYHEIGHT * ZOOM_SCALE, SDL_WINDOW_RESIZABLE, &m_appWindow, &m_appRenderer );
	SDL_RenderSetLogicalSize(m_appRenderer, DISPLAYWIDTH * ASPECT_RATIO, DISPLAYHEIGHT);
	
	m_screenSurface = SDL_CreateRGBSurface(0, DISPLAYWIDTH, DISPLAYHEIGHT, 32, 
											0x000000ff,
											0x0000ff00, 
											0x00ff0000, 
											0xff000000
											);
	m_screenTexture = SDL_CreateTexture(m_appRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, m_screenSurface->w * ASPECT_RATIO, m_screenSurface->h);
	m_isRunning = true;
}

int SDL_KeyboardButtonMappings[] = 
{
	SDLK_x, Input_Btn_A, 
	SDLK_z, Input_Btn_B, 
	SDLK_LEFT, Input_Dpad_Left,
	SDLK_RIGHT, Input_Dpad_Right,
	SDLK_UP, Input_Dpad_Up,
	SDLK_DOWN, Input_Dpad_Down,
	SDLK_RETURN, Input_Btn_C,
};

void SDLPlatform::updateInputState(int eventType, bool pressed)
{
	int numMappings = sizeof(SDL_KeyboardButtonMappings) / 2;
	
	for(int n = 0; n < numMappings; n++)
	{
		if(SDL_KeyboardButtonMappings[n * 2] == eventType)
		{
			if(pressed)
			{
				inputState |= SDL_KeyboardButtonMappings[n * 2 + 1];
			}
			else
			{
				inputState &= ~(SDL_KeyboardButtonMappings[n * 2 + 1]);
			}
		}
	}
}

void SDLPlatform::run()
{
	m_isRunning = true;
	
	engine.init();
	
	while(m_isRunning)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type) 
			{
				case SDL_QUIT:
				m_isRunning = false;
				break;
				case SDL_KEYDOWN:
				updateInputState(event.key.keysym.sym, true);
				break;
				case SDL_KEYUP:
				updateInputState(event.key.keysym.sym, false);
				break;
			}

		}

		SDL_SetRenderDrawColor ( m_appRenderer, 206, 221, 231, 255 );
		SDL_RenderClear ( m_appRenderer );

		engine.ticks = 1;
		engine.update();
		engine.draw();

		draw();

		SDL_UpdateTexture(m_screenTexture, NULL, m_screenSurface->pixels, m_screenSurface->pitch);
		SDL_Rect src, dest;
		src.x = src.y = dest.x = dest.y = 0;
		src.w = DISPLAYWIDTH;
		src.h = DISPLAYHEIGHT;
		dest.w = DISPLAYWIDTH * ASPECT_RATIO;
		dest.h = DISPLAYHEIGHT;
		SDL_RenderCopy(m_appRenderer, m_screenTexture, &src, &dest);
		SDL_RenderPresent(m_appRenderer);

		SDL_Delay(1000 / 60);
	}

	SDL_Quit();
}

int main(int, char**)
{
	for(int n = 0; n < 256; n++)
	{
		WARNING("%d %d\n", n, FixedMath::Sin(n));
	}

	Platform.init();

	Platform.run();

	return 0;
}

void SDLPlatform::drawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    *(Uint32 *)p = pixel;
}

void SDLPlatform::drawPixel(uint8_t x, uint8_t y, uint8_t colour)
{
	int red = (((colour >> 0) & 7) * 255) / 7;
	int green = (((colour >> 3) & 7) * 255) / 7;
	int blue = (((colour >> 6) & 3) * 255) / 3;
	Uint32 col = SDL_MapRGBA(m_screenSurface->format, red, green, blue, 255);
	drawPixel(m_screenSurface, x, y, col);
}

#if USE_TEXTURE_MAPPING

#if 0

uint8_t scalerLUT[SCALER_LUT_SIZE];

uint8_t generateScaleForSize(int scanline, int geo)
{
	float top = HALF_DISPLAYHEIGHT - geo * (1.0f + 1.0f / 8.0f);
	float bottom = top + geo * 2;
	float v = (scanline - top) / (bottom - top);
	if(v < 0.0f)
	{
		return 0;
	}
	else if(v > 1.0f)
	{
		return TEXTURE_SIZE + 1;
	}
	else
	{
		return (uint8_t)(v * (TEXTURE_SIZE)) + 1;
	}
}

void generateScalerLUT(int scanline)
{
#if 0	// Split LUT generation over two scanlines
	bool isEven = (scanline & 1) == 0;
	int start = isEven ? 0 : SCALER_LUT_SIZE / 2;
	int end = isEven ? SCALER_LUT_SIZE / 2 : SCALER_LUT_SIZE;
	scanline = isEven ? scanline : scanline - 1;
#else
	int start = 0;
	int end = SCALER_LUT_SIZE;
#endif
	for(int geo = start; geo < end; geo++)
	{
		scalerLUT[geo] = generateScaleForSize(scanline, geo);
	}

#if 0
	if(scanline < HALF_DISPLAYHEIGHT)
	{
		int edge = HALF_DISPLAYHEIGHT - scanline;
		scalerLUT[edge] = generateScaleForSize(scanline, edge);
	}
	else
	{
		int edge = scanline - HALF_DISPLAYHEIGHT;
		scalerLUT[edge] = generateScaleForSize(scanline, edge);
	}
#endif
}
#endif

#define SCALER_LUT_SIZE MAX_WALL_BUFFER_HEIGHT + 1

void SDLPlatform::draw()
{
	uint8_t* currentDisplayBuffer = currentBuffer == 0 ? displayBuffer : displayBuffer + (DISPLAYWIDTH * 2);
	uint8_t* currentOverlayBuffer = currentBuffer == 0 ? overlayBuffer : overlayBuffer + (DISPLAYWIDTH * DISPLAYHEIGHT / 16);
	uint8_t* scalerPtr = (uint8_t*) TextureScaleLUT;
	uint8_t scalerLUT[SCALER_LUT_SIZE];

	for(int n = 0; n < SCALER_LUT_SIZE; n++)
	{
		scalerLUT[n] = *scalerPtr++;
	}

	int y = 0;

	while(y < DISPLAYHEIGHT)
	{
		//generateScalerLUT(y);

		for(int x = 0; x < DISPLAYWIDTH; x++)
		{
			uint8_t geo = min(currentDisplayBuffer[x * 2], SCALER_LUT_SIZE - 1);
			uint8_t offset = scalerLUT[geo];
			uint8_t textureBase = currentDisplayBuffer[x * 2 + 1];
			uint8_t colour = textureBank[textureBase + offset];

			int overlayIndex = (((y >> 1) * DISPLAYWIDTH) + x) / 8;
			uint8_t overlay = currentOverlayBuffer[overlayIndex];
			int overlayMask = 1 << (x % 8);
			
			if((overlayMask & overlay) != 0)
			{
				colour = overlayColour;
			}

			drawPixel(x, y, colour);
		}
		y++;

		if(y < DISPLAYHEIGHT)
		{
			for(int n = 0; n < PATCHES_PER_SCANLINE; n++)
			{
				uint8_t index = *scalerPtr++;
				uint8_t replacement = *scalerPtr++;
				scalerLUT[index] = replacement;
			}
		}
	}
}
#else
void SDLPlatform::draw()
{
	uint8_t offset = 128 - (DISPLAYHEIGHT / 2);
	uint8_t* currentDisplayBuffer = currentBuffer == 0 ? displayBuffer : displayBuffer + (DISPLAYWIDTH * 2);
	uint8_t* currentOverlayBuffer = currentBuffer == 0 ? overlayBuffer : overlayBuffer + (DISPLAYWIDTH * DISPLAYHEIGHT / 16);

	int y = 0;

	while(y < DISPLAYHEIGHT)
	{
		for(int x = 0; x < DISPLAYWIDTH; x++)
		{
			uint8_t outer = outerColours[y];

			uint8_t colour = currentDisplayBuffer[x * 2 + 1];

			uint8_t geo = currentDisplayBuffer[x * 2] + offset;
			if((geo & 0x80) == 0)
			{
				colour = outer;
			}

			int overlayIndex = (((y >> 1) * DISPLAYWIDTH) + x) / 8;
			uint8_t overlay = currentOverlayBuffer[overlayIndex];
			int overlayMask = 1 << (x % 8);
			
			if((overlayMask & overlay) != 0)
			{
				colour = overlayColour;
			}

			drawPixel(x, y, colour);
		}

		y++;

		if(y >= DISPLAYHEIGHT / 2 + 1)
		{
			offset --;
		}
		else
		{
			offset ++;
		}
	}
}
#endif

void ClearVram()
{
	uint8_t* currentDisplayBuffer = currentBuffer == 0 ? displayBuffer : displayBuffer + (DISPLAYWIDTH * 2);

	for(int n = 0; n < DISPLAYWIDTH * 2; n++)
		currentDisplayBuffer[n] = 0;
}
