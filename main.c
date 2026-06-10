#include "stm32f4xx_hal.h"
#include "lcd_i2c.h"
#include <stdio.h>

I2C_HandleTypeDef hi2c1;
LCD_HandleTypeDef lcd;
ADC_HandleTypeDef hadc1;

uint16_t read;
float result;
uint16_t current_raw;
int32_t current_mA;

void I2C1_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    HAL_I2C_Init(&hi2c1);
}

void ADC1_Init(void) {
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;     // <-- FIXED: PA1 added
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    HAL_ADC_Init(&hadc1);
}

void ADC_Select(uint32_t ch)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ch;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

void GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

// -----------------------------------
int main(void)
{
      HAL_Init();
    GPIO_Init();
    I2C1_Init();
    ADC1_Init();

    LCD_Init(&lcd, &hi2c1, 0x27);     // <-- MUST BE CORRECT
    LCD_Clear(&lcd);

    char buffer[16];

    while(1)
    {
        // ---- Read PA0 ----
    	// Read PA0
    	ADC_Select(ADC_CHANNEL_0);
    	HAL_ADC_Start(&hadc1);
    	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    	read = HAL_ADC_GetValue(&hadc1);

    	// Read PA1
    	ADC_Select(ADC_CHANNEL_1);
    	HAL_ADC_Start(&hadc1);
    	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        current_raw = HAL_ADC_GetValue(&hadc1);

        float pa1_mV = (current_raw * 3300.0f) / 4095.0f;
        float acs712_out = pa1_mV * 2.0f;
        float diff = acs712_out - 2500.0f;
        current_mA = (diff / 185.0f);

        result = (read * 3300) / 4095;
        // ---- Display ----
        LCD_SetCursor(&lcd, 0, 0);
        sprintf(buffer, "I:%d mA", current_mA);
        LCD_Print(&lcd, buffer);

        LCD_SetCursor(&lcd, 1, 0);
        sprintf(buffer, "V:%4u mV", (uint16_t)result);
        LCD_Print(&lcd, buffer);

        HAL_Delay(500);
    }
}
