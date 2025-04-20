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
    ld hl, rSELECTED_ROM
    ld b, [hl]
    inc b
    inc b
    sla b
    sla b
    sla b
    dec b

    ld hl, IS_GBC
    ld a, [hl]
    and a
    jp nz, stat_cgb
    
stat_dmg:
    ld a, [rSTAT]
    and 3
    and a
    jr nz, stat_dmg

    ld a, [rLYC]
    cp a, b
    jp nz, not_first_lyc_dmg

    ld a, b
    add a, 8
    ld [rLYC], a
    ld a, [SCX_SELECTED]
    ld [rSCX], a
    ld a, %11101000
    ld [rBGP], a
    reti

not_first_lyc_dmg:
    ld hl, CURRENT_SELECTION_LONG
    ld a, [hl]
    and a
    jp nz, dont_increment_scx_dmg
    ld hl, SCX_SELECTED
    inc [hl]
dont_increment_scx_dmg:
    ld a, 0
    ld [rSCX], a
    ld a, b
    ld [rLYC], a
    ld a, %11100100
    ld [rBGP], a
    reti

stat_cgb:
    ld a, [rSTAT]
    and 3
    and a
    jr nz, stat_cgb


    ld a, [rLYC]
    cp a, b
    jp nz, not_first_lyc_cgb
	set_palette_bg $ff, $21, $8c, 1
    ld a, b
    add a, 8
    ld [rLYC], a
    ld a, [SCX_SELECTED]
    ld [rSCX], a
    reti

not_first_lyc_cgb:
    ld hl, CURRENT_SELECTION_LONG
    ld a, [hl]
    and a
    jp nz, dont_increment_scx_cgb
    ld hl, SCX_SELECTED
    inc [hl]
dont_increment_scx_cgb:

	set_palette_bg $63, $a5, $ff, 1
    ld a, 0
    ld [rSCX], a
    ld a, b
    ld [rLYC], a
    reti