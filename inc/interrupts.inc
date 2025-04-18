vblank_interrupt:
    di
	call update_keys
	call handle_input
    call update_menu
    call update_rtc
    call update_cheats
    reti

stat_interrupt:
    di
    
wait_mode0:
    ld a, [rSTAT]
    and 3
    and a
    jr nz, wait_mode0

    ld hl, rSELECTED_ROM
    ld b, [hl]
    inc b
    inc b
    sla b
    sla b
    sla b
    dec b

    ld a, [rLYC]
    cp a, b
    jp nz, not_first_lyc

    ld a, b
    add a, 8
    ld [rLYC], a
    ld a, [SCX_SELECTED]
    ld [rSCX], a
    ld a, %11101000
    ld [rBGP], a

    jp skip_a
not_first_lyc:
    ld hl, CURRENT_SELECTION_LONG
    ld a, [hl]
    and a
    jp nz, dont_increment_scx
    ld hl, SCX_SELECTED
    inc [hl]
dont_increment_scx:
    ld a, 0
    ld [rSCX], a
    ld a, b
    ld [rLYC], a
    ld a, %11100100
    ld [rBGP], a
skip_a:
    reti