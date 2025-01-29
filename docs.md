# Chillycart mapper

ROM size is 64KB

## Memory

### 0000-3FFF - ROM Bank 00 (Read Only)

Contains the first 16KiB of the ROM.

### 4000-7FFF - ROM bank 00-03 (Read Only)

Same as MBC5.

## Registers

### 2000-3FFF ROM Bank Number (Write Only)

Here go the bank number. Only the 2 least significant bit are checked. Writing 0 will give bank 0 like MBC5.

### A000 Load selected (Read/Write)

Writing 0 to this address will load the selected ROM (and save) or opens the directory
Writing 1 to this address will go back by a directory
Writing 0xff to this address will load the last used ROM (and save)

it will read 0 if the current selection is a gb or gbc file
it will read 1 if the current selection is a directory
it will read 0xff if the current selection is not a valid rom

WARNING: Write to this register can clear the rom content. Always check if the current selection is a directory or a file before writing 0x0 here. If the selected item is file disable interrupts and execute code from WRAM or HRAM

### A001 Save copy to SD card (Read/Write)

Reads $0 when Save content was copied to SD card at least one time
Reads $1 when Save content was not copied to SD card
Reads $FF when Save is backupping
Writing any values will start the current save to SD card

### A002 Copy file list (Read/Write)

Reads $0 when is copying to B000-B1BF
Else will read the number of pages of the current directory

Writing the value X will copy to B000-B1BF the page X of the file list

### A004 Load RTC (Write Only)

Writing any values to this address will copy the current RTC time to A010-A015

### A005 Save RTC (Write Only)

Writing any values to this address will copy the RTC values to RTC

### A010-A015 RTC Registers (Read/Write)

**A010:** Year
**A011:** Month
**A012:** Day
**A013:** Hours
**A014:** Minutes
**A015:** Seconds

### A100 Current ROM selection (Read/Write)

index of the current rom in the current page

### B000-B1BF File list (Read Only)

Contains a list of 14 files. Every file is rappresent with a string of 32 bytes formatted in ascii
