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
.global overlayColour
.global textureBank
.global textureScaleLUTSource
.global InitializeVideoMode
.global DisplayLogo
.global VideoModeVsync
.global ClearVram

.section .bss
	.align 8
	textureBank:			.space 256
	textureScaleLUT:		.space 193
	displayBuffer:			.space 480 ; SCREEN_WIDTH * 4
	overlayBuffer:			.space 1920 ; (SCREEN_WIDTH * 64 / 8)
	textureScaleLUTSource:	.word 1
	overlayColour:			.byte 1
	currentBuffer:			.byte 1

.section .text

; Patch the texture scale lut using instructions (8 cycles per update)
.macro PATCH_TEXTURE_SCALE_LUT_INNER tempReg=r17 start=0, end=13
	lpm ZL, X+
	lpm tempReg, X+
	st Z, tempReg
	.if \end-\start
		PATCH_TEXTURE_SCALE_LUT "(\start+1)", \end
	.endif
.endm

.macro PATCH_TEXTURE_SCALE_LUT tempReg=r17 start=0, end=13
	; the current location of the source LUT from earlier (is r16:r17 ok?)
	movw XH:XL, r16:r17
	PATCH_TEXTURE_SCALE_LUT_INNER tempReg, start, end
	movw r16:r17, XH:XL
.endm

sub_video_mode:

	WAIT r16,1351
	
	clr r20						; Clear scanline counter
	
	; Load the initial state for the texture scale LUT
	lds XH, hi8(textureScaleLUTSource)
	lds XL, lo8(textureScaleLUTSource)
	ldi ZH, hi8(textureScaleLUT)
	ldi ZL, lo8(tetxureSclaeLUT)
	ldi r16, TEXTURE_SCALER_LUT_SIZE
load_texture_scale_lut:
	lpm r17, X+
	st Z+, r17
	dec r16
	brne load_texture_scale_lut
	
	; copy the current location of the source LUT for later (is r16:r17 ok?)
	movw r16:r17, XH:XL
	
	; probably need a wait here
	;WAIT r19,212 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT
	
	rcall hsync_pulse 
	rjmp render_scan_line

;*************************************************************
; Rendering main loop starts here
;*************************************************************
next_scan_line:	
	rcall hsync_pulse 

	; 167 is the minimum cycles to wait
	WAIT r19,167 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT
	
	PATCH_TEXTURE_SCALE_LUT r19, 0, 5

	lds r23, overlayColour			; Load overlay colour
	
	;***draw line***
render_scan_line:
	rcall render_video_line
	
	;clr r0
	;out _SFR_IO_ADDR(DATA_PORT),r0 ; black

	; todo: work out how much to wait 
	;WAIT r19,60 - CENTER_ADJUSTMENT

	inc r20			; increase scanline count
	cpi r20, (128)	; compare to screen height
	brsh end_mode
	
	PATCH_TEXTURE_SCALE_LUT r19, 0, 8
	
	rjmp next_scan_line
	
end_mode:
	
	; todo: work out how much to wait 
	;WAIT r19,60 - CENTER_ADJUSTMENT
	rcall hsync_pulse
	
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

; Output a pixel (12 cycles)
; Trashes r24. Increments X by two
; Setup:
; X : display buffer
; Y : texture bank
; Z : texture offset scaler LUT
; r23 : overlay colour
.macro VIDEO_PIXEL overlayMask, overlayBit
	ld ZL, X+							; Load wall height
	ld YL, X+							; Load texture base offset
	ld r24, Z							; Look up texture offset for this scanline
	add YL, r24							; Add offset
	ld r24, Y							; Load texture offset
	sbrc \overlayMask, \overlayBit		; If bit set in overlay
	mov r24, r23						; then use overlay colour
	out _SFR_IO_ADDR(DATA_PORT), r24	; Output the pixel colour
.endm

; Output a block of pixels using the given overlay mask and bit range
.macro VIDEO_BLOCK overlayMask, startBit=0, endBit=7
	VIDEO_PIXEL \overlayMask, \startBit
	.if \endBit-\startBit
		VIDEO_BLOCK \overlayMask, "(\startBit+1)", \endBit
	.endif
.endm

; Load the overlay masks and store the contents in a given range of registers
.macro LOAD_OVERLAY_MASKS, startReg=0, endReg=14
	ld \startReg, Y+
	.if \endReg-\startReg
		LOAD_OVERLAY_MASKS "(\startReg+1)", \endReg
	.endif
.endm

; Macro for unrolling the VIDEO_BLOCK loop
.macro VIDEO_LINE, startReg=0, endReg=14
	VIDEO_BLOCK \startReg, 0, 7
	.if \endReg-\startReg
		VIDEO_LINE "(\startReg+1)", \endReg
	.endif
.endm
	
; Output a scanline of 120 pixels (15 blocks of 8 pixels)
; Trashes r0 - r17, r24, r25
; Assumes:
; r20 : current scanline
; r23 : overlay colour
render_video_line:
	; determine which buffer from the double buffer to use
	lds r18, currentBuffer;
	cpi r18, 1
	breq load_secondary_buffer

load_primary_buffer:
	; Set up display buffer pointer (2 cycles)
	ldi XL, lo8(displayBuffer)
	ldi XH, hi8(displayBuffer)

	; Set up overlay pointer 
	ldi YL, lo8(overlayBuffer)
	ldi YH, hi8(overlayBuffer)
	rjmp load_overlay_mask

load_secondary_buffer:
	; Set up display buffer pointer (2 cycles)
	ldi XL, lo8(displayBuffer + 240)
	ldi XH, hi8(displayBuffer + 240)

	; Set up overlay pointer 
	ldi YL, lo8(overlayBuffer + 960)
	ldi YH, hi8(overlayBuffer + 960)

load_overlay_mask:
	; ptr = overlayBuffer + (scanline >> 1) * (SCREEN_WIDTH / 8)
	mov r16, r20
	lsr r16
	ldi r17, (SCREEN_WIDTH / 8)
	mul r16, r17
	add YL, r0
	adc YH, r1

	; Load all the overlay contents ahead of time into registers r0 - r14 (30 cycles)
	LOAD_OVERLAY_MASKS 0, 14
	
	; Set up pointers
	ldi ZH, hi8(textureScaleLUT)
	ldi YH, hi8(textureBank)
	
	; Output the pixels to the screen (1440 cycles)
	VIDEO_LINE 0, 14
	
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

