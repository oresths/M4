#include "mbed.h"
#include "rtos.h"

#define _P2_10 10

DigitalOut led2(LED2);
DigitalOut led3(LED3);

Serial pc(USBTX, USBRX);

Queue<int, 20> queue;

//Things to note: 1) Interrupts must be enabled in NVIC too, 2) NVIC_SetPriority() is optional, default priority is 0
//->(highest), 3) if we want to choose the ISR during runtime, dynamic vectors must be used (MBED has NVIC_SetVector()
//->implemented for this task), 4) interrupt priority registers for interrupts 32-40 don't appear to LPCExpresso's
//-> Peripheral+ view, but they exist if we check the memory manually
void IO_IE() {
	LPC_GPIOINT->IO2IntEnR |= 1 << _P2_10;
	NVIC_EnableIRQ(GPIO_IRQn);
}

void waitingTask(void const *args) {
	pc.printf("wait\r\n");
	while (1) {
		osEvent evt = queue.get();
		pc.printf("Task : %d\r\n",evt.value.v);
		led3=!led3;
	}
}

//If queue.put() is executed inside an ISR, and we have continuous interrupts (not allowing non-ISR code to run),
//->queue uses a size 16 FIFO (independent from what the user sets). Shouldn't happen in a real case scenario.
//Things to note: 1) extern "C" is required if handler executed in C++ code, 2) Interrupt flag should be cleared
//-> or we will enter the ISR continuously, 3) an ISR can't be interrupted by the same interrupt that caused it,
//->only by a higher priority one.
extern "C" void GPIO_IRQHandler(){
	int num = rand();
	queue.put((int *)num);
	LPC_GPIOINT->IO2IntClr |= 1 << _P2_10;
	led2=!led2;
}

//Returns time. RTOS SysTick timer, 1ms accuracy.
//To be able to call the function in main() we must not include rt_Time.h and write the prototype and use extern "C"
extern "C" U32  rt_time_get (void);

int main (void) {
    Thread thread(waitingTask);
	led2 = 1;
	led3 = 0;
	IO_IE();
	wait(0.1);
	uint32_t time = rt_time_get();
	pc.printf("kseki %d\r\n", time);

    while (true) {

    }
}
