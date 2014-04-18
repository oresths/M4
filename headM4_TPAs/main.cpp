#include "mbed.h"
#include "rtos.h"
#include "grideye.hpp"
#include "CO2.hpp"
#include "USB.hpp"
//#include "gpdma.h"
//#include "dsp.h"
//#include "USBSerial2.h"

USBSerial usb(21);

int main (void) {
	Thread tCO2(CO2Task);

	CO2Init(p17, p18);	//p17=TX, p18=RX

//    I2C i2c0(p32,p31);	//sda, scl
	I2C i2c0(p19,p20);	//sda, scl
    I2C i2c1(p9,p10);

    GridEYEInit(&i2c0, &i2c1);

	i2c_sensor_t temp_sens1;
	i2c_sensor_t temp_sens2;
	i2c_sensor_t temp_sens3;

	temp_sens1.i2c_obj = &i2c0;
	temp_sens1.i2c_addr = 0xD0;
    Thread tGridEYECenterI2C0(GridEYETask, (void *)&temp_sens1);

    temp_sens2.i2c_obj = &i2c0;
    temp_sens2.i2c_addr = 0xD4;
    Thread tGridEYELeftI2C0(GridEYETask, (void *)&temp_sens2);

    temp_sens3.i2c_obj = &i2c1;
    temp_sens3.i2c_addr = 0xD8;
    Thread tGridEYERightI2C1(GridEYETask, (void *)&temp_sens3);

//    Thread tUSB(USBTask);

    while (true) {
		if (usb.readable()) {
			int command = usb._getc();
			int uu=command-1;
		}
    	tGridEYECenterI2C0.signal_set(GRIDEYE_I2C_SIGNAL);

		Thread::wait(12);
		CO2Trigger();

		Thread::wait(13);
		tGridEYERightI2C1.signal_set(GRIDEYE_I2C_SIGNAL);

		Thread::wait(25);
		tGridEYELeftI2C0.signal_set(GRIDEYE_I2C_SIGNAL);

		Thread::wait(40);
		//usb

		Thread::wait(10);

		Thread::wait(2000);
    }
}
