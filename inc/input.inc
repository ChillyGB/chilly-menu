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
    ld a, [SELECTED_ROM]
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
    ld hl, SELECTED_ROM
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
    ld hl, SELECTED_ROM
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
    ld hl, SELECTED_ROM
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
    ld hl, SELECTED_ROM
    ld [hl], 0
    ret

prev_dir:
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, $a000
    ld [hl], 1
    ld hl, SELECTED_ROM
    ld [hl], 0
    ld hl, CURRENT_PAGE
    ld [hl], 0
    ret

open_selected:
    ld hl, NEED_UPDATE
    ld [hl], 1
    ld hl, $a000
    ld a, [hl]
    and a
    jp z, game_loader
    ld [hl], 0
    ld hl, SELECTED_ROM
    ld [hl], 0
    ld hl, CURRENT_PAGE
    ld [hl], 0
    ret

handle_input:
    ld a, [wNEW_KEYS]
    and a, PADF_SELECT
    jp z, check_next
    ld hl, MENU_ENTRY
    ld a, [hl]
    inc a
    cp 3
    jp c, skip_entry_fix
    ld a, 0
skip_entry_fix:
    ld [hl], a
    ld hl, NEED_UPDATE
    ld [hl], 1
check_next:
    ld hl, MENU_ENTRY
	ld a, [hl]
	cp 0
	jp z, input_files
	cp 1
	jp z, input_rtc
	cp 2
	jp z, input_handle_end

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
    jp z, input_handle_end
    call open_selected

; Input when in RTC menu
input_rtc:
    jp input_handle_end
input_handle_end:
    ret