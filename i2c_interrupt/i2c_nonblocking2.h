/*
 * i2c_nonblocking2.h
 *
 *  Created on: Mar 29, 2014
 *      Author: menas
 */

#ifndef I2C_NONBLOCKING2_H_
#define I2C_NONBLOCKING2_H_


//#include "mbed.h"
//#include "rtos.h"
#include "cmsis_os.h"
//#include "i2c_api.h"
#include "cmsis.h"
#include "pinmap.h"
#include "error.h"

//#include "i2c_api.h"

#define SI 3
//#define I2C1_SIGNAL 0x1

//static int i2c_wait_SI(i2c_t *obj);
//Thread *tI2C1pt;
//osThreadDef(i2c_wait_SI, osPriorityNormal, DEFAULT_STACK_SIZE);
//osThreadId tI2C1 = osThreadCreate(osThread(i2c_wait_SI), NULL);

//osMessageQDef(I2C1_queue, 1, uint32_t);
//osMessageQId  I2C1_queue;
extern osMessageQId  I2C1_queue;

//DigitalOut led2(LED2);
//DigitalOut led3(LED3);
//
//Serial pcc(USBTX, USBRX);

//Queue<uint8_t, 20> queue;

//Things to note: 1) Interrupts must be enabled in NVIC too, 2) NVIC_SetPriority() is optional, default priority is 0
//->(highest), 3) if we want to choose the ISR during runtime dynamic vectors must be used (MBED has NVIC_SetVector()
//->implemented for this task)
void I2C1_IE() {
	NVIC_EnableIRQ(I2C1_IRQn);
}

//If queue.put() is executed in ISR, and we have continuous interrupts (not allowing any task to run),
//->queue uses a size 16 FIFO. Shouldn't happen in a real case scenario.
//Things to note: 1) extern "C" is required, 2) Interrupt flag should be clear or we will enter the ISR continuously,
//-> 3) an ISR can't be interrupted by the same interrupt that caused it, only by a higher priority one.
//extern "C"
void I2C1_IRQHandler(){
//	queue.put((uint8_t *)LPC_I2C1->DAT);
	NVIC_DisableIRQ(I2C1_IRQn);
	osMessagePut(I2C1_queue, (uint32_t)1, 0);
//	osSignalSet(tI2C1, I2C1_SIGNAL);
//	tI2C1pt->signal_set(I2C1_SIGNAL);
//	led2=!led2;
}


// Clear the Serial Interrupt (SI)
static inline void i2c_clear_SI(i2c_t *obj) {
//    i2c_conclr(obj, 0, 0, 1, 0);
	obj->i2c->CONCLR = 1 << SI;
	NVIC_EnableIRQ(I2C1_IRQn);
}

// Wait until the Serial Interrupt (SI) is set
static int i2c_wait_SI(i2c_t *obj) {
//    int timeout = 0;
//    while (!(I2C_CONSET(obj) & (1 << 3))) {
//        timeout++;
//        if (timeout > 100000) return -1;
//    }
//	while (1) {
//		pcc.printf("wait\r\n");
	osEvent evt = osMessageGet(I2C1_queue, osWaitForever);	//well, maybe forever is too long
//	osSignalWait(I2C1_SIGNAL, osWaitForever);
//		led3=!led3;
//	}
    return 0;
}


/*void waitingTask(void const *args) {
	pcc.printf("wait\r\n");
	while (1) {
//		osEvent evt = queue.get();
//		pcc.printf("Task : %d\r\n",evt.value.v);
		led3=!led3;
	}
}*/



#endif /* I2C_NONBLOCKING2_H_ */
