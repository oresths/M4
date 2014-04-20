#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <iostream>
#include <termios.h>

using namespace std;

int main(void) {
	int fd;

//	fd = open("/dev/head", O_RDWR | O_NOCTTY | O_NDELAY);	//to make read non-blocking
	fd = open("/dev/head", O_RDWR | O_NOCTTY );
	if (fd == -1) {
		puts("[Head]: cannot open port");
		printf("\n open() failed with error [%s]\n", strerror(errno));
		return -1;
	} else {
		puts("[Head]: usb port successfully opened\n");
	}
	usleep(30000);	//needs some time to initialize, even though it opens succesfully. tcflush() didn't work
					//without waiting at least 8 ms

	int CO2nbytes = 4;
	int TPA81nbytes = 8;
	int nbytesOUT = 1;

	union {
		unsigned char CO2bufIN[4];
		float CO2bufIN_float;
	};

	unsigned char TPAbufIN[8];

	int bufOUT;

	int nr;
//Warning: Emptying the buffer maybe could corrupt packet sequence ? ie if we flush in the middle of
//incoming data. Needs more testing.
//	tcflush(fd, TCIFLUSH);	//empties incoming buffer

//	fcntl(fd, F_SETFL, FNDELAY);	//make read() non-blocking
//	fcntl(fd, F_SETFL, 0);	//make read() blocking


	for (;;) {
		tcflush(fd, TCIFLUSH);

		bufOUT = 1;
		nr = write(fd, (const void *)&bufOUT, nbytesOUT);
		if (nr!=1) cout << "Write Error" << endl;
		nr=read(fd, TPAbufIN, TPA81nbytes);	//blocking
		if (nr<0) cout << "Read Error" << endl;
		cout << "TPA1 = ";
		for (int i = 0; i < TPA81nbytes; ++i) {
			cout << (int)TPAbufIN[i] << " ";
		}
		cout << endl;


		bufOUT = 2;
		nr = write(fd, (const void *)&bufOUT, nbytesOUT);
		if (nr!=1) cout << "Write Error" << endl;
		nr=read(fd, TPAbufIN, TPA81nbytes);	//blocking
		if (nr<0) cout << "Read Error" << endl;
		cout << "TPA2 = ";
		for (int i = 0; i < TPA81nbytes; ++i) {
			cout << (int)TPAbufIN[i] << " ";
		}
		cout << endl;


		bufOUT = 3;
		nr = write(fd, (const void *)&bufOUT, nbytesOUT);
		if (nr!=1) cout << "Write Error" << endl;
		nr=read(fd, TPAbufIN, TPA81nbytes);	//blocking
		if (nr<0) cout << "Read Error" << endl;
		cout << "TPA3 = ";
		for (int i = 0; i < TPA81nbytes; ++i) {
			cout << (int)TPAbufIN[i] << " ";
		}
		cout << endl;


		bufOUT = 4;
		nr = write(fd, (const void *)&bufOUT, nbytesOUT);
		if (nr!=1) cout << "Write Error" << endl;
		nr=read(fd, CO2bufIN, CO2nbytes);	//blocking
		if (nr<0) cout << "Read Error" << endl;
		cout << "CO2 = " << CO2bufIN_float << endl;

		usleep(200*1000);
	}

	close(fd);
	cout << "Closed" << endl;
	return EXIT_SUCCESS;
}
