/*
 * mhz19b_port_nix.c
 *
 *  Created on: Apr 3, 2018
 *      Author: Dr. Saldon
 * Last update: Apr 3, 2018
 *
 * 	Notes:
 * 		
 */


/*-----------------------------------------------------------------------------
MACRO SECTION
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
INCLUDE SECTION
-----------------------------------------------------------------------------*/
#include "mhz19b.h"

/*-----------------------------------------------------------------------------
GLOBAL SECTION
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
HEADER SECTION
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
IMPLEMENTATION SECTION
-----------------------------------------------------------------------------*/

/**
 * Initialize USART
 *
 * Port specific USART initialization code.
 */
void portUsartInit(void){

}

/**
 * Send byte over USART
 *
 * Port specific code.
 */
void portUsartSendByte(portUsartDesc_t usart, uint8_t byte){
	uint8_t _byte = byte;
	write(usart, &_byte, 1);
	tcdrain(usart);
}
