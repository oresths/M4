/*
#include "mbed.h"

#define I2C_THERM_ADDR 0x0E	///Thermistor Register Starting Address
#define I2C_TEMP_ADDR 0x80	///Temperature Registers Starting Address

#define PIXELS_COUNT 64

Serial pc(USBTX, USBRX);

//I2C i2c0(p32, p31);	//sda, scl

uint8_t i2c_addr_GND = 0b1101000 << 1;	//mbed accepts the oversimplified wrong address type

int main() {
//	i2c0.frequency(400000);

	char cmd[2];

	union {
		char thermistor_echo[2];	//1 LSB = 0.0625 C , result 12-bit as signed absolute value
		uint16_t therm_echo_uint16;	//little endian
	};
//	thermistor_echo[0] = 0x90;
//	thermistor_echo[1] = 0x01;	//prepei 25C
	thermistor_echo[0] = 0x40;
	thermistor_echo[1] = 0x09;	//prepei -20C
	float thermistor_value;

	union {
		char temp_echo[2*PIXELS_COUNT];	//1 LSB = 0.25 C , result 12-bit as 2's complement
		uint16_t temp_echo_uint16[PIXELS_COUNT];	//little endian
	};
	temp_echo[0] = 0xF4;
	temp_echo[1] = 0x01;	//prepei 125C
	temp_echo[2] = 0x9C;
	temp_echo[3] = 0x0F;	//prepei -25C
	float temp_values[PIXELS_COUNT];

	while (1) {
		wait_ms(100);

		pc.printf("GridEye\r\n");

		cmd[0] = I2C_THERM_ADDR;
//		i2c0.write(i2c_addr_GND, cmd, 1, 1);
//		i2c0.read(i2c_addr_GND, thermistor_echo, 2);

		if (therm_echo_uint16 & 0x800) {  //if negative
			thermistor_value = - 0.0625 * (0x7FF & therm_echo_uint16);
		} else {	//else if positive
			thermistor_value = 0.0625 * (0x7FF & therm_echo_uint16);
		}

		pc.printf("Termistor Temp = %f\r\n", thermistor_value);

		cmd[0] = I2C_TEMP_ADDR;
//		i2c0.write(i2c_addr_GND, cmd, 1, 1);
//		i2c0.read(i2c_addr_GND, temp_echo, 2*PIXELS_COUNT);

		for (int i = 0; i < PIXELS_COUNT; ++i) {
			if (temp_echo_uint16[i] & 0x800) {  //if negative
				temp_values[i] = 0.25 * (int16_t)(0xF000 | temp_echo_uint16[i]);
			} else {	//else if positive
				temp_values[i] = 0.25 * (0x7FF & temp_echo_uint16[i]);
			}
		}

		pc.printf("Grid Tempp =\r\n");

		wait(1);
	}
}
*/
