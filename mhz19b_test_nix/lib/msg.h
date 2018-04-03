/*
 * msg.h
 *
 *  Created on: Apr 3, 2018
 *      Author: Dr. Saldon
 *
 * 	Notes:
 *
 */

#ifndef _MSG_H_
#define _MSG_H_

/*-----------------------------------------------------------------------------
MACRO SECTION
-----------------------------------------------------------------------------*/
#define MSG_OK 		0
#define MSG_ERROR 	1
#define MSG_DIE		2
#define MSG_DEBUG	3

#define NC 		"\033[0m"
#define GREEN 	"\033[0;32m"
#define RED 	"\033[1;31m"

/*-----------------------------------------------------------------------------
INCLUDE SECTION
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-----------------------------------------------------------------------------
GLOBAL SECTION
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
HEADER SECTION
-----------------------------------------------------------------------------*/
void msglte(FILE* logfd, int type, const char* msg, int errnum);
void msglt(FILE* logfd, int type, const char* msg);
void msgl(FILE* logfd, const char* msg);
void msgte(int type, const char* msg, int errnum);
void msgt(int type, const char* msg);
void msg(const char* msg);

/*-----------------------------------------------------------------------------
IMPLEMENTATION SECTION
-----------------------------------------------------------------------------*/

void msglte(FILE* logfd, int type, const char* msg, int errnum)
{
	if((type == MSG_ERROR) || (type == MSG_DIE))
	{
		fprintf(stderr, "%s*ERROR*%s %s: %s\n", RED, NC, msg, strerror(errnum));
		fprintf(logfd, "*ERROR* %s: %s\n", msg, strerror(errnum));

		if(type == MSG_DIE)
		{
			if(fsync(fileno(logfd)) == -1)
				{
					fprintf(stderr, "%s*ERROR*%s File sync error: %s\n", RED, NC, strerror(errno));
				}
			exit(EXIT_FAILURE);
		}
	}

	if(type == MSG_OK)
	{
		fprintf(stdout, "%s*OK*%s %s\n", GREEN, NC, msg);
		fprintf(logfd, "*OK* %s\n", msg);
	}
}

void msglt(FILE* logfd, int type, const char* msg)
{
	if((type == MSG_ERROR) || (type == MSG_DIE))
	{
		fprintf(stdout, "%s*ERROR*%s %s\n", RED, NC, msg);
		fprintf(logfd, "*ERROR* %s\n", msg);
		if(type == MSG_DIE)
		{
			exit(EXIT_FAILURE);
		}
	}

	if(type == MSG_OK)
	{
		fprintf(stdout, "%s*OK*%s %s\n", GREEN, NC, msg);
		fprintf(logfd, "*OK* %s\n", msg);
	}
}

void msgl(FILE* logfd, const char* msg)
{
	fprintf(stdout, "%s\n", msg);
	fprintf(logfd, "%s\n", msg);
}

void msgte(int type, const char* msg, int errnum)
{
	if((type == MSG_ERROR) || (type == MSG_DIE))
	{
		fprintf(stderr, "%s*ERROR*%s %s: %s\n", RED, NC, msg, strerror(errnum));

		if(type == MSG_DIE)
		{
			exit(EXIT_FAILURE);
		}
	}

	if(type == MSG_OK)
	{
		fprintf(stdout, "%s*OK*%s %s\n", GREEN, NC, msg);
	}
}

void msgt(int type, const char* msg)
{
	if((type == MSG_ERROR) || (type == MSG_DIE))
	{
		fprintf(stderr, "%s*ERROR*%s %s\n", RED, NC, msg);

		if(type == MSG_DIE)
		{
			exit(EXIT_FAILURE);
		}
	}

	if(type == MSG_OK)
	{
		fprintf(stdout, "%s*OK*%s %s\n", GREEN, NC, msg);
	}
}

void msg(const char* msg)
{
	fprintf(stdout, "%s\n", msg);
}

#endif /* _MSG_H_ */
