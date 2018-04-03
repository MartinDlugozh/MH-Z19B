/*
 * mhz19b.h
 *
 *  Created on: Apr 2, 2018
 *      Author: Dr. Saldon
 * Last update: Apr 3, 2018
 *
 * 	Notes:
 * 		
 */

#ifndef MHZ19B_DRV_H_
#define MHZ19B_DRV_H_
#pragma once

/*-----------------------------------------------------------------------------
MACRO SECTION
-----------------------------------------------------------------------------*/
#define MHZ19B_USART_BAUDRATE 		9600	// USART parameters (9600b1n8)
#define MHZ19B_USART_PARITY			0
#define MHZ19B_USART_DATA_BITS 		8
#define MHZ19B_USART_STOP_BITS 		1

#define MHZ19B_PACKET_LENGTH 		9		// MH-Z19B data packet length [bytes]

#define MHZ19B_RANGE_LOW	 		2000	// MH-Z19B Detection range setting for 2000 [ppm] ([parts per million])
#define MHZ19B_RANGE_HIGH	 		5000	// MH-Z19B Detection range setting for 5000 [ppm] ([parts per million])

#define MHZ19B_MEASURING_TIME 		120000	// Maximum measurment time [s]

#define MHZ19B_STX 					0xff	// Magic byte (start byte)
#define MHZ19B_SENSOR 				0x01 	// Sensor number (address)
#define MHZ19B_CMD_READ 			0x86	// Read CO2 concentration
#define MHZ19B_CMD_ZERO 			0x87	// Calibrate Zero Point (ZERO)
#define MHZ19B_CMD_SPAN 			0x88	// Calibrate Span Point (SPAN)
#define MHZ19B_CMD_AUTOCAL			0x79	// ON/OFF Auto Calibration
#define MHZ19B_CMD_RANGE 			0x99	// Detection range setting

// Data buffer offsets
// TX
#define MHZ19B_CMD_BUF_STX			0x00	// Magic byte (start byte) offset
#define MHZ19B_CMD_BUF_SEN			0x01	// Sensor number (address) offset
#define MHZ19B_CMD_BUF_CMD			0x02	// Command code offset
#define MHZ19B_CMD_BUF_DAH 			0x03	// Command data MSB offset
#define MHZ19B_CMD_BUF_DAL 			0x04	// Command data LSB offset
#define MHZ19B_CMD_BUF_CRC			0x08	// Checksum offset

// RX
#define MHZ19B_RSP_BUF_STX			0x00	// Magic byte (start byte) offset
#define MHZ19B_RSP_BUF_CMD			0x01	// Command code offset
#define MHZ19B_RSP_BUF_CCH 			0x02 	// CO2 concentration value MSB offset
#define MHZ19B_RSP_BUF_CCL 			0x03	// CO2 concentration value LSB offset
#define MHZ19B_RSP_BUF_TT			0x04	// Temperature value offset
#define MHZ19B_RSP_BUF_SS			0x05	// S offset
#define MHZ19B_RSP_BUF_UUH			0x06	// U MSB offset
#define MHZ19B_RSP_BUF_UUL			0x07	// U LSB offset
#define MHZ19B_RSP_BUF_CRC			0x08	// Checksum offset

#define MHZ19B_MAX_TEMPERATURE 	 	60 		// [degC] (by datasheet working temperature is 0~50degC)

/*-----------------------------------------------------------------------------
INCLUDE SECTION
-----------------------------------------------------------------------------*/
#include "mhz19b_port_nix.h"

/*-----------------------------------------------------------------------------
GLOBAL SECTION
-----------------------------------------------------------------------------*/
typedef enum _mhz19bRxState{
	mhzRxWait = 0,						// Waiting for STX reception
	mhzRxStx,							// Got STX, receiving data
	mhzRxData,							// Packet received
} mhz19bRxState_t;

typedef enum _mhz19bResult{
	mhzResOk = 0,						// -OK- 	Common OK
	mhzResError,						// -ERROR- 	Common ERROR
	mhzResCrcError,						// -ERROR- 	Packet CRC check error
	mhzResCO2Overrange,					// -ERROR- 	Received CO2 concentration value is out of range
	mhzResTempOverrange,				// -ERROR- 	Received temperature value is out of range
	mhzResBufferOverflow,				// -ERROR- 	Internal Rx buffer overflow
	mhzResRxDone,						// -OK- 	Packet reception successful
} mhz19bResult_t;

typedef struct _mhz19bDataPacket{
	uint8_t buff[MHZ19B_PACKET_LENGTH];	// Data buffer
	uint8_t* buffHead;					// Buffer head pointer
	mhz19bRxState_t rxState;			// Reasponse Reception state
	uint8_t bytes_received;				// Received bytes counter
} mhz19bDataPacket_t;

typedef union _mhz19bRange{
	uint16_t u16;						// Detection range value represented as regular uint16_t number
	uint8_t u8[2];						// The same, but represented as HH (MSB = u8[0]) and LL (LSB = u8[1])
} mhz19bRange_t;

typedef union _mhz19bSpan{
	uint16_t u16;						// Span value represented as regular uint16_t number
	uint8_t u8[2];						// The same, but represented as HH (MSB = u8[0]) and LL (LSB = u8[1])
} mhz19bSpan_t;

typedef struct _mhz19bData{
	uint16_t co2;						// CO2 concentration [ppm] ([parts per million])
	uint8_t temperature;				// Temperature [degC]
	uint8_t _s;							// byte 5
	uint16_t _u;						// byte 6 and 7
	uint8_t crc;						// Packet CRC
	mhz19bRange_t range;				// Detection upper range (can be 2000 ppm or 5000 ppm)
	mhz19bDataPacket_t packet;			// Data Packet buffer
	mhz19bResult_t result;				// Action result
} mhz19bData_t;

/*-----------------------------------------------------------------------------
HEADER SECTION
-----------------------------------------------------------------------------*/
void 			portUsartInit			(void);
void 			portUsartSendByte		(int usart, uint8_t byte);
uint8_t 		mhzGetCheckSum			(uint8_t *packet);
mhz19bResult_t 	mhzCheckBufferOverflow	(mhz19bData_t* mhz);
void 			mhzCleanBuffer			(mhz19bData_t* mhz);
void 			mhzSetRange				(int usart, mhz19bData_t* mhz, uint16_t range);
void 			mhzSetAutocal			(int usart, mhz19bData_t* mhz, uint8_t autocal);
void 			mhzCalZeroPoint			(int usart, mhz19bData_t* mhz);
void 			mhzCalSpanPoint			(int usart, mhz19bData_t* mhz, uint16_t span);
void 			mhzSensorInit			(int usart, mhz19bData_t* mhz, uint16_t range);
void 			mhzRequestData			(int usart, mhz19bData_t* mhz);
mhz19bResult_t 	mhzRxByteCallback		(mhz19bData_t* mhz, uint8_t byte);
uint16_t 		mhzGetCo2				(mhz19bDataPacket_t* packet, uint16_t range, mhz19bResult_t* result);
uint8_t 		mhzGetTemperature		(mhz19bDataPacket_t* packet, mhz19bResult_t* result);
uint8_t 		mhzGetSS				(mhz19bDataPacket_t* packet, mhz19bResult_t* result);
uint16_t 		mhzGetUU				(mhz19bDataPacket_t* packet, mhz19bResult_t* result);
mhz19bResult_t 	mhzParsePacket			(mhz19bData_t* mhz);

/*-----------------------------------------------------------------------------
IMPLEMENTATION SECTION
-----------------------------------------------------------------------------*/

#endif /* MHZ19B_DRV_H_ */
