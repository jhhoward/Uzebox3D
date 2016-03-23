/*
 *  Uzebox Bitmap Demo
 *  Copyright (C) 2009 Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
extern "C" {
#include "uzebox.h"
}
#include "UzeboxPlatform.h"
#include "Engine.h"

UzeboxPlatform Platform;

int main(){

	//Clear the screen (fills the vram with tile zero)
	ClearVram();
	
	/*int n = 0;
	for(int y = 0; y < SCREEN_HEIGHT; y++)
		for(int x = 0; x < SCREEN_WIDTH / 8; x++)
		{
			if(y & 1)
				overlayBuffer[n] = 170;
			else
				overlayBuffer[n] = 85;
			n++;
		}
		*/
	for(int n = 0; n < SCREEN_WIDTH; n++)
	{
		displayBuffer[n * 2] = 40;
		displayBuffer[n * 2 + 1] = UZE_RGB(128, 128, 128);
	}
	

	srand(0x365e);
	
	engine.init();
	
	while(1)
	{
		Platform.update();
		engine.update();
		WaitVsync(1);
		engine.renderer.drawFrame();
	}
}

void UzeboxPlatform::update()
{
	int joypad = ReadJoypad(0);
	
	inputState = 0;
	
	if(joypad & BTN_A)
		inputState |= Input_Btn_A;
	if(joypad & BTN_B)
		inputState |= Input_Btn_B;
	if(joypad & BTN_Y)
		inputState |= Input_Btn_C;
	if(joypad & BTN_LEFT)
		inputState |= Input_Dpad_Left;
	if(joypad & BTN_RIGHT)
		inputState |= Input_Dpad_Right;
	if(joypad & BTN_UP)
		inputState |= Input_Dpad_Up;
	if(joypad & BTN_DOWN)
		inputState |= Input_Dpad_Down;
}

