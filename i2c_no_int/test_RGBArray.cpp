/*
#include "mbed.h"

#define PIXELS_COUNT 64

#define GREEN 0x1C
#define RED 0xE0
#define ORANGE 0xFC
#define WHITE 0xFF
#define MAGENTA 0xE2
#define TEAL 0x1F

char ledArray [64];
int celsius;

SPI RGB_LEDArray(p5, p6, p7); /// mosi, miso, sclk
DigitalOut ss(p8);	///Slave Select

Serial pc(USBTX, USBRX);

int main() {
	ss = 1;	//Make sure the RG matrix is deactivated
	RGB_LEDArray.format(8,0);
	RGB_LEDArray.frequency(125000);

	while (1) {
		wait_ms(100);

		float temp_values[PIXELS_COUNT];

		pc.printf("RGBArray\r\n");

		for (int i = 0; i < 20; ++i) {
			temp_values[i] = 25;
		}
		for (int i = 20; i < 40; ++i) {
			temp_values[i] = 28;
		}
		for (int i = 40; i < 64; ++i) {
			temp_values[i] = 40;
		}

		//Determine LED Color for Pixel
		for (int pixel = 0; pixel < PIXELS_COUNT; ++pixel) {
			celsius = temp_values[pixel];
			if (celsius < 26) {
				ledArray[pixel] = WHITE;
			} else if (celsius >= 26 && celsius <= 30) {
				ledArray[pixel] = MAGENTA;
			} else {
				ledArray[pixel] = TEAL;
			}
		}

//		ledArray[0] = 0;
//		ledArray[1] = 0xE0;
//		ledArray[2] = 0x1C;
//		ledArray[3] = 0x03;
//		for (int i = 0; i < 22; ++i) {
//			ledArray[i] = 0;
//		}
//		for (int i = 22; i < 50; ++i) {
//			ledArray[i] = 0xfc;
//		}
//		for (int i = 50; i < 64; ++i) {
//			ledArray[i] = 03;
//		}
//
//		for (int i = 0; i < 64; ++i) {
//			ledArray[i] = 0xff;
//		}

		//Transfer LED Data
		ss = 0;
		wait_us(500);	//Suggested wait by RGBMatrixBackpack Guide
		RGB_LEDArray.write(0x26);	//Resets RGBMatrixBackpack index. (see sparkfun's github). It shouldn't be needed
									//-> but it doesn't work without it...
		for (int pixel = 0; pixel < PIXELS_COUNT; ++pixel) {
			RGB_LEDArray.write(ledArray[pixel]);
		}
		wait_us(500);	//Suggested wait by RGBMatrixBackpack Guide
		ss = 1;

//		//Transfer LED Data
//		ss = 0;
//		wait_us(500);	//Suggested wait by RGBMatrixBackpack Guide
//		RGB_LEDArray.write(0x26);
//		for (int pixel = 0; pixel < PIXELS_COUNT-40; ++pixel) {
//			RGB_LEDArray.write(0x1c);
//		}
//		wait_us(500);	//Suggested wait by RGBMatrixBackpack Guide
//		ss = 1;
//
//		wait(2);
//
//		ss = 0;
//		wait_us(500);	//Suggested wait by RGBMatrixBackpack Guide
//		RGB_LEDArray.write(0x26);
//		for (int pixel = 0; pixel < PIXELS_COUNT-40; ++pixel) {
//			RGB_LEDArray.write(0xff);
//		}
//		wait_us(500);	//Suggested wait by RGBMatrixBackpack Guide
//		ss = 1;
//
//		wait(2);
//
//		ss = 0;
//		wait_us(500);	//Suggested wait by RGBMatrixBackpack Guide
//		RGB_LEDArray.write(0x26);
//		for (int pixel = 0; pixel < PIXELS_COUNT-40; ++pixel) {
//			RGB_LEDArray.write(0x03);
//		}
//		wait_us(500);	//Suggested wait by RGBMatrixBackpack Guide
//		ss = 1;
//
//		wait(2);

		wait(2);
	}
}
*/
