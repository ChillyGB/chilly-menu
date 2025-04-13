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

num_to_ascii_10:
	ld [hl], $30
loop_10_10:
	cp 10
	jp c, final_num_10
	sub a, 10
	inc [hl]
	jp loop_10_10
final_num_10:
	inc hl
	add $30
	ld [hl], a
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

write_time:
	; Write separators
	ld a, "/"
	ld hl, $9867
	ld [hl], a
	inc hl
	inc hl
	inc hl
	ld [hl], a
	ld a, ":"
	ld hl, $98a8
	ld [hl], a
	inc hl
	inc hl
	inc hl
	ld [hl], a

	; Write date
	ld a, [rDAY]
	ld hl, $9865
	call num_to_ascii_10

	ld a, [rMONTH]
	ld hl, $9868
	call num_to_ascii_10

	ld a, 20
	ld hl, $986b
	call num_to_ascii_10

	ld a, [rYEAR]
	ld hl, $986d
	call num_to_ascii_10

	; Write time
	ld a, [rHOURS]
	ld hl, $98a6
	call num_to_ascii_10

	ld a, [rMINUTES]
	ld hl, $98a9
	call num_to_ascii_10

	ld a, [rSECONDS]
	ld hl, $98ac
	call num_to_ascii_10

	ret

update_menu:
	; Check if an update is needed
	ld hl, NEED_UPDATE
	ld a, [hl]
	and a
	ret z
	ld [hl], 0

	; Wait for vblank for turning PPU off
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
	ld hl, $9a2a
	call num_to_ascii

	ld hl, rCOPYFILELIST
	ld b, [hl]
	ld hl, $9a2e
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

	ld bc, menu_text_timezone
	ld d, menu_text_timezone.end - menu_text_timezone
	ld hl, $99e0
loop_load_text_timezone:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec d
	jp nz, loop_load_text_timezone

	ld hl, $a004
	ld [hl], 0
	call write_time

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

	; Write firmware version
	ld bc, $a200
	ld d, 20
	ld hl, $9a00
loop_firmware_text:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec d
	jp nz, loop_firmware_text

	jp load_current_view_end
load_current_view_end:
	ret

load_current_page_to_vram:
	call $c200

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

update_rtc:
	ld hl, NEED_RTC_UPDATE
	ld a, [hl]
	and a
	ret z
	ld [hl], 0

	ld hl, SELECTED_RTC
	ld a, [hl]
	cp 0
	jp z, move_to_day
	cp 1
	jp z, move_to_month
	cp 2
	jp z, move_to_year
	cp 3
	jp z, move_to_hours
	cp 4
	jp z, move_to_minutes
	cp 5
	jp z, move_to_seconds
	jp move_end
move_to_day:
	ld hl, $fe00
	ld [hl], 36
	inc hl
	ld [hl], 52
	jp move_end
move_to_month:
	ld hl, $fe00
	ld [hl], 36
	inc hl
	ld [hl], 76
	jp move_end
move_to_year:
	ld hl, $fe00
	ld [hl], 36
	inc hl
	ld [hl], 116
	jp move_end
move_to_hours:
	ld hl, $fe00
	ld [hl], 52
	inc hl
	ld [hl], 60
	jp move_end
move_to_minutes:
	ld hl, $fe00
	ld [hl], 52
	inc hl
	ld [hl], 84
	jp move_end
move_to_seconds:
	ld hl, $fe00
	ld [hl], 52
	inc hl
	ld [hl], 108
	jp move_end
move_end:
	call write_time
	ret

game_loader:
	di

	call wait_vblank
	ld bc, menu_text_loading1
	ld de, menu_text_loading1.end - menu_text_loading1
	ld hl, $98e2
load_game_loading_1:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, load_game_loading_1

	ld bc, menu_text_loading2
	ld de, menu_text_loading2.end - menu_text_loading2
	ld hl, $9902
load_game_loading_2:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, load_game_loading_2

	ld bc, menu_text_loading3
	ld de, menu_text_loading3.end - menu_text_loading3
	ld hl, $9922
load_game_loading_3:
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jp nz, load_game_loading_3

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
	ld hl, rLOAD_SELECTED
	ld [hl], 0
.waiting_for_game
	jr .waiting_for_game
.end:

game_list_loader_ram::
    ; Load directory
    ld hl, CURRENT_PAGE
	ld a, [hl]
	ld hl, rCOPYFILELIST
	ld [hl], a

	ld hl, rCOPYFILELIST
.wait_directory_finish:
	ld a, [hl]
	and a
	jr z, .wait_directory_finish
	ret
.end:

menu_text_title_1::
	db "    File manager"
.end:

menu_text_title_2::
	db "    RTC Settings"
.end:

menu_text_title_3::
	db "        Info"
.end:

menu_text_loading1::
	db 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6
.end:

menu_text_loading2::
	db 10, "  Loading...  ", 11
.end:

menu_text_loading3::
	db 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9
.end:

menu_text_about::
	db 0, 0, 0, 0, 0, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db 0, 0, 0, 0, 0, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db 0, 0, 0, 0, 0, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db 0, 0, 0, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db 0, 0, 0, 0, 0, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db 0, 0, 0, 0, 0, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db 0, 0, 0, 0, 0, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db 0, 0, 0, 0, 0, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db 0, 0, 0, 0, 0, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db "                                "
	db " Chilly-Menu v0.1.0             "
	db "   By AuroraViola               "
	db "                                "
	db "  Firmware Version              "
	db "                                "
	db "                                "
.end:

menu_text_pages::
	db "   Pages:    /"
.end:

menu_text_timezone::
	db " Time zone is UTC+0"
.end: