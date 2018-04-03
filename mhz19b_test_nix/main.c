/*
 * main.c
 *
 *  Created on: Apr 2, 2018
 *      Author: Dr. Saldon
 * 	Notes:
 * 		
 */

/*-----------------------------------------------------------------------------
MACRO SECTION
-----------------------------------------------------------------------------*/
// Compiller rules
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

/*-----------------------------------------------------------------------------
INCLUDE SECTION
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "lib/file_io.h"
#include "lib/msg.h"
#include "lib/micros-nix.h"
#include "lib/usrin.h"
#include "mhz19b_drv/mhz19b.h"

/*-----------------------------------------------------------------------------
GLOBAL SECTION
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
HEADER SECTION
-----------------------------------------------------------------------------*/
int kbhit(void);
int set_interface_attribs(int fd, int speed);
void help(int argc, char* argv[]);

/*-----------------------------------------------------------------------------
IMPLEMENTATION SECTION
-----------------------------------------------------------------------------*/
int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch == 27)
	{
		return 1;
	}
	return 0;
}

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        fprintf(stderr, "%s*ERROR*%s from tcgetattr(): %s\n", RED, NC, strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "%s*ERROR*%s from tcsetattr(): %s\n", RED, NC, strerror(errno));
        return -1;
    }
    return 0;
}

void set_blocking(int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf(stderr, "%s*ERROR*%s from tggetattr(): %s\n", RED, NC, strerror(errno));
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0){
        	fprintf(stderr, "%s*ERROR*%s setting term attributes: %s\n", RED, NC, strerror(errno));
        }
}

void help(int argc, char* argv[])
{
	fprintf(stdout, "%s=====================================================%s\n", GREEN, NC);
	fprintf(stdout, "%s=                                                   =%s\n", GREEN, NC);
	fprintf(stdout, "%s=                      MH-Z19B                      =%s\n", GREEN, NC);
	fprintf(stdout, "%s=               CO2 concentration sensor            =%s\n", GREEN, NC);
	fprintf(stdout, "%s=                                                   =%s\n", GREEN, NC);
	fprintf(stdout, "%s=                   Test application                =%s\n", GREEN, NC);
	fprintf(stdout, "%s=                                                   =%s\n", GREEN, NC);
	fprintf(stdout, "%s=====================================================%s\n\n", GREEN, NC);

	if ((argc == 1) || (find_argv(argc, argv, "--help") != 0))
    {
		printf("\n");
		printf("\tUsage:\n\n");
		printf("\t");
		printf("%s", argv[0]);
		printf(" <address of serial port>\n");
		printf("\n\tEnjoy!\n\n");
		exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[]){
	help(argc, argv);
	uint64_t timeStartup = microsSinceEpoch();
	uint64_t time = microsSinceEpoch();
	uint64_t timeNoResponse = microsSinceEpoch();

	/*** CREATE LOG-FILE ***/
	char* buff = malloc(sizeof(char)*64);		// allocate memory for the log-file name
	if(buff != NULL){
		time = microsSinceEpoch();
		if(sprintf(buff, "./logs/%lu_mh-z19b.log", time) == -1){
			msgte(MSG_ERROR, "Error buffering log filename", errno);
		}
	}else{
		msgte(MSG_DIE, "Cannot allocate memory", errno);
	}

	FILE* logfile;						// create log-file descriptor
	logfile = fopenw(buff, "a");		// open log file
	free(buff);

	time = microsSinceEpoch();
	if(fprintf(logfile, "--- %lu --- Log start\n", time) == -1) // 1-st string
	{
		msgte(MSG_ERROR, "File writing error (log)", errno);
	}else{
		fprintf(logfile, "UTC, CO2, Temperature, S, U\n");
	}
	/*** CREATE LOG-FILE ***/

	/*** OPENNING UART ***/
	msgl(logfile, "Openning USART..");
    int uart = -1;		//uart address
   	uart = openwu(argv[1], O_RDWR | O_NOCTTY | O_SYNC);

   	msgl(logfile, "Configuring USART..");
    if(set_interface_attribs(uart, B9600) == -1)
    {
    	msglte(logfile, MSG_DIE, "UART configuration error", errno);
    }
    set_blocking(uart, 0);
	/*** END OPENNING UART ***/

	mhz19bData_t mhzSensor;
	mhzSensorInit(uart, &mhzSensor, MHZ19B_RANGE_HIGH);

	sleep(10);
	mhzRequestData(uart, &mhzSensor);

	for(;;){
		uint8_t rx_byte = 0;
		int n = read(uart, &rx_byte, 1);
		if(n == 1){
			mhzSensor.result = mhzRxByteCallback(&mhzSensor, rx_byte);
			if((mhzSensor.result == mhzResRxDone) || (mhzSensor.packet.rxState == mhzRxData)){
				mhzSensor.result = mhzParsePacket(&mhzSensor);
				time = microsSinceEpoch();
				fprintf(stdout, "%s*OK*%s Uptime = %lu: CO2 = %d, T = %d \n",
						GREEN, NC, ((time - timeStartup)/1000), mhzSensor.co2, mhzSensor.temperature);
				fprintf(logfile, "%lu, %d, %d, %d, %d \n", time, mhzSensor.co2, mhzSensor.temperature, mhzSensor._s, mhzSensor._u);
				sleep(2);
				REQ:
				mhzRequestData(uart, &mhzSensor);
			}
		}

		if((microsSinceEpoch() - timeNoResponse) >= 10000000){
			timeNoResponse = microsSinceEpoch();
			goto REQ;
		}

		if(kbhit()){
			goto STOP;
		}
	}

	STOP:
	msglt(logfile, MSG_OK, "  -== STOP ==-  ");
	fsyncw(logfile);
	fclosew(logfile);
	parse_self_destruct(argc, argv);
	exit(EXIT_SUCCESS);
	return 0;
}
