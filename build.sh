#!/bin/sh

mkdir -p "build/obj"

rgbgfx -o ./inc/img/chars.2bpp ./inc/img/chars.png

function build_rom () {
    rgbasm -o "./build/obj/$1.o" "./src/$1.asm"
    rgblink -o "./build/$1.gb" "./build/obj/$1.o"
    rgbfix -v -p 0xFF -m 0x57 "./build/$1.gb"
}

build_rom "boot"
