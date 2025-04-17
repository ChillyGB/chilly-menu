update_keys:
    ; Poll half the controller
    ld a, P1F_GET_BTN
    call .onenibble
    ld b, a ; B7-4 = 1; B3-0 = unpressed buttons
  
    ; Poll the other half
    ld a, P1F_GET_DPAD
    call .onenibble
    swap a ; A3-0 = unpressed directions; A7-4 = 1
    xor a, b ; A = pressed buttons + directions
    ld b, a ; B = pressed buttons + directions
  
    ; And release the controller
    ld a, P1F_GET_NONE
    ldh [rP1], a
  
    ; Combine with previous wCurKeys to make wNewKeys
    ld a, [wCUR_KEYS]
    xor a, b ; A = keys that changed state
    and a, b ; A = keys that changed to pressed
    ld [wNEW_KEYS], a
    ld a, b
    ld [wCUR_KEYS], a
    ret

.onenibble
    ldh [rP1], a ; switch the key matrix
    call .knownret ; burn 10 cycles calling a known ret
    ldh a, [rP1] ; ignore value while waiting for the key matrix to settle
    ldh a, [rP1]
    ldh a, [rP1] ; this read counts
    or a, $F0 ; A7-4 = 1; A3-0 = unpressed keys
.knownret
    ret

set_current_selection_long:
    ld hl, CURRENT_SELECTION_LONG
    ld [hl], 0
    ld a, [rSELECTED_ROM]
    ld de, $20
    ld hl, $9854
    and a
    jp z, loop_selection_check_end
loop_selection_check:
    add hl, de
    dec a
    jp nz, loop_selection_check
loop_selection_check_end:
    ld a, [hl]
    and a
    ret nz
    ld hl, CURRENT_SELECTION_LONG
    ld [hl], 1
    ret

prev_selection:
    ld hl, SCX_SELECTED
    ld [hl], 0
    ld hl, rSELECTED_ROM
    ld a, [hl]
    and a
    jp z, dont_decrement_sel
    dec a
dont_decrement_sel:
    ld [hl], a
    call set_current_selection_long
    ret

next_selection:
    ld hl, SCX_SELECTED
    ld [hl], 0
    ld hl, rSELECTED_ROM
    ld a, [hl]
    cp 13
    jp z, dont_increment_sel
    inc a
dont_increment_sel:
    ld [hl], a
    call set_current_selection_long
    ret

prev_page:
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, CURRENT_PAGE
    ld a, [hl]
    and a
    jp z, dont_decrement_page
    dec a
dont_decrement_page:
    ld [hl], a
    ld hl, rSELECTED_ROM
    ld [hl], 0
    ret

next_page:
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, rCOPYFILELIST
    ld b, [hl]
    dec b
    ld hl, CURRENT_PAGE
    ld a, [hl]
    cp b
    jp z, dont_increment_page
    inc a
dont_increment_page:
    ld [hl], a
    ld hl, rSELECTED_ROM
    ld [hl], 0
    ret

prev_dir:
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, rLOAD_SELECTED
    ld [hl], 1
    ld hl, rSELECTED_ROM
    ld [hl], 0
    ld hl, CURRENT_PAGE
    ld [hl], 0
    ret

open_selected:
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, rLOAD_SELECTED
    ld a, [hl]
    and a
    jp z, game_loader
    ld [hl], 0
    ld hl, rSELECTED_ROM
    ld [hl], 0
    ld hl, CURRENT_PAGE
    ld [hl], 0
    ret

disable_rtc_edit:
    ld hl, RTC_EDIT_MODE
    ld a, [hl]
    and a
    ret z
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, rLCDC
    res 1, [hl]
    ld hl, RTC_EDIT_MODE
    ld [hl], 0
    ret

enable_rtc_edit:
    ld hl, RTC_EDIT_MODE
    ld a, [hl]
    and a
    ret nz
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, rLCDC
    set 1, [hl]
    ld hl, RTC_EDIT_MODE
    ld [hl], 1
    ret

prev_rtc_selection:
    ; check if rtc edit is enabled
    ld hl, RTC_EDIT_MODE
    ld a, [hl]
    and a
    ret z

    ld hl, SELECTED_RTC
    ld a, [hl]
    and a
    ret z

    ld hl, NEED_RTC_UPDATE
    ld [hl], 1

    ld hl, SELECTED_RTC
    dec [hl]
    ret

next_rtc_selection:
    ; check if rtc edit is enabled
    ld hl, RTC_EDIT_MODE
    ld a, [hl]
    and a
    ret z

    ld hl, SELECTED_RTC
    ld a, [hl]
    cp 5
    ret z

    ld hl, NEED_RTC_UPDATE
    ld [hl], 1

    ld hl, SELECTED_RTC
    inc [hl]
    ret

get_selected_rtc:
    ld hl, SELECTED_RTC
    ld a, [hl]
    cp 0
    jp z, rtc_case_0
    cp 1
    jp z, rtc_case_1
    cp 2
    jp z, rtc_case_2
    cp 3
    jp z, rtc_case_3
    cp 4
    jp z, rtc_case_4
    cp 5
    jp z, rtc_case_5
    ret
rtc_case_0:
    ld hl, rDAY
    ret
rtc_case_1:
    ld hl, rMONTH
    ret
rtc_case_2:
    ld hl, rYEAR
    ret
rtc_case_3:
    ld hl, rHOURS
    ret
rtc_case_4:
    ld hl, rMINUTES
    ret
rtc_case_5:
    ld hl, rSECONDS
    ret

save_rtc:
    ld hl, RTC_EDIT_MODE
    ld a, [hl]
    and a
    ret z
    call disable_rtc_edit
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, RTC_WAS_SAVED
    ld [hl], 1
    ld hl, rRTC_SAVE
    ld [hl], 1
    ret

open_cheats:
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, rCHEATS_LOAD
    ld [hl], a
    ld hl, MENU_ENTRY
    ld [hl], 3
    ld hl, rSCX
    ld [hl], 220
    ld hl, rLCDC
    res 4, [hl]
    ret

save_cheats:
close_cheats:
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, MENU_ENTRY
    ld [hl], 0
    ld hl, rSCX
    ld [hl], 0
    ld hl, rLCDC
    set 4, [hl]
    ret

handle_input:
    ld a, [wNEW_KEYS]
    and a, PADF_SELECT
    jp z, check_next
    ld hl, MENU_ENTRY
    ld a, [hl]
    cp 3
    jp z, check_next
    inc a
    cp 3
    jp c, skip_entry_fix
    ld a, 0
skip_entry_fix:
    ld [hl], a
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, rLCDC
    res 1, [hl]
    ld hl, RTC_EDIT_MODE
    res 0, [hl]
check_next:
    ld hl, MENU_ENTRY
	ld a, [hl]
	cp 0
	jp z, input_files
	cp 1
	jp z, input_rtc
	cp 3
	jp z, input_cheats
	jp input_handle_end
; Input when in Files menu
input_files:
check_up_files:
    ld a, [wNEW_KEYS]
    and a, PADF_UP
    jp z, check_down_files
    call prev_selection
check_down_files:
    ld a, [wNEW_KEYS]
    and a, PADF_DOWN
    jp z, check_left_files
    call next_selection
check_left_files:
    ld a, [wNEW_KEYS]
    and a, PADF_LEFT
    jp z, check_right_files
    call prev_page
check_right_files:
    ld a, [wNEW_KEYS]
    and a, PADF_RIGHT
    jp z, check_b_files
    call next_page
check_b_files:
    ld a, [wNEW_KEYS]
    and a, PADF_B
    jp z, check_a_files
    call prev_dir
check_a_files:
    ld a, [wNEW_KEYS]
    and a, PADF_A
    jp z, check_start_files
    call open_selected
check_start_files:
    ld a, [wNEW_KEYS]
    and a, PADF_START
    jp z, input_handle_end
    call open_cheats
    jp input_handle_end
; Input when in RTC menu
input_rtc:
check_up_rtc:
    ld a, [wNEW_KEYS]
    and a, PADF_UP
    jp z, check_down_rtc
    ld a, [RTC_EDIT_MODE]
    and a
    jp z, check_down_rtc
    ld hl, NEED_RTC_UPDATE
    ld [hl], 1
    call get_selected_rtc
    inc [hl]
check_down_rtc:
    ld a, [wNEW_KEYS]
    and a, PADF_DOWN
    jp z, check_left_rtc
    ld a, [RTC_EDIT_MODE]
    and a
    jp z, check_left_rtc
    ld hl, NEED_RTC_UPDATE
    ld [hl], 1
    call get_selected_rtc
    dec [hl]
check_left_rtc:
    ld a, [wNEW_KEYS]
    and a, PADF_LEFT
    jp z, check_right_rtc
    call prev_rtc_selection
check_right_rtc:
    ld a, [wNEW_KEYS]
    and a, PADF_RIGHT
    jp z, check_b_rtc
    call next_rtc_selection
check_b_rtc:
    ld a, [wNEW_KEYS]
    and a, PADF_B
    jp z, check_a_rtc
    call disable_rtc_edit
check_a_rtc:
    ld a, [wNEW_KEYS]
    and a, PADF_A
    jp z, check_start_rtc
    call enable_rtc_edit
check_start_rtc:
    ld a, [wNEW_KEYS]
    and a, PADF_START
    jp z, input_handle_end
    call save_rtc
    jp input_handle_end
input_cheats:
check_start_cheats:
    ld a, [wNEW_KEYS]
    and a, PADF_START
    jp z, check_b_cheats
    call save_cheats
check_b_cheats:
    ld a, [wNEW_KEYS]
    and a, PADF_B
    jp z, input_handle_end
    call close_cheats
input_handle_end:
    ret