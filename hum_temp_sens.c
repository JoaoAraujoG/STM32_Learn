#include "hum_tem_sens.h"

#define ADDR 0x40 												// Temperature / humidity sensor i2c address

#define HM_READ_REL_HUM	0xE5									//Measure Relative Humidity, Hold Master Mode 0xE5
#define NHM_READ_REL_HUM 0xF5									// Measure Relative Humidity, No Hold Master Mode 0xF5
#define HM_READ_TEMP 0xE3										// Measure Temperature, Hold Master Mode 0xE3
#define NHM_READ_TEMP	0xF3										// Measure Temperature, No Hold Master Mode 0xF3
#define READ_PREV_TEMP_VAL 0xE0								// Read Temperature Value from Previous RH Measurement 0xE0
#define RESET	0xFE														// Reset 0xFE
#define WRITE_USER_REG 0xE6										// Write RH/T User Register 1 0xE6
#define READ_USER_REG	0xE7										// Read RH/T User Register 1 0xE7
#define WRITE_HC_REG 0x51											// Write Heater Control Register 0x51
#define READ_HC_REG	0x11											// Read Heater Control Register 0x11
#define READ_EID_B1												// Read Electronic ID 1st Byte 0xFA 0x0F
#define READ_EID_B2												// Read Electronic ID 2nd Byte 0xFC 0xC9
#define READ_FH_REV												// Read Firmware Revision 0x84 0xB8

I2C_HandleTypeDef* i2c_addr = NULL;		// I2C address value

void init_RH_Temp_sens(I2C_HandleTypeDef* hi2c)
{
	i2c_addr = hi2c;
}

uint16_t get_raw_measurement(uint8_t command) // Gets the raw measurement of a fisica variable mesured by the sensor
{
	
	uint8_t buffer[2];							// Holds two bytes read from slave
	uint16_t raw_measurement = 0;		// Holds the raw mesurement
	
	HAL_I2C_Master_Transmit(i2c_addr, ADDR << 1, &command, 1, 100);		// Sends a command to slave to initialize convertion
	HAL_Delay(100);																									  // Waits for the conversion to be made
	HAL_I2C_Master_Receive(i2c_addr, 0x40 << 1, buffer, 2, 100);		  // Sends read request to slave and saves the 2Bytes returned
	raw_measurement = buffer[0] << 8 | buffer[1];            // Raw mesurement value returned by the sensor
	
	return raw_measurement;
}

float get_temperature()
{
	uint16_t raw_temperature = get_raw_measurement(HM_READ_TEMP);		// Holds the raw temperature
	float temperature = (175.72 * raw_temperature)/65536 - 46.85;		// Raw temperature value converted to ºC
	
	return temperature;
}

float get_humidity()
{
	uint16_t raw_relative_humidity = get_raw_measurement(HM_READ_REL_HUM);		// Holds the raw temperature
	float relative_humidity = (125 * raw_relative_humidity)/65536 - 6;				// Raw temperature value converted to ºC
	
	return relative_humidity;
}


