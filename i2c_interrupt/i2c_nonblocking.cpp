#include "mbed.h"
#include "rtos.h"

#define SI 3

DigitalOut led2(LED2);
DigitalOut led3(LED3);

Serial pcc(USBTX, USBRX);

Queue<uint8_t, 20> queue;

//Things to note: 1) Interrupts must be enabled in NVIC too, 2) NVIC_SetPriority() is optional, default priority is 0
//->(highest), 3) if we want to choose the ISR during runtime dynamic vectors must be used (MBED has NVIC_SetVector()
//->implemented for this task)
void I2C1_IE() {
	NVIC_EnableIRQ(I2C1_IRQn);
}

void waitingTask(void const *args) {
	pcc.printf("wait\r\n");
	while (1) {
		osEvent evt = queue.get();
		pcc.printf("Task : %d\r\n",evt.value.v);
		led3=!led3;
	}
}

//If queue.put() is executed in ISR, and we have continuous interrupts (not allowing any task to run),
//->queue uses a size 16 FIFO. Shouldn't happen in a real case scenario.
//Things to note: 1) extern "C" is required, 2) Interrupt flag should be clear or we will enter the ISR continuously,
//-> 3) an ISR can't be interrupted by the same interrupt that caused it, only by a higher priority one.
extern "C" void I2C1_IRQHandler(){
	queue.put((uint8_t *)LPC_I2C1->DAT);
	LPC_I2C1->CONSET |= 1 << SI;
	led2=!led2;
}

//// Wait until the Serial Interrupt (SI) is set
//static int i2c_wait_SI(i2c_t *obj) {
//    int timeout = 0;
//    while (!(I2C_CONSET(obj) & (1 << 3))) {
//        timeout++;
//        if (timeout > 100000) return -1;
//    }
//    return 0;
//}
//
