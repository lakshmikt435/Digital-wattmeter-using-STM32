#include "lcd_i2c.h"
#include "stm32f4xx_hal.h"
#include "string.h"

static void LCD_Write4Bits(LCD_HandleTypeDef *lcd, uint8_t data) {
    uint8_t buf = data | lcd->backlight;
    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address << 1, &buf, 1, HAL_MAX_DELAY);

    // Enable pulse
    buf |= LCD_ENABLE;
    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address << 1, &buf, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    buf &= ~LCD_ENABLE;
    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address << 1, &buf, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
}

static void LCD_Send(LCD_HandleTypeDef *lcd, uint8_t value, uint8_t mode) {
    LCD_Write4Bits(lcd, (value & 0xF0) | mode);
    LCD_Write4Bits(lcd, ((value << 4) & 0xF0) | mode);
}

void LCD_Init(LCD_HandleTypeDef *lcd, I2C_HandleTypeDef *hi2c, uint8_t addr) {
    lcd->hi2c = hi2c;
    lcd->address = addr;
    lcd->backlight = LCD_BACKLIGHT;

    HAL_Delay(50); // wait for LCD power up

    LCD_Write4Bits(lcd, 0x30);
    HAL_Delay(5);
    LCD_Write4Bits(lcd, 0x30);
    HAL_Delay(1);
    LCD_Write4Bits(lcd, 0x30);
    HAL_Delay(1);
    LCD_Write4Bits(lcd, 0x20); // 4-bit mode

    LCD_Send(lcd, 0x28, 0); // 2 lines, 5x8 font
    LCD_Send(lcd, 0x0C, 0); // display ON, cursor OFF
    LCD_Send(lcd, 0x06, 0); // entry mode
    LCD_Clear(lcd);
}

void LCD_Clear(LCD_HandleTypeDef *lcd) {
    LCD_Send(lcd, 0x01, 0);
    HAL_Delay(2);
}

void LCD_SetCursor(LCD_HandleTypeDef *lcd, uint8_t row, uint8_t col) {
    const uint8_t row_offsets[] = {0x00, 0x40};
    if(row >= LCD_ROWS) row = 0;
    LCD_Send(lcd, 0x80 | (col + row_offsets[row]), 0);
}

void LCD_Print(LCD_HandleTypeDef *lcd, char *str) {
    while(*str) {
        LCD_Send(lcd, *str++, LCD_REGSELECT);
    }
}

void LCD_Backlight(LCD_HandleTypeDef *lcd, uint8_t state) {
    lcd->backlight = state ? LCD_BACKLIGHT : LCD_NOBACKLIGHT;
    uint8_t data = lcd->backlight;
    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address << 1, &data, 1, HAL_MAX_DELAY);
}
