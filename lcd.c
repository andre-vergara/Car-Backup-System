#include "lcd.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdint.h>

#define LCD_RS 0x01
#define LCD_ENABLE 0x04
#define LCD_BACKLIGHT 0x08

static int lcd_backpack_write(uint8_t val){
    return i2c_write_blocking(
        i2c0,   // I2C controller
        0x27,   // backpack address
        &val,   // address of the byte
        1,      // send one byte
        false   // finish with an I2C STOP
    );
}

static void lcd_send_nibble(uint8_t nibble, bool data){
    uint8_t output;
    output = nibble & 0x0F;
    output = output << 4;
    output = output | LCD_BACKLIGHT;

    if (data){
        output = output | LCD_RS;
    }

    lcd_backpack_write(output); // Enable is low

    lcd_backpack_write(output | LCD_ENABLE); // raises Enable
    sleep_us(1);

    lcd_backpack_write(output); // lowers Enable
    sleep_us(50);
}

static void lcd_send_byte(uint8_t val, bool data){
    uint8_t high_nibble, low_nibble;
    high_nibble = val >> 4;
    low_nibble = val & 0x0F;

    lcd_send_nibble(high_nibble, data);

    lcd_send_nibble(low_nibble, data);

}

static void lcd_send_command(uint8_t command){
    lcd_send_byte(command, false);
}

static void lcd_send_character(uint8_t character){
    lcd_send_byte(character, true);
}

void lcd_init(void){
    sleep_ms(50);

    lcd_send_nibble(0x03, false);
    sleep_ms(5);

    lcd_send_nibble(0x03, false);
    sleep_us(150);

    lcd_send_nibble(0x03, false);
    sleep_us(150);

    lcd_send_nibble(0x02, false);
    sleep_us(50);

    lcd_send_command(0x28);
    lcd_send_command(0x08);
    lcd_send_command(0x01);

    sleep_ms(2);

    lcd_send_command(0x06);
    lcd_send_command(0x0C);
}

void lcd_clear(void){
    lcd_send_command(0x01);
    sleep_ms(2);
}

void lcd_set_cursor(int row, int col){
    if (row < 0 || row > 1){
        return;
    }
    if (col < 0 || col > 15){
        return;
    }

    uint8_t address;

    if (row == 0){
        address = 0x00 + col;
    }else { address = 0x40 + col; }

    address = address | 0x80;
    lcd_send_command(address);
}

void lcd_print(const char *text){
    while (*text != '\0'){
        lcd_send_character(*text);
        text++;     // move text forward to the next character
    }
}
