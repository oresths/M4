#include "mbed.h"
#include "rtos.h"
#include "cmsis_os.h"

#define I2C_THERM_ADDR 0x0E	///Thermistor Register Starting Address
#define I2C_TEMP_ADDR 0x80	///Temperature Registers Starting Address

#define PIXELS_COUNT 64

#define GREEN 0x1C
#define RED 0xE0
#define ORANGE 0xFC

#define I2C_IRQN I2C0_IRQn
#define I2C_QUEUE I2C0_queue
#define I2C_OBJECT i2c0

char ledArray [64];
int celsius;

SPI RGB_LEDArray(p5, p6, p7); /// mosi, miso, sclk
DigitalOut SPI_ss(p8);	///Slave Select

Serial pc(USBTX, USBRX);

I2C I2C_OBJECT(p9, p10);	//sda, scl

uint8_t i2c_addr_GND = 0b1101000 << 1;	//mbed accepts the oversimplified wrong address type

osMessageQDef(I2C_QUEUE, 1, uint32_t);
osMessageQId  I2C_QUEUE;
//extern osMessageQId  I2C1_queue;

int main() {
	SPI_ss = 1;	//Make sure the RG matrix is deactivated, maybe this should be first line executed
	RGB_LEDArray.format(8,0);
	RGB_LEDArray.frequency(125000);

	I2C_QUEUE = osMessageCreate(osMessageQ(I2C_QUEUE), NULL);

	I2C_OBJECT.frequency(400000);
	NVIC_EnableIRQ(I2C_IRQN);

	char cmd[2];

	union {
		char thermistor_echo[2];	//1 LSB = 0.0625 C , result 12-bit as signed absolute value
		uint16_t therm_echo_uint16;	//little endian
	};
	float thermistor_value;

	union {
		char temp_echo[2*PIXELS_COUNT];	//1 LSB = 0.25 C , result 12-bit as 2's complement
		uint16_t temp_echo_uint16[PIXELS_COUNT];	//little endian
	};
	float temp_values[PIXELS_COUNT];

	while (1) {
		wait_ms(100);

		pc.printf("GridEye\r\n");

		cmd[0] = I2C_THERM_ADDR;
		I2C_OBJECT.write(i2c_addr_GND, cmd, 1, 1);
		I2C_OBJECT.read(i2c_addr_GND, thermistor_echo, 2);

		if (therm_echo_uint16 & 0x800) {  //if negative
			thermistor_value = - 0.0625 * (0x7FF & therm_echo_uint16);
		} else {	//else if positive
			thermistor_value = 0.0625 * (0x7FF & therm_echo_uint16);
		}

		pc.printf("Termistor Temp = %f\r\n", thermistor_value);

		cmd[0] = I2C_TEMP_ADDR;
		I2C_OBJECT.write(i2c_addr_GND, cmd, 1, 1);
		I2C_OBJECT.read(i2c_addr_GND, temp_echo, 2*PIXELS_COUNT);

		for (int i = 0; i < PIXELS_COUNT; ++i) {
			if (temp_echo_uint16[i] & 0x800) {  //if negative
				temp_values[i] = 0.25 * (int16_t)(0xF000 | temp_echo_uint16[i]);
			} else {	//else if positive
				temp_values[i] = 0.25 * (0x7FF & temp_echo_uint16[i]);
			}
		}

		pc.printf("Grid Temp =\r\n");

		//Determine LED Color for Pixel
		for (int pixel = 0; pixel < PIXELS_COUNT; ++pixel) {
			celsius = temp_values[pixel];
			if (celsius < 26) {
				ledArray[pixel] = GREEN;
			} else if (celsius >= 26 && celsius <= 30) {
				ledArray[pixel] = ORANGE;
			} else {
				ledArray[pixel] = RED;
			}
		}

		//Transfer LED Data
		SPI_ss = 0;
		wait_us(500);
		RGB_LEDArray.write(0x26);	//Resets RGBMatrixBackpack index. (see sparkfun's github). It shouldn't be needed
									//-> but it doesn't work without it...
		for (int pixel = 0; pixel < PIXELS_COUNT; ++pixel) {
			RGB_LEDArray.write(ledArray[pixel]);
		}
		wait_us(500);
		SPI_ss = 1;

		wait(1);
	}
}
