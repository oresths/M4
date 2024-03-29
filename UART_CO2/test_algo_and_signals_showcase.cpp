/*
#include "mbed.h"
#include "rtos.h"
//#include "gpdma.h"

//#include "USBSerial2.h"
#include "Serial2.h"

* CO2 Sensor Protocol Instructions
#define RD   0x13        ///Read
#define DLE 0x10            ///Data Link Escape
#define Var_ID 0x06         ///Variable ID live data simple
#define NAK 0x19            ///Negative Acknowledge
#define DAT 0x1A            ///Single Data Frame
#define EOFF 0x1F          ///End of Frame
#define Checksum_hi 0x00    ///Checksum High byte
#define Checksum_lo 0x58    ///Checksum low byte

Serial pc(USBTX, USBRX);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

InterruptIn myButton(p23);

Serial2 co2uart(P4_22, P4_23);	//P4_22=Xbee3=TX, P4_23=Xbee2=RX

int32_t CO2RDA = 1;	///UART Receiver Data Available for CO2 sensor

Thread *tCO2pt;


void CO2Task(void const *args) ;

void RX_isr() ;

void CO2Init() ;

void CO2Trigger() ;

void THR_empty_isr() ;

int main() {
	myButton.mode(PullUp);
	myButton.rise(&RX_isr);

	Thread tCO2(CO2Task);
	tCO2pt = &tCO2;

	CO2Init();

	led1 = 1;
	led2 = 1;
	led3 = 0;

	while (1) {
		Thread::wait(50);	//temp - DELETE !!!!
		CO2Trigger();

		Thread::wait(5);	//temp - DELETE !!!!

		Thread::wait(1000);	//temp - DELETE !!!!
	}
}

void RX_isr() {
//	LPC_UART2->IER = 0;
	co2uart.attach(NULL, Serial::RxIrq);
//	uint32_t RBR  = LPC_UART2->RBR;
	led1=!led1;
	tCO2pt->signal_set(CO2RDA);
}

void CO2Task(void const *args) {
	uint8_t state = 0;
	uint8_t DATaPacket = 0;
	uint8_t NAKPacket = 0;
	uint8_t DataLength;	///Data field length minus version and status flag bytes
	uint8_t vi = 0, si = 0, gi = 0;
	uint8_t gj = 0;
	uint8_t StatusError =0;	///When Status flag !=0 indicates Status Error
	uint8_t uint8ieee[4];	///Contains the IEEE-754 as integers (CPU is little Endian)
	float GasReading;
	uint8_t ByteStuffing = 0;
	uint8_t ChecksumReceived[2];
	uint16_t ChecksumCalculated;
	uint8_t CO2SensorError = 0;
	uint8_t xara[33] = {0x10,0x1a,0xa,0x1,0x0,0x0,0x0,
				0x10,0x10,0xe3,0x60,0x10,0x10,
				0x10,0x1f,0x1,0xe7,
				0x10,0x1a,0x9,0x1,0x0,0x0,0x0,
				0x11,0x10,0x10,0x15,0x2a,
				0x10,0x1f,0x0,0xd4};
	int kk=-1;
	while (1) {
		Thread::signal_wait(CO2RDA);
		led3=!led3;
		kk++;
		int recv_char = co2uart.getc();	///Received character from CO2UART
		co2uart.attach(&RX_isr, Serial::RxIrq);
//		int recv_char = xara[kk];	///Received character from CO2UART
		led2=!led2;
		if (state>0 && state<=7) ChecksumCalculated += recv_char;
		switch (state) {
		case 0:
			if (recv_char == DLE) {
				DATaPacket = 0;
				NAKPacket = 0;
				StatusError =0;
				ChecksumCalculated = recv_char;	//We don't initialize Checksum in the last state
												//->in case we didn't go there because of noise.
				CO2SensorError = 0;
				state++;
			}
			break;
		case 1:	//If noise corrupts a byte in the FSM sequence, we loop between state
				//->0 and 1, until we have a valid sequence of DLE&DAT or DLE&NAK
			if (recv_char == DAT) {
				DATaPacket = 1;
				state++;
			} else if (recv_char == NAK) {
				NAKPacket = 1;
				state++;
			} else
				state = 0;
			break;
		case 2:
			if (NAKPacket)
				state = 0;
			else if (DATaPacket) {
				DataLength = recv_char - 4;
				if (DataLength <4) state = 0;	//We check for the upper limit in state 5.
				else state++;
			}
			else state = 0;
			break;
		case 3:
			if (vi < 2 - 1)
				vi++;
			else {
				vi = 0;
				state++;
			}
			break;
		case 4:
			if (si < 2 - 1) {
				if (recv_char != 0)
					StatusError = 1;
				si++;
			} else {
				if (recv_char != 0)
					StatusError = 1;
				si = 0;
				state++;
			}
			break;
		case 5:
			if (gi < DataLength - 1){
				if (recv_char != DLE || ByteStuffing) {
					uint8ieee[gj] = recv_char;
					ByteStuffing = 0;
					gj++;
					if (gj >= 4) state=0;	//Don't violate array limits if DataLength corrupt
				}
				else ByteStuffing = 1;
				gi++;
			} else {
				uint8ieee[gj] = recv_char;
				GasReading = *(float *)uint8ieee;
				ByteStuffing = 0;
				pc.printf("dl= %d \r\n", gi+1);
				gi = 0;
				gj = 0;
				state++;
			}
			break;
		case 6:	//DLE
			if (recv_char == DLE)
				state++;
			else state = 0;
			break;
		case 7:	//EOF
			if (recv_char == EOFF)
				state++;
			else state = 0;
			break;
		case 8:	//ChecksumReceived high
			ChecksumReceived[1] = recv_char;
			state++;
			break;
		case 9:	//ChecksumReceived low
			ChecksumReceived[0] = recv_char;
			if (ChecksumCalculated != *(uint16_t *)ChecksumReceived || StatusError)
				CO2SensorError = 1;
//			if (!CO2SensorError) sendtoPC;
			if (!CO2SensorError) pc.printf("metr= %f \r\n", GasReading);
			state = 0;
			break;
		}
	}
}

void CO2Init() {
	co2uart.baud(38400);	///Baud 38400, 8N1
	co2uart.attach(&THR_empty_isr, Serial::TxIrq);
	co2uart.attach(&RX_isr, Serial::RxIrq);
}

void CO2Trigger() {
	uint8_t co2TransmitBuffer[7] = { DLE, RD, Var_ID, DLE, EOFF, Checksum_hi,
	Checksum_lo };

	for (int i = 0; i < 7; ++i) {
		co2uart.putcNB(co2TransmitBuffer[i]);//Message must be maximum 16 bytes
											 //->(FIFO size)
	}
}

void THR_empty_isr() {
	led1 = !led1;
}
*/
