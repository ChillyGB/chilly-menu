# Chillycart mapper

max ROM size is 4MB.

## Memory

### 0000-3FFF - ROM Bank 00 (Read Only)

Contains the first 16KiB of the ROM.

### 4000-7FFF - ROM bank 00-FF (Read Only)

Same as MBC5.

## Registers

### 2000-3FFF ROM Bank Number (Write Only)

Here goes the bank number. Writing $0 will read bank $0 just like MBC5.

**NOTE**: chilly-menu currently doesn't use this register.

### A000 Load selected (Read/Write)

Writing $0 to this address will load the selected ROM (and save) or opens the directory.
Writing $1 to this address will go back by a directory.
Writing $ff to this address will load the last used ROM (and save).

it will read $0 if the current selection is a gb or gbc file.
it will read $1 if the current selection is a directory.
it will read $ff if the current selection is not a valid rom.

**WARNING**: Writing to this register can clear the ROM content. Always check if the current selection is a directory or a file before writing 0x0 here. If the selected item is a file: disable interrupts and execute code from WRAM or HRAM.

### A002 Copy file list (Read/Write)

Reads $0 when is copying to B000-B1BF.
Else will read the number of pages of the current directory.

Writing the value X will copy to B000-B1BF the page X of the file list.

### A004 Load RTC (Write Only)

Writing any values to this address will copy the current RTC time to A010-A015.

### A005 Save RTC (Write Only)

Writing any values to this address will copy the RTC values from A010-A015 to RTC.

### A006 Load Cheats (Write Only)

Writing any values to this address will copy the cheats code of the current selected game to A300-A390.

### A007 Save Cheats (Write Only)

Writing any values to this address will copy the cheats code from A300-A390 to SD Cart.

### A010-A015 RTC Registers (Read/Write)

- **A010:** Year
- **A011:** Month
- **A012:** Day
- **A013:** Hours
- **A014:** Minutes
- **A015:** Seconds

### A100 Current file selection (Read/Write)

Index of the current selected file (or directory) in the current page.

### A200-A213 Firmware string (Read Only)

A 20 chars string formatted in ascii that can be used to get the firmware version.

### A300-A390 Latched cheats (Read/Write)

Contains a list of 16 cheats codes. A cheat is represented by 9 values ranged from $0 to $10. Every value is a digit of the gamegenie code. A value of $10 means that that digit wasn't set. Any cheat code that contains $10 is considered invalid and wont be loaded.

### B000-B1BF File list (Read Only)

Contains a list of 14 files. Every file is rappresent with a string of 32 bytes formatted in ascii.
