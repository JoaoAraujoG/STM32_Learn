#include "i2c-lcd.h"

I2C_HandleTypeDef* i2c_address = NULL;;  // Pointer to i2c

#define SLAVE_ADDRESS_LCD 0x27 // change this according to ur setup

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 4  // Enable bit
#define Rw 2 // Read/Write bit
#define Rs 1 // Register select bit


void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit (i2c_address, SLAVE_ADDRESS_LCD << 1,(uint8_t *) data_t, 4, 100);
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	HAL_I2C_Master_Transmit (i2c_address, SLAVE_ADDRESS_LCD << 1,(uint8_t *) data_t, 4, 100);
}

void lcd_send_string (char *str)
{
	while(!(*str == '\r' || *str == '\0')) 
	{
		lcd_send_data (*str++);
	}
}

void lcd_set_cursor(unsigned char col, unsigned char row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	lcd_send_cmd(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void lcd_clear(){
	lcd_send_cmd(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	HAL_Delay(2000);
}

void lcd_home()
{
	lcd_send_cmd(LCD_RETURNHOME);  // set cursor position to zero
	HAL_Delay(2000);
}

// Turns the underline cursor on/off
void lcd_no_cursor() 
{
	lcd_send_cmd(LCD_DISPLAYCONTROL | ~LCD_CURSORON);
}

void lcd_cursor() {
	lcd_send_cmd(LCD_DISPLAYCONTROL | LCD_CURSORON);
}

// Turn on and off the blinking cursor
void lcd_no_blink() {
	
	lcd_send_cmd(LCD_DISPLAYCONTROL | ~LCD_BLINKON);
}
void lcd_blink() {
	
	lcd_send_cmd(LCD_DISPLAYCONTROL | LCD_BLINKON);
}

void lcd_display() {
	lcd_send_cmd(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
}

void lcd_init (I2C_HandleTypeDef* i2c_addr)
{
	
	i2c_address = i2c_addr;
	
	HAL_Delay(50);
	uint8_t reset = LCD_DISPLAYCONTROL | LCD_NOBACKLIGHT;
	
	HAL_I2C_Master_Transmit(i2c_address, SLAVE_ADDRESS_LCD << 1,&reset, 1, 100);
	HAL_Delay(1000);
	
	lcd_send_cmd (LCD_FUNCTIONSET | LCD_8BITMODE | LCD_2LINE | LCD_5x8DOTS);		//	Infoms the lcd that we're using a 2 line dysplay 0x28
	lcd_send_cmd (LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);		//	Dysplay off, blinking off, cursor off
	
	lcd_clear();
	
	lcd_send_cmd (LCD_ENTRYMODESET | LCD_ENTRYSHIFTINCREMENT | LCD_ENTRYRIGHT);		//	Set entry mode to no shift and
																																								//	increment by 1
	
	lcd_home();
	
	
}