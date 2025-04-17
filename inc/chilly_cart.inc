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
	wait_vblank
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
	memset $9800, $9c00 - $9800, 0
	ld hl, MENU_ENTRY
	ld a, [hl]
	cp 0
	jp z, load_menu_roms
	cp 1
	jp z, load_menu_rtc
	cp 2
	jp z, load_menu_about
	cp 3
	jp z, load_menu_cheats
; FILES ENTRY
load_menu_roms:
	ld hl, rIE
	set 1, [hl]

	call $c200
	; Write game list to VRAM
	memcpy $9840, $b000, $1c0

	memcpy $9800, menu_text_title_1, menu_text_title_1.end - menu_text_title_1

	memcpy $9a20, menu_text_pages, menu_text_pages.end - menu_text_pages

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

	memcpy $9800, menu_text_title_2, menu_text_title_2.end - menu_text_title_2

	memcpy $99e0, menu_text_timezone, menu_text_timezone.end - menu_text_timezone

	ld hl, RTC_WAS_SAVED
	ld a, [hl]
	and a
	jp z, skip_time_saved_text

	ld [hl], 0
	memcpy $98e2, frame_top, frame_top.end - frame_top
	memcpy $9902, frame_mid, frame_mid.end - frame_mid
	memcpy $9922, frame_bottom, frame_bottom.end - frame_bottom
	memcpy $9905, menu_text_rtc_saved, menu_text_rtc_saved.end - menu_text_rtc_saved
skip_time_saved_text:

	ld hl, $a004
	ld [hl], 0
	call write_time

	jp load_current_view_end
; ABOUT ENTRY
load_menu_about:
	ld hl, rIE
	res 1, [hl]

	memcpy $9800, menu_text_title_3, menu_text_title_3.end - menu_text_title_3

	memcpy $9840, menu_text_about, menu_text_about.end - menu_text_about

	; Write firmware version
	memcpy $9a00, $a200, 20

	jp load_current_view_end
; CHEATS ENTRY
load_menu_cheats:
	ld hl, rIE
	res 1, [hl]

	DEF x = 0
	REPT 16
		memcpy_inc $9820 + (x << 5), $a300 + (x * 9) , 3
		ld hl, $9823 + (x << 5)
		ld [hl], $11
		memcpy_inc $9824 + (x << 5), $a303 + (x * 9) , 3
		ld hl, $9827 + (x << 5)
		ld [hl], $11
		memcpy_inc $9828 + (x << 5), $a306 + (x * 9) , 3
		DEF x += 1
	ENDR

	jp load_current_view_end
load_current_view_end:
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
	wait_vblank
	memcpy $98e2, frame_top, frame_top.end - frame_top
	memcpy $9902, frame_mid, frame_mid.end - frame_mid
	memcpy $9922, frame_bottom, frame_bottom.end - frame_bottom
	memcpy $9905, menu_text_loading, menu_text_loading.end - menu_text_loading
	memcpy $c300, game_loader_ram, game_loader_ram.end - game_loader_ram
	jp $c300

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

frame_top::
	db 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6
.end:

frame_mid::
	db 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11
.end:

frame_bottom::
	db 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9
.end:

menu_text_loading:
	db "Loading..."
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

menu_text_rtc_saved::
	db "Time Saved"
.end:

menu_text_timezone::
	db " Time zone is UTC+0"
.end: