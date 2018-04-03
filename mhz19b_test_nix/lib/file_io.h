/*
 * file_io.h
 *
 *  Created on: Apr 3, 2018
 *      Author: Dr. Saldon
 *
 * 	Notes:
 * 		
 */

#ifndef FILE_IO_H_
#define FILE_IO_H_

/*-----------------------------------------------------------------------------
MACRO SECTION
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
INCLUDE SECTION
-----------------------------------------------------------------------------*/
#include <errno.h>
#include <fcntl.h>
#include "msg.h"

/*-----------------------------------------------------------------------------
GLOBAL SECTION
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
HEADER SECTION
-----------------------------------------------------------------------------*/
int openwu(const char *__file, int __oflag);
FILE *fopenw(const char *__restrict __filename, const char *__restrict __modes);
int fclosew(FILE* fd);
void fsyncw(FILE* fd);
ssize_t writew(int _fd, const void *_buf, size_t _n);

/*-----------------------------------------------------------------------------
IMPLEMENTATION SECTION
-----------------------------------------------------------------------------*/

int openwu(const char *__file, int __oflag)
{
	int uart;

	uart = open(__file, __oflag);
    if (uart == -1) {
        msgte(MSG_DIE, "Error opening", errno);
    }else{
		printf("Connected to ");
		printf("%s\n", __file);
    }
    return uart;
}

FILE *fopenw(const char *__restrict __filename,
		    const char *__restrict __modes) {
	FILE* fd;

	fd = fopen(__filename, __modes);
	if(fd == NULL)
	{
		msgte(MSG_DIE, "File I/O error", errno);
	}
	return fd;
}

int fclosew(FILE* fd)
{
	if(fd)
	{
		if(fsync(fileno(fd)) == -1)
		{
			msgte(MSG_DIE, "File sync error", errno);
		}

		if(fclose(fd) == -1)
		{
			msgte(MSG_DIE, "File closing error", errno);
		}
	}
	return 0;
}

ssize_t writew(int _fd, const void *_buf, size_t _n)
{
	if(_buf == NULL || _n ==0)
	{
		msgte(MSG_ERROR, "Error: bad write() input", 0);
		return 0;
	}

	ssize_t wlen = write(_fd, _buf, _n);
	if(wlen < 0)
	{
		msgte(MSG_ERROR, "Error from write()", errno);
		return -1;
	}
	return wlen;
}

void fsyncw(FILE* fd)
{
	if(fsync(fileno(fd)) == -1)
	{
		msgte(MSG_ERROR, "File sync error", errno);
	}
}

#endif /* FILE_IO_H_ */
