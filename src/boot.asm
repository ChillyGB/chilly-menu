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

	; Clear OAM and setup sprites
	ld d, $a0
	ld hl, $fe00
	ld a, 0
clear_oam:
	ld [hli], a
	dec d
	jp nz, clear_oam

	ld hl, $fe00
	ld a, 32
	ld [hli], a
	ld a, 52
	ld [hli], a
	ld a, 1
	ld [hli], a
	ld a, 0
	ld [hli], a
	ld a, 48
	ld [hli], a
	ld a, 52
	ld [hli], a
	ld a, 1
	ld [hli], a
	ld a, $40
	ld [hli], a

	; Clear RAM
	ld d, $ff
	ld hl, $c000
	ld a, 0
clear_ram:
	ld [hli], a
	dec d
	jp nz, clear_ram

	; clear_vram
	ld de, $2000
	ld hl, $8000
clear_vram:
	ld a, 0
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, clear_vram

	; Write tiles
	ld bc, char_tiles
	ld de, char_tiles.end - char_tiles
	ld hl, $8000
load_tiles:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, load_tiles

	; Load game_list_loader to WRAM
	ld bc, game_list_loader_ram
	ld de, game_list_loader_ram.end - game_list_loader_ram
	ld hl, $c200
game_list_loader_loop:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, game_list_loader_loop

	; Initialize registers
	ld a, %11100100
	ld [rBGP], a
	ld [rOBP0], a
	ld [rOBP1], a

	ld a, 31
	ld [rLYC], a
	
	ld a, 64
	ld [rSTAT], a

	ld a, 0
	ld [rSELECTED_ROM], a

	ld a, 0
	ld [CURRENT_PAGE], a

	ld a, 1
	ld [NEED_UPDATE], a

	ld a, 0
	ld [RTC_EDIT_MODE], a

	ld a, 0
	ld [SELECTED_RTC], a

	ld a, 7
	ld [rWX], a

	; Turn PPU on
	ld a, LCDCF_ON | LCDCF_BGON | LCDCF_BG8000
	ld [rLCDC], a

	ld a, 0
	ld [rIF], a
	ld a, 1
	ld [rIE], a

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

INCLUDE "inc/addresses.inc"
INCLUDE "inc/interrupts.inc"
INCLUDE "inc/chilly_cart.inc"
INCLUDE "inc/splash.inc"
INCLUDE "inc/input.inc"