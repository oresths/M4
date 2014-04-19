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
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	int fd;

//	fd = open("/dev/head", O_RDWR | O_NOCTTY | O_NDELAY);
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

	const char *bufOUT;
	bufOUT = (const char *) malloc(20 * 20 * sizeof(char));

	unsigned char *temp_bufOUT;
	temp_bufOUT = (unsigned char *) malloc(20 * sizeof(char));

/*	int dokOUT[20 * 20];
	for (i = 0; i < 20; i++) {
		for (j = 0; j < 20; j++) {
			dokOUT[i * 20 + j] = j + i;
		}
	}

	stringstream ss;
	for (i = 0; i < 20 * 20; i++) {
		ss << dokOUT[i];
	}
	bufOUT = ss.str().c_str();*/

	int r;
	int nr;
	//Warning: Emptying the buffer maybe could corrupt packet sequence ? ie if we flush in the middle of
	//incoming data. Needs more testing.
//	tcflush(fd, TCIFLUSH);	//empties incoming buffer

////	fcntl(fd, F_SETFL, FNDELAY);	//make read() non-blocking
//	while ( (nr=read(fd, bufIN, 64)) == 64 ) {}
////	fcntl(fd, F_SETFL, 0);	//make read() blocking
//	for (r = 0; r < 20; r++) {
	for (;;) {
		bufOUT = 4;
		write(fd, temp_bufOUT, nbytesOUT);
		usleep(500*1000);

//		cout << "prin" << endl;
		nr=read(fd, bufIN, 4);	//blocking
		if (nr<0) cout << "Error" << endl;

//		printf("CO2= %f", t2);
		cout << *(float *)bufIN  << endl;

		usleep(500*1000);

/*		for (i = 0; i < 20; i++)
			temp_bufOUT[i] = bufOUT[r * 20 + i];
//		cout << temp_bufOUT;
		write(fd, temp_bufOUT, nbytesOUT);*/

	}

	close(fd);
	cout << "Closed" << endl;
	return EXIT_SUCCESS;
}
