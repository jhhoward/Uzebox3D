#pragma once
#include <SDL.h>
#include "Vector2.h"

#define STARTING_WINDOW_WIDTH 800
#define STARTING_WINDOW_HEIGHT 600

#define UZE_RGB(r, g, b) ((r >> 5) | ((g >> 5) << 3) | ((b >> 6) << 6))

#define GRID_COLOUR UZE_RGB(50, 50, 50)
#define GRID_COLOUR_BOLD UZE_RGB(75, 75, 75)
#define GRID_SPACING 16
#define VERTEX_BOX_SIZE 3
#define NO_SELECTION -1

class Level;

class LevelEditor
{
public:
	void init();
	void run();

	void drawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
	void drawPixel(int x, int y, uint8_t colour);
	void drawLine(int x0, int y0, int x1, int y1, uint8_t colour);

	struct 
	{
		Vector2 panningPosition;
	} View;

	struct
	{
		int hoverRoom;
		int hoverVertex;
		int hoverWall;
		int selectedRoom;
		int selectedWall;
		int selectedVertex;
	} Selection;

	Vector2 transformPoint(Vector2& in);
	Vector2 untransformPoint(Vector2& in);
	Vector2 snapToGrid(Vector2& in);

private:
	void draw();
	void drawGrid();
	void updateSelection();

	SDL_Window* m_appWindow;
	SDL_Renderer* m_appRenderer;
	SDL_Surface* m_screenSurface;
	SDL_Texture* m_screenTexture;
	bool m_isRunning;

	bool m_isPanning;
	bool m_isDraggingVertex;
	bool m_isDraggingWall;
	Vector2 m_wallDragStart;

	Level* m_currentLevel;
};

extern LevelEditor gEditor;