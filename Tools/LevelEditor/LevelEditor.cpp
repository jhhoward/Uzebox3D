#include <SDL.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include "LevelEditor.h"
#include "Level.h"

LevelEditor gEditor;
char get_line_intersection(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, Vector2& out);

int main(int, char**)
{
	gEditor.init();
	gEditor.run();

	return 0;
}

void LevelEditor::init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer( STARTING_WINDOW_WIDTH, STARTING_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &m_appWindow, &m_appRenderer );
	SDL_RenderSetLogicalSize(m_appRenderer, STARTING_WINDOW_WIDTH, STARTING_WINDOW_HEIGHT);
	
	m_screenSurface = SDL_CreateRGBSurface(0, STARTING_WINDOW_WIDTH, STARTING_WINDOW_HEIGHT, 32, 
											0x000000ff,
											0x0000ff00, 
											0x00ff0000, 
											0xff000000
											);
	m_screenTexture = SDL_CreateTexture(m_appRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, m_screenSurface->w, m_screenSurface->h);
	m_isRunning = true;
	m_isPanning = false;
	View.panningPosition.x = -STARTING_WINDOW_WIDTH / 2;
	View.panningPosition.y = -STARTING_WINDOW_HEIGHT / 2;

	Selection.hoverRoom = NO_SELECTION;
	Selection.hoverVertex = NO_SELECTION;
	Selection.hoverWall = NO_SELECTION;
	Selection.selectedRoom = NO_SELECTION;
	Selection.selectedWall = NO_SELECTION;
	Selection.selectedVertex = NO_SELECTION;
	m_isDraggingVertex = false;
	m_isDraggingWall = false;

	m_currentLevel = new Level();
}


void LevelEditor::run()
{
	m_isRunning = true;
	
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
				case SDL_MOUSEBUTTONDOWN:
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					if(Selection.hoverVertex != NO_SELECTION)
					{
						m_isDraggingVertex = true;
						Selection.selectedVertex = Selection.hoverVertex;
					}
					else if(Selection.hoverWall != NO_SELECTION)
					{
						Vector2 mousePos;
						SDL_GetMouseState(&mousePos.x, &mousePos.y);
						m_wallDragStart = untransformPoint(mousePos);
						m_isDraggingWall = true;
						Selection.selectedRoom = Selection.hoverRoom;
						Selection.selectedWall = Selection.hoverWall;
					}
				}
				else if(event.button.button == SDL_BUTTON_RIGHT)
				{
					m_isPanning = true;
				}
				break;
				case SDL_MOUSEBUTTONUP:
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					if(m_isDraggingVertex)
					{
						m_currentLevel->mergePotentialVertices(Selection.selectedVertex);
					}
					m_isDraggingVertex = false;
					Selection.selectedVertex = NO_SELECTION;
					m_isDraggingWall = false;
				}
				else if(event.button.button == SDL_BUTTON_RIGHT)
				{
					m_isPanning = false;
				}
				break;
				case SDL_MOUSEMOTION:
				if(m_isPanning)
				{
					View.panningPosition.x -= event.motion.xrel;
					View.panningPosition.y -= event.motion.yrel;
				}
				break;
				case SDL_KEYDOWN:
				if(!m_isPanning && !m_isDraggingWall && !m_isDraggingVertex)
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_INSERT:
						{
							if(Selection.hoverWall != NO_SELECTION && Selection.hoverRoom != NO_SELECTION)
							{
								m_currentLevel->splitWall(Selection.hoverRoom, Selection.hoverWall);
							}
							else
							{
								Vector2 mousePos;
								SDL_GetMouseState(&mousePos.x, &mousePos.y);
								mousePos = untransformPoint(mousePos);
								m_currentLevel->createRoom(mousePos, 20);
							}
						}
						case SDLK_DELETE:
						{
							if(Selection.hoverVertex != NO_SELECTION)
							{
								m_currentLevel->deleteVertex(Selection.hoverVertex);
								Selection.hoverVertex = NO_SELECTION;
							}
						}
							break;
						case SDLK_s:
							m_currentLevel->save("map.dat");
							m_currentLevel->saveHeader("Data_map.h");
							break;
						case SDLK_l:
						{
							m_currentLevel->load("map.dat");
							Selection.selectedRoom = NO_SELECTION;
							Selection.selectedWall = NO_SELECTION;
							Selection.selectedVertex = NO_SELECTION;
						}
							break;
						break;
					}
				}
					break;
			}

		}

		updateSelection();

		SDL_SetRenderDrawColor ( m_appRenderer, 0, 0, 0, 255 );
		SDL_RenderClear ( m_appRenderer );

		draw();

		SDL_UpdateTexture(m_screenTexture, NULL, m_screenSurface->pixels, m_screenSurface->pitch);
		/*SDL_Rect src, dest;
		src.x = src.y = dest.x = dest.y = 0;
		src.w = DISPLAYWIDTH;
		src.h = DISPLAYHEIGHT;
		dest.w = DISPLAYWIDTH * ASPECT_RATIO;
		dest.h = DISPLAYHEIGHT;*/
		SDL_RenderCopy(m_appRenderer, m_screenTexture, nullptr, nullptr); //&src, &dest);
		SDL_RenderPresent(m_appRenderer);

		SDL_Delay(1000 / 60);
	}

	SDL_Quit();
}

void LevelEditor::drawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	if(x < 0 || y < 0 || x >= surface->w || y >= surface->h)
		return;

    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    *(Uint32 *)p = pixel;
}

void LevelEditor::drawPixel(int x, int y, uint8_t colour)
{
	int red = (((colour >> 0) & 7) * 255) / 7;
	int green = (((colour >> 3) & 7) * 255) / 7;
	int blue = (((colour >> 6) & 3) * 255) / 3;
	Uint32 col = SDL_MapRGBA(m_screenSurface->format, red, green, blue, 255);
	drawPixel(m_screenSurface, x, y, col);
}

void LevelEditor::drawLine(int x0, int y0, int x1, int y1, uint8_t colour) 
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2, e2;
 
	for(;;)
	{
		drawPixel(x0, y0, colour);
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

void LevelEditor::drawGrid()
{
	int windowWidth = m_screenSurface->w;
	int windowHeight = m_screenSurface->h;
	Vector2 offset = View.panningPosition;
	offset.x = -offset.x % GRID_SPACING;
	offset.y = -offset.y % GRID_SPACING;

	for(int y = offset.y; y < windowHeight; y += GRID_SPACING)
	{
		drawLine(0, y, windowWidth, y, GRID_COLOUR);
	}

	for(int x = offset.x; x < windowWidth; x+= GRID_SPACING)
	{
		drawLine(x, 0, x, windowHeight, GRID_COLOUR);
	}

	Vector2 center = View.panningPosition;
	drawLine(-center.x, 0, -center.x, windowHeight, GRID_COLOUR_BOLD);
	drawLine(0, -center.y, windowWidth, -center.y, GRID_COLOUR_BOLD);
	
}

void LevelEditor::draw()
{
	memset(m_screenSurface->pixels, 0, m_screenSurface->pitch * m_screenSurface->h);
	drawGrid();
	m_currentLevel->draw();


#if 0
	Vector2 mousePos;
	SDL_GetMouseState(&mousePos.x, &mousePos.y);

	Vector2 p0 = Vector2(0, 0);
	Vector2 p1 = snapToGrid(untransformPoint(mousePos));

	drawLine(transformPoint(p0).x, transformPoint(p0).y, transformPoint(p1).x, transformPoint(p1).y, UZE_RGB(128, 64, 64));

	for(int r = 0; r < m_currentLevel->rooms.size(); r++)
	{
		for(int w = 0; w < m_currentLevel->rooms[r]->walls.size(); w++)
		{
			Vector2 first = m_currentLevel->vertices[m_currentLevel->rooms[r]->vertices[w]];
			Vector2 second = w == m_currentLevel->rooms[r]->vertices.size() - 1 ? m_currentLevel->vertices[m_currentLevel->rooms[r]->vertices[0]] : m_currentLevel->vertices[m_currentLevel->rooms[r]->vertices[w + 1]];

			Vector2 collision;
			if(get_line_intersection(p0, p1, first, second, collision))
			{
				Vector2 proj = transformPoint(collision);
				drawLine(proj.x - 10, proj.y - 10, proj.x + 10, proj.y + 10, UZE_RGB(255, 128, 128));
				drawLine(proj.x + 10, proj.y - 10, proj.x - 10, proj.y + 10, UZE_RGB(255, 128, 128));
			}
		}
	}
#endif

}

Vector2 LevelEditor::transformPoint(Vector2& in)
{
	Vector2 out;
	out.x = in.x - View.panningPosition.x;
	out.y = in.y - View.panningPosition.y;
	return out;
}

Vector2 LevelEditor::untransformPoint(Vector2& in)
{
	Vector2 out;
	out.x = in.x + View.panningPosition.x;
	out.y = in.y + View.panningPosition.y;
	return out;
}

Vector2 LevelEditor::snapToGrid(Vector2& in)
{
	Vector2 frac = Vector2(in.x % GRID_SPACING, in.y % GRID_SPACING);
	frac.x = frac.x < GRID_SPACING / 2 ? 0 : GRID_SPACING;
	frac.y = frac.y < GRID_SPACING / 2 ? 0 : GRID_SPACING;
	Vector2 out;
	out.x = in.x - (in.x % GRID_SPACING) + frac.x;
	out.y = in.y - (in.y % GRID_SPACING) + frac.y;
	return out;
}

void LevelEditor::updateSelection()
{
	Vector2 mousePos;

	SDL_GetMouseState(&mousePos.x, &mousePos.y);

	if(m_isDraggingVertex)
	{
		m_currentLevel->vertices[Selection.selectedVertex] = snapToGrid(untransformPoint(mousePos));
		Selection.hoverWall = NO_SELECTION;
		Selection.hoverRoom = NO_SELECTION;
	}
	else if(m_isDraggingWall)
	{

	}
	else
	{
		Selection.hoverVertex = m_currentLevel->findHoverVertex(untransformPoint(mousePos));
		m_currentLevel->findHoverWall(untransformPoint(mousePos), Selection.hoverRoom, Selection.hoverWall);
	}
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
