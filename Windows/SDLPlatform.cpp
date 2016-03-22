// SDL platform
#include <stdio.h>
#include "SDLPlatform.h"
#include "Engine.h"

SDLPlatform Platform;

uint8_t displayBuffer[2 * DISPLAYWIDTH * 2];
//uint8_t colourTable[512];
uint8_t overlayBuffer[2 * DISPLAYWIDTH * DISPLAYHEIGHT / 16];

uint8_t currentBuffer = 0;

uint8_t outerColours[DISPLAYHEIGHT];

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

		SDL_Delay(1000 / 30);
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


const uint8_t ditherMask[4] =
{
	51, 153,
	204, 102
};

uint8_t Dither(uint8_t x, uint8_t y, uint8_t alpha, uint8_t i, uint8_t j)
{
	uint8_t index = (j & 1) * 2 + (i & 1);
	if(alpha < ditherMask[index])
		return x;
	else return y;
}

uint8_t GradientColours[256];
uint8_t NumGradientColours = 0;

void GenTable()
{
	uint8_t lastCol = UZE_RGB(0, 0, 0);
	NumGradientColours = 1;
	GradientColours[0] = lastCol;

	for(int n = 0; n < 255; n++)
	{
		uint8_t col = UZE_RGB(n, n, n);

		if(lastCol != col)
		{
			lastCol = col;
			GradientColours[NumGradientColours++] = col;
		}
	}
}

void SDLPlatform::draw()
{
	uint8_t offset = 127 - (DISPLAYHEIGHT / 2);
	uint8_t floorColour = UZE_RGB(32, 64, 32);
//	uint8_t floorColour = UZE_RGB(64, 64, 64);
//	uint8_t ceilingColour = UZE_RGB(32, 32, 32);
	uint8_t ceilingColour = UZE_RGB(0, 0, 64);
	uint8_t outer = ceilingColour;
	uint8_t* currentDisplayBuffer = currentBuffer == 0 ? displayBuffer : displayBuffer + (DISPLAYWIDTH * 2);
	uint8_t* currentOverlayBuffer = currentBuffer == 0 ? overlayBuffer : overlayBuffer + (DISPLAYWIDTH * DISPLAYHEIGHT / 16);

	GenTable();

	for(int y = 0; y < DISPLAYHEIGHT; y++)
	{
		if(y >= DISPLAYHEIGHT / 2)
		{
			outer = floorColour;

			offset --;

			uint8_t gradCol = ((y - (DISPLAYHEIGHT / 2)) * 255) / (DISPLAYHEIGHT / 2);
			outer = Dither(floorColour, 0, gradCol, 0, y);
			//outer = UZE_RGB(gradCol, gradCol, gradCol);
		}
		else
		{
			offset ++;

			//uint8_t gradCol = (((DISPLAYHEIGHT / 2) - y) * 255) / (DISPLAYHEIGHT / 2);
			//outer = UZE_RGB(gradCol, gradCol, gradCol);
		}

		for(int x = 0; x < DISPLAYWIDTH; x++)
		{
			if(y >= DISPLAYHEIGHT / 2)
			{
#if 0
				uint8_t gradCol = ((y - (DISPLAYHEIGHT / 2)) * 256) / (DISPLAYHEIGHT / 2);

				uint8_t posA = (gradCol * NumGradientColours) / 256;
				uint8_t posB = posA + 1;
				uint8_t alpha = ((gradCol - ((posA * 256) / NumGradientColours)) * 256) / (256 / NumGradientColours);

				posB = min(NumGradientColours - 1, posB);

				outer = Dither(GradientColours[posA], GradientColours[posB], alpha, x, y);
				outer = GradientColours[posA];
#elif 0
				uint8_t r = (floorColour & 7) << 5;
				uint8_t g = (floorColour & 56) << 2;
				uint8_t b = (floorColour & 192);
				uint8_t wallHeight = 128 - offset;
				r = min(r, (r * wallHeight) / (HALF_DISPLAYHEIGHT / FOG_FUDGE));
				g = min(g, (g * wallHeight) / (HALF_DISPLAYHEIGHT / FOG_FUDGE));
				b = min(b, (b * wallHeight) / (HALF_DISPLAYHEIGHT / FOG_FUDGE));
				outer = UZE_RGB(r, g, b);
#else
				outer = floorColour;
#endif
				//outer = UZE_RGB(gradCol, gradCol, gradCol);
			}
			else
			{
				uint8_t r = (ceilingColour & 7) << 5;
				uint8_t g = (ceilingColour & 56) << 2;
				uint8_t b = (ceilingColour & 192);
				uint8_t wallHeight = 128 - offset;
				r = min(r, (r * wallHeight) / (HALF_DISPLAYHEIGHT / FOG_FUDGE));
				g = min(g, (g * wallHeight) / (HALF_DISPLAYHEIGHT / FOG_FUDGE));
				b = min(b, (b * wallHeight) / (HALF_DISPLAYHEIGHT / FOG_FUDGE));
				outer = UZE_RGB(r, g, b);
				outer = ceilingColour;
			}

			outer = outerColours[y];

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
				colour = 0;
			}

			drawPixel(x, y, colour);
		}
	}
}

void ClearVram()
{
	uint8_t* currentDisplayBuffer = currentBuffer == 0 ? displayBuffer : displayBuffer + (DISPLAYWIDTH * 2);

	for(int n = 0; n < DISPLAYWIDTH * 2; n++)
		currentDisplayBuffer[n] = 0;
}
