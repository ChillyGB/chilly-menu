MACRO wait_vblank
	push af
.loop\@
	ld a, [rLY]
	cp 144
	jr c, .loop\@
	pop af
ENDM

MACRO memcpy
	ld hl, \1
	ld bc, \2
	ld de, \3
.loop\@
	ld a, [bc]
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jr nz, .loop\@
ENDM

MACRO memcpy_8
	ld hl, \1
	ld bc, \2
	ld d, \3
.loop\@
	ld a, [bc]
	ld [hli], a
	inc bc
	dec d
	jr nz, .loop\@
ENDM

MACRO memset
	ld hl, \1
	ld de, \2
.loop\@
	ld a, \3
	ld [hli], a
	inc bc
	dec de
	ld a, d
	or a, e
	jr nz, .loop\@
ENDM

MACRO memcpy_inc
	ld hl, \1
	ld bc, \2
	ld d, \3
.loop\@
	ld a, [bc]
	inc a
	ld [hli], a
	inc bc
	dec d
	jr nz, .loop\@
ENDM

MACRO memcpy_dec
	ld hl, \1
	ld bc, \2
	ld d, \3
.loop\@
	ld a, [bc]
	dec a
	ld [hli], a
	inc bc
	dec d
	jr nz, .loop\@
ENDM

MACRO set_palette_bg
	ld hl, rBCPS
	ld [hl], $80 + (\4 << 1)
	ld hl, rBCPD
	ld [hl], ((((\2 >> 3) << 5) & $e0) | (\1 >> 3))
	ld [hl], ((\2 >> 6) | ((\3 >> 3) << 2))
ENDM

MACRO set_palette_obj
	ld hl, rOCPS
	ld [hl], $80 + (\4 << 1)
	ld hl, rOCPD
	ld [hl], ((((\2 >> 3) << 5) & $e0) | (\1 >> 3))
	ld [hl], ((\2 >> 6) | ((\3 >> 3) << 2))
ENDM