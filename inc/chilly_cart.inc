; Prende il numero da B
; Mette il numero ASCII da HL a HL + 2
num_to_ascii:
	push af
	push hl
	ld a, b
	ld [hl], $30
loop_100:
	cp 100
	jp c, init_loop_10
	sub a, 100
	inc [hl]
	jp loop_100
init_loop_10:
	inc hl
	ld [hl], $30
loop_10:
	cp 10
	jp c, final_num
	sub a, 10
	inc [hl]
	jp loop_10
final_num:
	inc hl
	add $30
	ld [hl], a

	pop hl
	pop af
	ret

clear_tilemap:
	ld hl, $9800
	ld bc, $9c00 - $9800
loop_clear_vram:
	ld a, 0
	ld [hli], a
	dec bc
	ld a, b
	or a, c
	jp nz, loop_clear_vram
	ret

update_menu:
	; Check if an update is needed
	ld hl, NEED_UPDATE
	ld a, [hl]
	and a
	ret z

	; Wait for vblank for turning PPU off
	ld [hl], 0
	call wait_vblank
	ld hl, rLCDC
	res 7, [hl]

	call load_current_view
    call set_current_selection_long
	ld hl, SCX_SELECTED
	ld [hl], 0
	
	; Turn PPU on
	ld hl, rLCDC
	set 7, [hl]
	ret

load_current_view:
	call clear_tilemap
	ld hl, MENU_ENTRY
	ld a, [hl]
	cp 0
	jp z, load_menu_roms
	cp 1
	jp z, load_menu_rtc
	cp 2
	jp z, load_menu_about
; FILES ENTRY
load_menu_roms:
	ld hl, rIE
	set 1, [hl]

	call load_current_page_to_vram

	ld bc, menu_text_title_1
	ld d, menu_text_title_1.end - menu_text_title_1
	ld hl, $9800
loop_load_text_title_1:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec d
	jp nz, loop_load_text_title_1

	ld bc, menu_text_pages
	ld d, menu_text_pages.end - menu_text_pages
	ld hl, $9a20
loop_load_pages_text:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec d
	jp nz, loop_load_pages_text

	ld hl, CURRENT_PAGE
	ld b, [hl]
	inc b
	ld hl, $9a27
	call num_to_ascii

	ld hl, rCOPYFILELIST
	ld b, [hl]
	ld hl, $9a2b
	call num_to_ascii

	jp load_current_view_end
; RTC ENTRY
load_menu_rtc:
	ld hl, rIE
	res 1, [hl]

	ld bc, menu_text_title_2
	ld d, menu_text_title_2.end - menu_text_title_2
	ld hl, $9800
loop_load_text_title_2:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec d
	jp nz, loop_load_text_title_2

	; TODO

	jp load_current_view_end
; ABOUT ENTRY
load_menu_about:
	ld hl, rIE
	res 1, [hl]

	ld bc, menu_text_title_3
	ld d, menu_text_title_3.end - menu_text_title_3
	ld hl, $9800
loop_load_text_title_3:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec d
	jp nz, loop_load_text_title_3

	ld bc, menu_text_about
	ld de, menu_text_about.end - menu_text_about
	ld hl, $9840
loop_about_text:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or e
	jp nz, loop_about_text

	jp load_current_view_end
load_current_view_end:
	ret

load_current_page_to_vram:
    ; Load directory
    ld hl, CURRENT_PAGE
	ld a, [hl]
	ld hl, rCOPYFILELIST
	ld [hl], a
wait_directory_finish:
	ld a, [hl]
	and a
	jp z, wait_directory_finish

	; Write game list to VRAM
	ld bc, $b000
	ld de, $1c0
	ld hl, $9840
game_list:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, game_list
    ret

game_loader:
	ld bc, game_loader_ram
	ld de, game_loader_ram.end - game_loader_ram
	ld hl, $c200
game_loader_loop:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, game_loader_loop
	jp $c200

game_loader_ram::
	di
	ld hl, $a000
	ld [hl], 0
.waiting_for_game
	jr .waiting_for_game
.end:

menu_text_title_1::
	db "Files   "
.end:

menu_text_title_2::
	db "RTC     "
.end:

menu_text_title_3::
	db "About   "
.end:

menu_text_about::
	db "                                "
	db "                                "
	db "                                "
	db "                                "
	db "                                "
	db "                                "
	db "                                "
	db "                                "
	db "                                "
	db "                                "
	db " Chilly-Menu v0.1.0             "
	db "   By AuroraViola               "
	db "                                "
	db "                                "
	db "github.com/ChillyGB             "
	db "                                "
.end:

menu_text_pages::
	db "Pages:    /"
.end: