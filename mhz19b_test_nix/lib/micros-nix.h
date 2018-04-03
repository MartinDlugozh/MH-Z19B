/*
 * micros-nix.h
 *
 *  Created on: Jan 26, 2018
 *      Author: Dr. Saldon
 */

#ifndef _MICROS_NIX_H_
#define _MICROS_NIX_H_ 1
#pragma once

/*-----------------------------------------------------------------------------
MACRO SECTION
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
INCLUDE SECTION
-----------------------------------------------------------------------------*/

#if (defined __QNX__) | (defined __QNXNTO__)
/* QNX specific headers */
#include <unix.h>
#else
/* Linux / MacOS POSIX timer headers */
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#endif

/*-----------------------------------------------------------------------------
GLOBAL VARIABLES SECTION
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
HEADER SECTION
-----------------------------------------------------------------------------*/
uint64_t microsSinceEpoch(void);
uint64_t millisSinceEpoch(void);

/*-----------------------------------------------------------------------------
IMPLEMENTATION SECTION
-----------------------------------------------------------------------------*/

/* QNX timer version */
#if (defined __QNX__) | (defined __QNXNTO__)
uint64_t microsSinceEpoch(void)
{
	struct timespec time;
	
	uint64_t micros = 0;
	
	clock_gettime(CLOCK_REALTIME, &time);  
	micros = (uint64_t)time.tv_sec * 1000000 + time.tv_nsec/1000;
	
	return micros;
}
#else
uint64_t microsSinceEpoch(void)
{
	struct timeval tv;
	
	uint64_t micros = 0;
	
	gettimeofday(&tv, NULL);  
	micros =  ((uint64_t)tv.tv_sec) * 1000000 + tv.tv_usec;
	
	return micros;
}
#endif

uint64_t millisSinceEpoch(void)
{
	return (uint64_t)(microsSinceEpoch()/1000);
}

#endif
