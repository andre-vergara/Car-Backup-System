- Give hints and pseudocode first
- Do not provide full code unless explicitly asked
- Explain embedded logic and Pico SDK functions
- Preserve my learning process

# LCD Context

I am working on a Raspberry Pi Pico 2 W car-backup / proximity-warning project in C.

## LCD Hardware

- LCD: QAPASS 1602A
- Type: 16x2 character LCD
- Interface: I2C backpack
- I2C address: 0x27
- SDA: GP4
- SCL/SCK: GP5
- I2C controller: i2c0
- I2C speed: 100 kHz

The Pico project already links `hardware_i2c`.

## Current Project Structure

- `main.c` = main application logic
- `lcd.c` = LCD implementation / driver code
- `lcd.h` = LCD function declarations

`lcd.h` currently exposes functions conceptually like:

- `lcd_init()`
- `lcd_clear()`
- `lcd_set_cursor(row, col)`
- `lcd_print(text)`

## What I Have Already Done

I successfully initialized the I2C bus using GP4/GP5 and ran an I2C scanner.

The LCD responded at:

0x27

So the I2C wiring and device address are confirmed working.

## What I Need To Build Next

I want to implement the LCD driver in `lcd.c`.

The planned helper-function structure is:

- low-level backpack write
- send 4-bit nibble
- send full byte
- send LCD command
- send LCD character/data
- initialize LCD
- clear LCD
- set cursor
- print a C string

The LCD needs to be initialized for:

- 4-bit mode
- 2 display lines
- display ON
- cursor OFF
- blink OFF
- cursor moves right after each character

## Important Teaching Preference

I am learning embedded C and want to write the code myself.

Do NOT give me the entire LCD driver unless I explicitly ask for it.

Instead:

- guide me one function at a time
- give detailed embedded-style pseudocode
- tell me which Pico SDK functions are relevant
- explain the purpose of each bit/command
- let me attempt the implementation
- review and debug what I write
- explain why something is wrong before giving the fix

Start with the lowest-level LCD helper needed to communicate with the I2C backpack.