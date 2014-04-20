//============================================================================
// Name        : dccc.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

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

	int i, j;

	int nbytes = 4;
	int nbytesOUT = 1;

	unsigned char *bufIN;
	bufIN = (unsigned char *) malloc(nbytes * sizeof(char));

//	const char *bufOUT;
//	bufOUT = (const char *) malloc(20 * 20 * sizeof(char));

	unsigned char *temp_bufOUT;
	temp_bufOUT = (unsigned char *) malloc(20 * sizeof(char));

/*	stringstream ss;
	for (i = 0; i < 20 * 20; i++) {
		ss << i;
	}
	bufOUT = ss.str().c_str();*/

	int nr;
	//Warning: Emptying the buffer maybe could corrupt packet sequence ? ie if we flush in the middle of
	//incoming data. Needs more testing.
//	tcflush(fd, TCIFLUSH);	//empties incoming buffer

////	fcntl(fd, F_SETFL, FNDELAY);	//make read() non-blocking
//	while ( (nr=read(fd, bufIN, 64)) == 64 ) {}
////	fcntl(fd, F_SETFL, 0);	//make read() blocking
//	for (r = 0; r < 20; r++) {
	int bufOUT;
	cout << endl;
	for (;;) {
		bufOUT = 1;
		write(fd, (const void *)&bufOUT, nbytesOUT);
//		usleep(1*1000);
		nr=read(fd, bufIN, 8);	//blocking
		if (nr<0) cout << "Error" << endl;
		cout << "TPA1 = ";
		for (int i = 0; i < 8; ++i) {
			cout << (int)bufIN[i] << " ";
		}
		cout << endl;

//		usleep(100*1000);

		bufOUT = 2;
		write(fd, (const void *)&bufOUT, nbytesOUT);
//		usleep(1*1000);
		nr=read(fd, bufIN, 8);	//blocking
		if (nr<0) cout << "Error" << endl;
		cout << "TPA2 = ";
		for (int i = 0; i < 8; ++i) {
			cout << (int)bufIN[i] << " ";
		}
		cout << endl;

//		usleep(100*1000);

		bufOUT = 3;
		write(fd, (const void *)&bufOUT, nbytesOUT);
//		usleep(1*1000);
		nr=read(fd, bufIN, 8);	//blocking
		if (nr<0) cout << "Error" << endl;
		cout << "TPA3 = ";
		for (int i = 0; i < 8; ++i) {
			cout << (int)bufIN[i] << " ";
		}
		cout << endl;

//		usleep(100*1000);

		bufOUT = 4;
		write(fd, (const void *)&bufOUT, nbytesOUT);
//		usleep(1*1000);
		nr=read(fd, bufIN, 4);	//blocking
		if (nr<0) cout << "Error" << endl;
		cout << "CO2 = " << *(float *)bufIN  << endl;

		usleep(200*1000);
	}

	close(fd);
	cout << "Closed" << endl;
	return EXIT_SUCCESS;
}
