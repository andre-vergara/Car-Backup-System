# Raspberry Pi Pico 2 W Car Backup Sensor

This project is a proximity-warning system built with a Raspberry Pi Pico 2 W. It repeatedly measures the distance to an object with an ultrasonic sensor, displays the result on a 16x2 character LCD, and drives a buzzer with different warning patterns as the measured distance decreases.

The program also monitors the ultrasonic sensor's Echo signal for two failure conditions. If Echo never goes high or remains high for too long, the buzzer is disabled and a diagnostic message is shown on both the LCD and the USB serial output.

## Hardware Used

- Raspberry Pi Pico 2 W
- Ultrasonic distance sensor with Trigger and Echo signals
- QAPASS 1602A 16x2 character LCD
- I2C backpack for the LCD, configured at address `0x27`
- Buzzer controlled by a GPIO output
- Breadboard and jumper wiring

## Pin Mapping

| Device signal | Pico connection | Purpose |
| --- | --- | --- |
| Ultrasonic Echo | GP0 | Receives the timed Echo pulse |
| Ultrasonic Trigger | GP1 | Sends the measurement trigger pulse |
| LCD SDA | GP4 | I2C data using `i2c0` |
| LCD SCL | GP5 | I2C clock using `i2c0` |
| Buzzer signal | GP6 | Turns the buzzer on and off |

The LCD uses a 100 kHz I2C bus. All devices must share a common ground and must be powered according to their hardware requirements.

## Ultrasonic Distance Measurement

Each measurement begins with a short Trigger sequence:

1. Drive Trigger low for 5 microseconds.
2. Drive Trigger high for 10 microseconds.
3. Drive Trigger low again.
4. Wait for Echo to rise and record the start time.
5. Wait for Echo to fall and record the end time.

The Echo pulse duration is measured with the Pico SDK's microsecond timer. Distance is then calculated in centimeters:

distance = (pulse duration in microseconds * 0.0343) / 2

The factor `0.0343` represents the approximate speed of sound in centimeters per microsecond. Division by two accounts for the sound travelling to the object and returning to the sensor.

Both Echo wait loops have a 30 ms timeout so a missing or stuck signal cannot block the entire application indefinitely. Valid distances are also printed over USB serial with two decimal places.

## LCD Display

The LCD driver in `lcd.c` communicates with the display through its I2C backpack. It initializes the display in 4-bit, two-line mode with the display enabled and the cursor and cursor blinking disabled.

The first row displays the measured distance with one decimal place:

Distance: 13.0cm

The second row displays the current warning state: `STOP`, `DANGER`, `CAREFUL`, or `CLEAR`.

The driver provides these public functions:

- `lcd_init()`
- `lcd_clear()`
- `lcd_set_cursor(row, col)`
- `lcd_print(text)`

It also contains private helper functions for writing to the I2C backpack, transferring 4-bit nibbles, sending complete bytes, and selecting between LCD commands and character data.

## Buzzer Warning Behavior

| Measured distance | LCD status | Buzzer behavior |
| --- | --- | --- |
| 3 cm or less | `STOP` | Continuously on |
| More than 3 cm and up to 5 cm | `DANGER` | 75 ms on, then 75 ms off |
| More than 5 cm and up to 10 cm | `CAREFUL` | 150 ms on, then 150 ms off |
| More than 10 cm | `CLEAR` | Off |

The main loop waits an additional 250 ms after handling each valid measurement.

## Error Handling

If Echo does not rise within 30 ms, the LCD displays:

NO ECHO HIGH
CHECK GP0/TRIG

If Echo rises but does not fall within 30 ms, the LCD displays:

ECHO STUCK HIGH
CHECK GP0

In either case, the buzzer is switched off, the same condition is reported through USB serial, and the program waits 500 ms before trying another measurement.

## Project Structure

| File | Description |
| --- | --- |
| `Car_Backup_Sensor.c` | GPIO and I2C setup, ultrasonic timing, distance calculation, LCD updates, buzzer control, and error handling |
| `lcd.c` | I2C backpack and 4-bit LCD implementation |
| `lcd.h` | Public LCD function declarations |
| `CMakeLists.txt` | Pico SDK build configuration and linked libraries |
| `pico_sdk_import.cmake` | Pico SDK import helper |

## Build and Flash Notes

The project is configured for `pico2`, uses C11, and links `pico_stdlib` and `hardware_i2c`. USB standard I/O is enabled and UART standard I/O is disabled.

With the Raspberry Pi Pico SDK and its toolchain configured, generate and build the project with CMake:

```sh
cmake -S . -B build
cmake --build build
```

The Pico SDK build generates additional output formats, including the UF2 file used to flash the board. To flash it, connect the Pico while holding its BOOTSEL button and copy `build/Car_Backup_Sensor.uf2` to the mounted board. Distance readings and Echo error messages can then be viewed through the Pico's USB serial connection.

The checked-in CMake configuration references Pico SDK version 2.3.1 and sets the program version to 0.1.

## What I Learned

This project provided practical experience with:

- Configuring GPIO pins as digital inputs and outputs with the Pico SDK
- Producing a microsecond Trigger pulse and timing an Echo pulse
- Converting pulse duration into an ultrasonic distance measurement
- Adding timeouts so hardware faults do not lock the main loop
- Initializing and using the Pico's I2C peripheral
- Building a 4-bit character LCD driver one communication layer at a time
- Positioning and printing text on a fixed-width 16x2 display
- Formatting numeric values safely with `snprintf`
- Translating measured distance ranges into visual and audible warning states
- Separating reusable hardware-driver code from the main application logic
