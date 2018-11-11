#include "..\Drivers\STM32F7xx_HAL_Driver\Inc\stm32f7xx_hal.h"

void lcd_init (I2C_HandleTypeDef* i2c_addr);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_set_cursor(unsigned char col, unsigned char row); // sets cursor position

void lcd_display(void);

void lcd_blink(void);

void lcd_no_blink(void);

void lcd_cursor(void);

void lcd_no_cursor(void);

void lcd_home(void);

void lcd_clear(void);
