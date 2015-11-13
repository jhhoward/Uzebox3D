/*
 *  Uzebox Kernel - Mode 8
 *  Copyright (C) 2009  Alec Bourque
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
 *
 *  Uzebox is a reserved trade mark
*/

;****************************************************
;
;****************************************************	
	
.global displayBuffer
.global overlayBuffer
.global colourTable
.global InitializeVideoMode
.global DisplayLogo
.global VideoModeVsync
.global ClearVram

.section .bss
	.align 8
	colourTable:			.space 512
	displayBuffer:			.space SCREEN_WIDTH
	overlayBuffer:			.space 1152; (SCREEN_WIDTH * 64 / 8)

.section .text

sub_video_mode:

	WAIT r16,1352
	ldi YH,hi8(colourTable)
	
	clr r20
	ldi r21, 4

	ldi XH,hi8(overlayBuffer)
	ldi XL,lo8(overlayBuffer)

;*************************************************************
; Rendering main loop starts here
;*************************************************************
next_scan_line:	
	rcall hsync_pulse 

;	WAIT r19,330 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT
;	WAIT r19,270 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT

	WAIT r19,258 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT
	ldi XH,hi8(overlayBuffer)
	ldi XL,lo8(overlayBuffer)
	ldi r18, (SCREEN_WIDTH / 8)
	mov r19, r20
	lsr r19
	mul r18, r19
	add XL, r0
	adc XH, r1
	
	;***draw line***
	rcall render_tile_line
	
	clr r0
	out _SFR_IO_ADDR(DATA_PORT),r0 ; black
	;// between 161 and 165
	;WAIT r19, (4 * 144) - 161;
	;WAIT r19, 

#if 0
	WAIT r19,122 - CENTER_ADJUSTMENT

	;duplicate each line
	;sbrc r20,0
	;subi YL,lo8(-(SCREEN_WIDTH/4))
	;sbrc r20,0
	;sbci YH,hi8(-(SCREEN_WIDTH/4))
	
	
	inc r20
	cpi r20,(SCREEN_HEIGHT)
	mov r21, r20
	brne next_scan_line
#elif 1
	WAIT r19,118 - CENTER_ADJUSTMENT

	;duplicate each overlay line
	sbrc r20,1
	subi XL,lo8(-(SCREEN_WIDTH/8))
	sbrc r20,1
	sbci XH,hi8(-(SCREEN_WIDTH/8))
	
	inc r20
	cpi r20,(60)
	breq swap_colour_table
	brge decr_counter
	
	;sbrs r20, 0
	inc r21
	rjmp next_scan_line
	
	swap_colour_table:
	inc YH
	rjmp next_scan_line
	
	decr_counter:
	;sbrs r20, 0
	dec r21
	
	cpi r20, 120
	brne next_scan_line
#else
	WAIT r19,99 - CENTER_ADJUSTMENT
	;; < 64 = 1+1+1+1+1+2+N+N = 9
	;; = 64 = 1+1+2+1+2+N+N = 9
	;; > 64 = 1+1+1+2+1+1+2 = 9
	; -27 + 4 = -23
	inc r20
	
	cpi r20, 64
	breq swap_colour_table
	brge decr_counter
	
	inc r21
	nop
	nop
	rjmp next_scan_line
	
	swap_colour_table:
	inc YH
	nop
	nop
	rjmp next_scan_line
	
	decr_counter:
	dec r21
	
	cpi r20, SCREEN_HEIGHT
	brne next_scan_line
#endif
	
	nop
	rcall hsync_pulse ;145
	
	;set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	ori ZL,SYNC_FLAG_VSYNC
	eor ZL,r20
	sts sync_flags,ZL
			
	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	clr r1

	ret


;*************************************************
; RENDER TILE LINE
;
; r20     = render line counter (incrementing)
; Y       = VRAM adress to draw from (must not be modified)
;
; Must preserve r20,Y
; 
; cycles  = 1495
;*************************************************
render_tile_line:
	ldi ZL,lo8(displayBuffer)
	ldi ZH,hi8(displayBuffer)

	;10 cycles per pixel
	ldi r18,SCREEN_WIDTH/8
	
main_loop:
	;mov r19, ZL
	ld r19, X+
	;ldi r19, 0 ;170
	;nop
	
	ld YL, Z+
	add YL, r21
	ld r16, Y
	sbrc r19, 0
	ldi r16, 0
	out _SFR_IO_ADDR(DATA_PORT),r16 ;pixel 0
	
	ld YL, Z+
	add YL, r21
	ld r16, Y
	sbrc r19, 1
	ldi r16, 0
	nop
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16 ;pixel 1

	ld YL, Z+
	add YL, r21
	ld r16, Y
	sbrc r19, 2
	ldi r16, 0
	nop
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16 ;pixel 2
	
	ld YL, Z+
	add YL, r21
	ld r16, Y
	sbrc r19, 3
	ldi r16, 0
	nop
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16 ;pixel 3
	
	ld YL, Z+
	add YL, r21
	ld r16, Y
	sbrc r19, 4
	ldi r16, 0
	nop
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16 ;pixel 4
	
	ld YL, Z+
	add YL, r21
	ld r16, Y
	sbrc r19, 5
	ldi r16, 0
	nop
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16 ;pixel 5
	
	ld YL, Z+
	add YL, r21
	ld r16, Y
	sbrc r19, 6
	ldi r16, 0
	nop
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16 ;pixel 6
	
	ld YL, Z+
	add YL, r21
	ld r16, Y
	sbrc r19, 7
	ldi r16, 0
	dec r18
	;nop
	out _SFR_IO_ADDR(DATA_PORT),r16 ;pixel 7

	brne main_loop

	ret

;***********************************
; CLEAR VRAM 8bit
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:
	//init vram		
	ldi r30,lo8(SCREEN_WIDTH)
	ldi r31,hi8(SCREEN_WIDTH)

	ldi XL,lo8(displayBuffer)
	ldi XH,hi8(displayBuffer)

fill_vram_loop:
	st X+,r1
	sbiw r30,1
	brne fill_vram_loop

	clr r1

	ret
	

;Nothing to do in this mode
DisplayLogo:
VideoModeVsync:
InitializeVideoMode:
	ret

