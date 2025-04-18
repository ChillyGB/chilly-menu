INCLUDE "inc/hardware.inc"
INCLUDE "inc/macros.inc"
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
	wait_vblank
	ld a, 0
	ld [rNR52], a
	ld [rLCDC], a

	; Clear OAM and setup sprites
	memset $fe00, $00a0, 0

	ld hl, $fe00
	ld a, 0
	ld [hli], a
	ld a, 52
	ld [hli], a
	ld a, 2
	ld [hli], a
	ld a, 0
	ld [hli], a

	ld a, 0
	ld [hli], a
	ld a, 36
	ld [hli], a
	ld a, 12
	ld [hli], a
	ld a, $20
	ld [hli], a

	ld a, 0
	ld [hli], a
	ld a, 132
	ld [hli], a
	ld a, 12
	ld [hli], a
	ld a, 0

	ld [hli], a
	ld a, 0
	ld [hli], a
	ld a, 44
	ld [hli], a
	ld a, 1
	ld [hli], a
	ld a, $80
	ld [hli], a

	; Clear RAM
	memset $c000, $00ff, 0
	; clear_vram
	memset $8000, $2000, 0
	; Write tiles
	memcpy $8000, char_tiles, char_tiles.end - char_tiles
	memcpy $9000, cheat_char_tiles, cheat_char_tiles.end - cheat_char_tiles
	; Write qr tiles
	memcpy $8800, qr_tiles, qr_tiles.end - qr_tiles
	; Load game_list_loader to WRAM
	memcpy $c200, game_list_loader_ram, game_list_loader_ram.end - game_list_loader_ram

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
	ld a, LCDCF_ON | LCDCF_BGON | LCDCF_BG8000 | LCDCF_OBJ16
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

char_tiles::
	INCBIN "inc/img/chars.2bpp"
.end:

cheat_char_tiles::
	INCBIN "inc/img/cheat_chars.2bpp"
.end:

qr_tiles::
	INCBIN "inc/img/qr_code_tiles.2bpp"
.end:

INCLUDE "inc/addresses.inc"
INCLUDE "inc/interrupts.inc"
INCLUDE "inc/chilly_cart.inc"
INCLUDE "inc/input.inc"