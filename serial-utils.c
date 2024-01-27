// 
// Serial Port Utils
//
// Copyright (c) 2024 Jonathan Tainer. Subject to the BSD 2-Clause License.
//

#include "serial-utils.h"

#define PLATFORM_POSIX
//#define PLATFORM_WINDOWS

#ifdef PLATFORM_POSIX

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

#define BAUDRATE B115200
#define _POSIX_SOURCE 1 // POSIX compliant source
#define RX_BUF_LEN 1024

static int modem_fd = 0;
static struct termios oldtio = { 0 };
static struct termios newtio = { 0 };
static char rx_buf[RX_BUF_LEN];

int serial_open(const char* modem_dev) {
	modem_fd = open(modem_dev, O_RDWR | O_NOCTTY);
	if (modem_fd < 0) {
		return 1;
	}

	oldtio = (struct termios) { 0 };
	newtio = (struct termios) { 0 };
	tcgetattr(modem_fd, &oldtio); // Save current serial port settings
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON; // Canonical line processing, i.e. buffers IO until newline then flushes

	// Clear modem line and activate new port settings
	tcflush(modem_fd, TCIFLUSH);
	tcsetattr(modem_fd, TCSANOW, &newtio);
}

void serial_close() {
	// Restore old port settings
	tcsetattr(modem_fd, TCSANOW, &oldtio);
	close(modem_fd);
}

const char* serial_getline() {
	// Check if port has data to read
	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(modem_fd, &read_fds);
	struct timeval tv = { 0 };
	int ready = select(modem_fd+1, &read_fds, NULL, NULL, &tv);
	
	// No data to read
	if (!FD_ISSET(modem_fd, &read_fds))
		return NULL;

	// Read in line from serial port
	int count = read(modem_fd, rx_buf, RX_BUF_LEN);

	// Error reading from port, probably should handle this case better
	if (count < 0)
		return NULL;

	// Zero bytes read from port
	if (count == 0)
		return NULL;

	// Successful read
	rx_buf[count] = 0; // Ensure string is null terminated
	return rx_buf;
}

#endif // PLATFORM_POSIX

#ifdef PLATFORM_WINDOWS

#endif // PLATFORM_WINDOWS
