#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#include "stm32f4xx_hal.h"

#define LCD_COLS 16
#define LCD_ROWS 2

#define LCD_BACKLIGHT   0x08
#define LCD_NOBACKLIGHT 0x00
#define LCD_REGSELECT   0x01
#define LCD_ENABLE      0x04

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t address;
    uint8_t backlight;
} LCD_HandleTypeDef;

void LCD_Init(LCD_HandleTypeDef *lcd, I2C_HandleTypeDef *hi2c, uint8_t addr);
void LCD_Clear(LCD_HandleTypeDef *lcd);
void LCD_SetCursor(LCD_HandleTypeDef *lcd, uint8_t row, uint8_t col);
void LCD_Print(LCD_HandleTypeDef *lcd, char *str);
void LCD_Backlight(LCD_HandleTypeDef *lcd, uint8_t state);

#endif
