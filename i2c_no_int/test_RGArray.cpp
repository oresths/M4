#include "mbed.h"

#define PIXELS_COUNT 64

#define GREEN 0x01
#define RED 0x02
#define ORANGE 0x03

char ledArray [64];
int celsius;

SPI RG_LEDArray(p5, p6, p7); /// mosi, miso, sclk
DigitalOut ss(p8);	///Slave Select

Serial pc(USBTX, USBRX);

int main() {
	ss = 1;	//Make sure the RG matrix is deactivated
	RG_LEDArray.format(8,0);
	RG_LEDArray.frequency(125000);

	while (1) {
		wait_ms(100);

		float temp_values[PIXELS_COUNT];

		pc.printf("RGArray\r\n");

		for (int i = 0; i < 20; ++i) {
			temp_values[i] = 25;
		}
		for (int i = 21; i < 40; ++i) {
			temp_values[i] = 28;
		}
		for (int i = 41; i < 64; ++i) {
			temp_values[i] = 40;
		}

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
		for (int pixel = 0; pixel < PIXELS_COUNT; ++pixel) {
			ss = 0;
			wait_us(500);
			RG_LEDArray.write(ledArray[pixel]);
			wait_us(500);
			ss = 1;
		}

		wait(0.1);
	}
}
