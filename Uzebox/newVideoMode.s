; Output a pixel (10 cycles)
; Trashes r24, r25. Increments X by two
; Setup:
; r21 : scanline modifier
; r22 : 'outer' colour (floor or ceiling)
; r23 : overlay colour
.macro VIDEO_PIXEL overlayMask, overlayBit
	ld r25, X+							; Load wall colour
	ld r24, X+							; Load wall height
	add r24, r21						; Add scanline modifier
	sbrs r24, 7							; If result < 128
	mov r25, r22						; then use the outer colour
	sbrc \overlayMask, \overlayBit		; If bit set in overlay
	mov r25, r23						; then use overlay colour
	out r25								; Output the pixel colour
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
	; Set up display buffer pointer (2 cycles)
	ldi XL, lo8(displayBuffer)
	ldi XH, hi8(displayBuffer)
	
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

	; Load all the overlay contents ahead of time into registers r0 - r17 (36 cycles)
	LOAD_OVERLAY_MASKS 0, 17
	
	; Output the pixels to the screen (1440 cycles)
	VIDEO_LINE 0, 17
	
	ret
