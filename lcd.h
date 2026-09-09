#pragma once
#include <stdio.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(int row, int col);
void lcd_print(const char *text);