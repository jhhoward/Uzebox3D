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
.global currentBuffer
.global outerColours
.global overlayColour;
.global InitializeVideoMode
.global DisplayLogo
.global VideoModeVsync
.global ClearVram

.section .bss
	displayBuffer:			.space 576 ;SCREEN_WIDTH * 4
	overlayBuffer:			.space 2304; (SCREEN_WIDTH * 64 / 8)
	overlayColour:			.byte 1
	outerColours:			.space 128;
	currentBuffer:			.byte 1

.section .text

sub_video_mode:

	WAIT r16,1351
	
	clr r20						; Clear scanline counter
	ldi r21, 128	- (128 / 2)	; Init scanline modifier

;*************************************************************
; Rendering main loop starts here
;*************************************************************
next_scan_line:	
	rcall hsync_pulse 

;	WAIT r19,258 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT
	WAIT r19,212 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT

; r20 : current scanline
; r21 : scanline modifier
; r22 : 'outer' colour (floor or ceiling)
; r23 : overlay colour
	lds r23, overlayColour			; Load overlay colour
	
	; Load the outer colour
	ldi ZH, hi8(outerColours)
	ldi ZL, lo8(outerColours)
	clr r0
	add ZL, r20
	adc ZH, r0
	ld r22, Z
	
	;***draw line***
	;rcall render_tile_line
	rcall render_video_line
	
	clr r0
	out _SFR_IO_ADDR(DATA_PORT),r0 ; black

	WAIT r19,60 - CENTER_ADJUSTMENT

	inc r20			; increase scanline count
	cpi r20,(65)	; displayheight / 2 + 1
	brsh decr_counter
	
	inc r21
	rjmp .
	rjmp next_scan_line
	
	decr_counter:
	dec r21
	
	cpi r20, 127
	brne next_scan_line
	
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

; Output a pixel (10 cycles)
; Trashes r24, r25. Increments X by two
; Setup:
; r21 : scanline modifier
; r22 : 'outer' colour (floor or ceiling)
; r23 : overlay colour
.macro VIDEO_PIXEL overlayMask, overlayBit
	ld r24, X+							; Load wall height
	ld r25, X+							; Load wall colour
	add r24, r21						; Add scanline modifier
	sbrs r24, 7							; If result < 128
	mov r25, r22						; then use the outer colour
	sbrc \overlayMask, \overlayBit		; If bit set in overlay
	mov r25, r23						; then use overlay colour
	out _SFR_IO_ADDR(DATA_PORT), r25	; Output the pixel colour
.endm

; Output a block of pixels using the given overlay mask and bit range
.macro VIDEO_BLOCK overlayMask, startBit=0, endBit=7
	VIDEO_PIXEL \overlayMask, \startBit
	.if \endBit-\startBit
		VIDEO_BLOCK \overlayMask, "(\startBit+1)", \endBit
	.endif
.endm

; Load the overlay masks and store the contents in a given range of registers
.macro LOAD_OVERLAY_MASKS, startReg=0, endReg=17
	ld \startReg, Y+
	.if \endReg-\startReg
		LOAD_OVERLAY_MASKS "(\startReg+1)", \endReg
	.endif
.endm

; Macro for unrolling the VIDEO_BLOCK loop
.macro VIDEO_LINE, startReg=0, endReg=17
	VIDEO_BLOCK \startReg, 0, 7
	.if \endReg-\startReg
		VIDEO_LINE "(\startReg+1)", \endReg
	.endif
.endm
	
; Output a scanline of 144 pixels (18 blocks of 8 pixels)
; 47 cycles setup
; 1440 cycles rendering
; 1487? cycles total
; Trashes r0 - r17, r24, r25
; Assumes:
; r20 : current scanline
; r21 : scanline modifier
; r22 : 'outer' colour (floor or ceiling)
; r23 : overlay colour
render_video_line:
	; determine which buffer from the double buffer to use
	lds r18, currentBuffer;
	
	; Set up display buffer pointer (2 cycles)
	ldi XL, lo8(displayBuffer)
	ldi XH, hi8(displayBuffer)
	
	; If we are using the second buffer then increment pointer by 288
	ldi r16, 32
	ldi r17, 1
	sbrc r18, 0
	add XL, r16
	sbrc r18, 0
	adc XH, r17

	; Set up overlay pointer (9 cycles)
	; ptr = overlayBuffer + (scanline >> 1) * (SCREEN_WIDTH / 8)
	ldi YL, lo8(overlayBuffer)
	ldi YH, hi8(overlayBuffer)
	mov r16, r20
	lsr r16
	ldi r17, (SCREEN_WIDTH / 8)
	mul r16, r17
	add YL, r0
	adc YH, r1

	; If we are using the second buffer then increment pointer by 1152
	ldi r16, 128
	ldi r17, 4
	sbrc r18, 0
	add YL, r16
	sbrc r18, 0
	adc YH, r17

	; Load all the overlay contents ahead of time into registers r0 - r17 (36 cycles)
	LOAD_OVERLAY_MASKS 0, 17
	
	; Output the pixels to the screen (1440 cycles)
	VIDEO_LINE 0, 17
	
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

