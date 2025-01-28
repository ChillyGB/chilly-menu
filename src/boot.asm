INCLUDE "inc/hardware.inc"
INCLUDE "inc/text-macros.inc"

SECTION "VBI", ROM0[$40]

	jp vblank_interrupt

SECTION "STATI", ROM0[$48]

	jp stat_interrupt

SECTION "Header", ROM0[$100]

	jp entry_point

	ds $150 - @, 0 ; Make room for the header

entry_point:
	ld sp, $dfff

	; Turn PPU and APU off
	call wait_vblank
	ld a, 0
	ld [rNR52], a
	ld [rLCDC], a

	; Write tiles
	ld bc, char_tiles
	ld de, char_tiles.end - char_tiles
	ld hl, $8200
load_tiles:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, load_tiles

	ld bc, splash_tiles
	ld de, splash_tiles.end - splash_tiles
	ld hl, $8800
load_tiles_window:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, load_tiles_window

	ld bc, splash_tilemap
	ld de, splash_tilemap.end - splash_tilemap
	ld hl, $9c00
load_tilemap_window:
	ld a, [bc]
	add $80
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, load_tilemap_window

	; Turn PPU on

	ld a, %11100100
	ld [rBGP], a

	ld a, 1
	ld [rIE], a

	ld a, 31
	ld [rLYC], a
	
	ld a, 64
	ld [rSTAT], a

	ld a, 0
	ld [SELECTED_ROM], a

	ld a, 0
	ld [CURRENT_PAGE], a

	ld a, 1
	ld [NEED_UPDATE], a

	ld a, 2
	ld [SPLASH_SCREEN], a

	ld a, 127
	ld [SPLASH_SCREEN_TIMER], a

	ld a, 7
	ld [rWX], a

	ld a, LCDCF_ON | LCDCF_BGON | LCDCF_BG8000 | LCDCF_WINON | LCDCF_WIN9C00
	ld [rLCDC], a

	ei
	nop

main_loop:
	halt
	nop
	jr main_loop

wait_vblank:
	push af
wait_vblank_internal:
	ld a, [rLY]
	cp 144
	jr c, wait_vblank_internal
	pop af
	ret

char_tiles::
	INCBIN "inc/img/chars.2bpp"
.end:

splash_tiles::
	INCBIN "inc/img/splash_tiles.2bpp"
.end:

splash_tilemap::
	INCBIN "inc/img/splash_tilemap.2bpp"
.end:

INCLUDE "inc/addresses.inc"
INCLUDE "inc/interrupts.inc"
INCLUDE "inc/chilly_cart.inc"
INCLUDE "inc/splash.inc"
INCLUDE "inc/input.inc"