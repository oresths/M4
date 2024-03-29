#include "grideye.hpp"

//Serial pcg(USBTX, USBRX);	//TODO delete this , and following printf, or make debug output

static uint8_t GridEYECenterValues[PIXELS_COUNT];
static uint8_t GridEYELeftValues[PIXELS_COUNT];
static uint8_t GridEYERightValues[PIXELS_COUNT];

static Mutex i2c0_mutex;
static Mutex i2c1_mutex;

void GridEYEInit(I2C *i2c0_obj, I2C *i2c1_obj) {
	i2c0_obj->frequency(100000);
	i2c1_obj->frequency(100000);
    I2C0_queue_create();
    I2C1_queue_create();
}

void GridEYETask(void const *args) {
	const i2c_sensor_t *temp=(const i2c_sensor_t *)args;
	I2C *i2c_obj = temp->i2c_obj;
	uint8_t i2c_periph_num = temp->i2c_periph_num;
	uint8_t i2c_addr = temp->i2c_addr;
	uint8_t grideye_num = temp->grideye_num;

	char cmd[2];

	char temper_values[PIXELS_COUNT];

#if ENABLE_RGB_LEDMATRIX

	DigitalOut SPI_ss(p8);	///Slave Select
	SPI_ss = 1;	//Make sure the RG matrix is deactivated, maybe this should be first line executed.
	SPI RGB_LEDMatrix(p5, p6, p7); /// mosi, miso, sclk

#endif

	while (1) {
		Thread::signal_wait(GRIDEYE_I2C_SIGNAL);

//		pcg.printf("GridEye\r\n");

		cmd[0] = GRIDEYE_I2C_TEMP_ADDR;
		i2c_lock(i2c_periph_num);
		int wr= i2c_obj->write(i2c_addr, cmd, 1, true);
		i2c_obj->read(i2c_addr, temper_values, PIXELS_COUNT, true);
		i2c_unlock(i2c_periph_num);

//		for (int i = 0; i < PIXELS_COUNT; ++i) {
//			pcg.printf("Temp = %d\r\n",(uint8_t)temper_values[i]);
//		}

		GridEYEvaluesSet((uint8_t *)temper_values, grideye_num);

#if ENABLE_RGB_LEDMATRIX
		
		if (grideye_num == GEYE_CENTER) {	//TODO Use the address of the GridEYE with the unique address
			char ledArray [64];
			int celsius;

			RGB_LEDMatrix.format(8,0);
			RGB_LEDMatrix.frequency(125000);

			//Determine LED Color for Pixel
			for (int pixel = 0; pixel < PIXELS_COUNT; ++pixel) {
				celsius = temper_values[pixel];
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
			RGB_LEDMatrix.write(0x26);	//Resets RGBMatrixBackpack index. (see sparkfun's github). It shouldn't be needed
										//-> but it doesn't work without it...
			for (int pixel = 0; pixel < PIXELS_COUNT; ++pixel) {
				RGB_LEDMatrix.write(ledArray[pixel]);
			}
			wait_us(500);
			SPI_ss = 1;
		}

#endif
	}
}

void GridEYEvaluesSet(uint8_t values[], uint8_t grideye_num) {
	uint8_t *GridEYEvalues;
	switch (grideye_num) {
		case GEYE_CENTER:
			GridEYEvalues = GridEYECenterValues;
			break;
		case GEYE_LEFT:
			GridEYEvalues = GridEYELeftValues;
			break;
		case GEYE_RIGHT:
			GridEYEvalues = GridEYERightValues;
			break;
		default:
			return;
	}

	for (int i = 0; i < PIXELS_COUNT; ++i) {
		if (values[i] < 0) {
			GridEYEvalues[i] = 0;
		} else if (values[i] > 80) {
			GridEYEvalues[i] = 80;
		} else {
			GridEYEvalues[i] = (uint8_t)(values[i] + 0.5);	//rounding to nearest Celsius degree
		}
	}
}

uint8_t * GridEYEvaluesGet(uint8_t grideye_num) {
	switch (grideye_num) {
		case GEYE_CENTER:
			return GridEYECenterValues;
			break;
		case GEYE_LEFT:
			return GridEYELeftValues;
			break;
		case GEYE_RIGHT:
			return GridEYERightValues;
			break;
	}
	return GridEYECenterValues;	//Shouldn't come here
}

void i2c_lock(uint8_t i2c_periph_num) {
	switch (i2c_periph_num) {
		case 0:
			i2c0_mutex.lock();
			break;
		case 1:
			i2c1_mutex.lock();
			break;
	}
}

void i2c_unlock(uint8_t i2c_periph_num) {
	switch (i2c_periph_num) {
		case 0:
			i2c0_mutex.unlock();
			break;
		case 1:
			i2c1_mutex.unlock();
			break;
	}
}
