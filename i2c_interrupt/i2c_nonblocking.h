#ifndef I2C_NONBLOCKING_H_
#define I2C_NONBLOCKING_H_

#include "cmsis_os.h"
#include "cmsis.h"
#include "pinmap.h"
#include "error.h"

#define SI 3

#define I2C_IRQN I2C0_IRQn
#define I2C_QUEUE I2C0_queue

extern osMessageQId  I2C_QUEUE;

//Things to note: 1) Interrupts must be enabled in NVIC too, 2) NVIC_SetPriority() is optional, default priority is 0
//->(highest), 3) if we want to choose the ISR during runtime dynamic vectors must be used (MBED has NVIC_SetVector()
//->implemented for this task)
void I2C1_IE() {
	NVIC_EnableIRQ(I2C_IRQN);
}

//If queue.put() is executed in ISR, and we have continuous interrupts (not allowing any task to run),
//->queue uses a size 16 FIFO. Shouldn't happen in a real case scenario.
//Things to note: 1) extern "C" is required, 2) Interrupt flag should be clear or we will enter the ISR continuously,
//-> 3) an ISR can't be interrupted by the same interrupt that caused it, only by a higher priority one.
//extern "C"
void I2C1_IRQHandler(){
	NVIC_DisableIRQ(I2C_IRQN);
	osMessagePut(I2C_QUEUE, (uint32_t)1, 0);
}


// Clear the Serial Interrupt (SI)
static inline void i2c_clear_SI(i2c_t *obj) {
	obj->i2c->CONCLR = 1 << SI;
	NVIC_ClearPendingIRQ(I2C_IRQN);
	NVIC_EnableIRQ(I2C_IRQN);
}

// Wait until the Serial Interrupt (SI) is set
static int i2c_wait_SI(i2c_t *obj) {
	osEvent evt = osMessageGet(I2C_QUEUE, osWaitForever);	//well, maybe forever is too long
    return 0;
}


#endif /* I2C_NONBLOCKING_H_ */
