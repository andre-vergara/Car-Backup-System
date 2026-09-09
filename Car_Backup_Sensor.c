#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd.h"

// define pins
#define ECHO_PIN 0
#define TRIG_PIN 1
#define LCD_SDA 4
#define LCD_SCL 5
#define BUZZER 6
#define LCD_I2C i2c0
#define LCD_ADDRESS 0x27
// VBUS 5 rail
// GND GND
// 3V3 3.3 rail

int main()
{
    stdio_init_all();

    i2c_init(LCD_I2C, 100000);
    gpio_set_function(LCD_SDA, GPIO_FUNC_I2C);
    gpio_set_function(LCD_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(LCD_SDA);
    gpio_pull_up(LCD_SCL);

    sleep_ms(500);

    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_put(BUZZER, 0);

    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_put(TRIG_PIN, 0);

    lcd_init();

    char lcd_line[17];

    while (true)
    {
        uint64_t pulse_duration;
        uint64_t start_time, end_time;
        float distance;

        gpio_put(TRIG_PIN, 0); // low
        sleep_us(5);

        gpio_put(TRIG_PIN, 1); // high
        sleep_us(10);
        
        gpio_put(TRIG_PIN, 0); // low

        absolute_time_t echo_deadline = make_timeout_time_ms(30); // timeout prevents system freezing
        bool echo_started = false;  

        while (!time_reached(echo_deadline))
        {
            if (gpio_get(ECHO_PIN))
            {
                echo_started = true;
                break;
            }
            tight_loop_contents();
        }

        if (!echo_started)
        {
            printf("ERROR: ECHO never went HIGH\n");
            gpio_put(BUZZER, 0);
            lcd_set_cursor(0, 0);
            lcd_print("NO ECHO HIGH    ");
            lcd_set_cursor(1, 0);
            lcd_print("CHECK GP0/TRIG  ");
            sleep_ms(500);
            continue;
        }

        start_time = time_us_64(); // start time

        echo_deadline = make_timeout_time_ms(30);
        bool echo_finished = false;

        while (!time_reached(echo_deadline))
        {
            if (!gpio_get(ECHO_PIN))
            {
                echo_finished = true;
                break;
            }
            tight_loop_contents();
        }

        if (!echo_finished)
        {
            printf("ERROR: ECHO stayed HIGH\n");
            gpio_put(BUZZER, 0);
            lcd_set_cursor(0, 0);
            lcd_print("ECHO STUCK HIGH ");
            lcd_set_cursor(1, 0);
            lcd_print("CHECK GP0       ");
            sleep_ms(500);
            continue;
        }
        
        end_time = time_us_64(); // end time

        pulse_duration = end_time - start_time;

        distance = (pulse_duration * 0.0343) / 2;
        snprintf(
            lcd_line,               // destination string
            sizeof lcd_line,        // max buffer size
            "Distance:%5.1fcm ",    // formatting pattern
            distance                // value inserted at  %f
        );

        printf("Distance: %.2f cm\n", distance);
        lcd_set_cursor(0,0);
        lcd_print(lcd_line);
            
        lcd_set_cursor(1,0);
        if (distance <= 3){
            gpio_put(BUZZER, 1);
            lcd_print("STOP            ");
        }
        else if (distance <= 5){
            gpio_put(BUZZER, 1);
            lcd_print("DANGER          ");
            sleep_ms(75);
            gpio_put(BUZZER, 0);
            sleep_ms(75);
        } 
        else if (distance <= 10){
            gpio_put(BUZZER, 1);
            lcd_print("CAREFUL         ");
            sleep_ms(150);
            gpio_put(BUZZER, 0);
            sleep_ms(150);
        }
        else { 
            gpio_put(BUZZER, 0); 
            lcd_print("CLEAR           ");
        }

        sleep_ms(250);
    }

}
