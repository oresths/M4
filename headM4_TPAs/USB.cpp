#include "USB.hpp"

//I copied USBSerial.h to USBSerial2.h. Only changed the constructor so we can
//input the circular buffer size (buf). It would be a good idea for the buffer
//size to be a multiple of the incoming packet size. Haven't tested for packets
//bigger than 64 byte.
//The circular buffer fills up transparently with a callback, so we don't have
//to worry about missing packets.
static USBSerial *usb;

Serial pcu(USBTX, USBRX);

void USBInit() {
	usb = new USBSerial(20);	//TODO DEBUG
}

//USBTask could be made as interrupt callback
void USBTask(const void *args) {
	union {
		float CO2value;
		uint8_t CO2value_uint8;
	};
	uint8_t command;

	uint8_t *temp;
	uint8_t temp3;
	uint8_t temp2[8];

	while (true) {
		Thread::wait(COMMAND_POLLING_TIME);
		if (usb->readable()) {
			command = usb->_getc();

			switch (command) {
				case GEYE_CENTER_REQUEST:
					temp=GridEYEvaluesGet(GEYE_CENTER);
					usb->writeBlock(temp, PIXELS_COUNT);
					break;
				case GEYE_LEFT_REQUEST:
					usb->writeBlock(GridEYEvaluesGet(GEYE_LEFT), PIXELS_COUNT);
					break;
				case GEYE_RIGHT_REQUEST:
					temp=GridEYEvaluesGet(GEYE_RIGHT);
					for (int i = 0; i < 8; ++i) {
						temp2[i]=i;
					}
					temp3=0x31;
					usb->writeBlock(temp, 8);
					break;
				case CO2_REQUEST:
					CO2value = CO2valueGet();
					usb->writeBlock(&CO2value_uint8, 4);
					break;
				default:
					break;
			}

		}

	}
}
