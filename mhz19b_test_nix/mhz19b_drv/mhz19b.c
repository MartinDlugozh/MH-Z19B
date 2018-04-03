/*
 * mhz19b.c
 *
 *  Created on: Apr 2, 2018
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
 * MH-Z19B - Calculate check sum
 *
 * Read MH-Z19B datasheet for more information.
 *
 * Input:
 * 	uint8_t *packet - pointer to the first byte of MH-Z19B packet;
 *
 * Return:
 * 	uint8_t checksum - MH-Z19B packet checksum.
 */
uint8_t mhzGetCheckSum(uint8_t *packet)
{
	uint8_t checksum = 0;
	for(uint8_t i = 1; i < 8; i++)
	{
		checksum += packet[i];
	}
	checksum = 0xff - checksum;
	checksum += 1;
	return checksum;
}

/**
 * MH-Z19B - Check buffer overflow
 *
 * Check internal data buffer for overflow.
 *
 * Input:
 * 	mhz19bData_t* mhz - MH-Z19B working structure;
 *
 * Return:
 * 	mhz19bResult_t result - can be mhzResOk if no overflow detected,
 * 							and mhzResBufferOverflow in case of buffer overflow.
 */
mhz19bResult_t mhzCheckBufferOverflow(mhz19bData_t* mhz){
	mhz19bResult_t result = mhzResBufferOverflow;

	if(mhz->packet.buffHead < (mhz->packet.buff + MHZ19B_PACKET_LENGTH)){
		result = mhzResOk;
	}

	return result;
}

/**
 * MH-Z19B - Clean buffer
 *
 * Clean internal data buffer.
 *
 * Note: internal data buffer is used for storage raw MH-Z19B packets
 * both when receiving and transmitting data or commands.
 *
 * Input:
 * 	mhz19bData_t* mhz - MH-Z19B working structure.
 */
void mhzCleanBuffer(mhz19bData_t* mhz){
	memset(mhz->packet.buff, 0, MHZ19B_PACKET_LENGTH);
	mhz->packet.buffHead = mhz->packet.buff;
}

/**
 * MH-Z19B - Set Range
 *
 * Set MH-Z19B detection range and send MHZ19B_CMD_RANGE command to the sensor.
 * No response.
 *
 * Input:
 * 	portUsartDesc_t usart - usart port descriptor or address;
 * 	mhz19bData_t* mhz - MH-Z19B working structure;
 * 	uint16_t range - detection range: can be MHZ19B_RANGE_LOW (max 2000ppm)
 * 						or MHZ19B_RANGE_HIGH (max 5000ppm).
 */
void mhzSetRange(portUsartDesc_t usart, mhz19bData_t* mhz, uint16_t range){
	mhz->range.u16 = range;

	memset(mhz->packet.buff, 0, MHZ19B_PACKET_LENGTH);
	mhz->packet.buff[MHZ19B_CMD_BUF_STX] = MHZ19B_STX;			// STX
	mhz->packet.buff[MHZ19B_CMD_BUF_SEN] = MHZ19B_SENSOR;		// SEN
	mhz->packet.buff[MHZ19B_CMD_BUF_CMD] = MHZ19B_CMD_RANGE;	// CMD
	mhz->packet.buff[MHZ19B_CMD_BUF_DAH] = mhz->range.u8[0];	// RHH
	mhz->packet.buff[MHZ19B_CMD_BUF_DAL] = mhz->range.u8[1];	// RLL
	mhz->packet.buff[MHZ19B_CMD_BUF_CRC] = mhzGetCheckSum(mhz->packet.buff);			// CRC

	for(uint8_t i = 0; i < MHZ19B_PACKET_LENGTH; i++){
		portUsartSendByte(usart, mhz->packet.buff[i]);
	}
}

/**
 * MH-Z19B - Set ABC logic on/off
 *
 * Enable or disable MH-Z19B Auto Base Calibration.
 * No response.
 *
 * Input:
 * 	portUsartDesc_t usart - usart port descriptor or address;
 * 	mhz19bData_t* mhz - MH-Z19B working structure;
 * 	uint8_t autocal - ABC autocalibration: 0 - disable; 1 - enable.
 */
void mhzSetAutocal(portUsartDesc_t usart, mhz19bData_t* mhz, uint8_t autocal){
	memset(mhz->packet.buff, 0, MHZ19B_PACKET_LENGTH);
	mhz->packet.buff[MHZ19B_CMD_BUF_STX] = MHZ19B_STX;			// STX
	mhz->packet.buff[MHZ19B_CMD_BUF_SEN] = MHZ19B_SENSOR;		// SEN
	mhz->packet.buff[MHZ19B_CMD_BUF_CMD] = MHZ19B_CMD_AUTOCAL;	// CMD
	if(autocal == 1){											// ABC
		mhz->packet.buff[MHZ19B_CMD_BUF_DAH] = 0xa0;
	}else{
		mhz->packet.buff[MHZ19B_CMD_BUF_DAH] = 0x00;
	}
	mhz->packet.buff[MHZ19B_CMD_BUF_CRC] = mhzGetCheckSum(mhz->packet.buff);			// CRC

	for(uint8_t i = 0; i < MHZ19B_PACKET_LENGTH; i++){
		portUsartSendByte(usart, mhz->packet.buff[i]);
	}
}

/**
 * MH-Z19B - Zero point calibration
 *
 * ZERO POINT is 400PPM, PLS MAKE SURE THE SENSOR HAD BEEN WORKED UNDER 400PPM FOR OVER 20MINUTES
 * No response.
 *
 * Input:
 * 	portUsartDesc_t usart - usart port descriptor or address;
 * 	mhz19bData_t* mhz - MH-Z19B working structure.
 */
void mhzCalZeroPoint(portUsartDesc_t usart, mhz19bData_t* mhz){
	memset(mhz->packet.buff, 0, MHZ19B_PACKET_LENGTH);
	mhz->packet.buff[MHZ19B_CMD_BUF_STX] = MHZ19B_STX;			// STX
	mhz->packet.buff[MHZ19B_CMD_BUF_SEN] = MHZ19B_SENSOR;		// SEN
	mhz->packet.buff[MHZ19B_CMD_BUF_CMD] = MHZ19B_CMD_ZERO;		// CMD
	mhz->packet.buff[MHZ19B_CMD_BUF_CRC] = mhzGetCheckSum(mhz->packet.buff);			// CRC

	for(uint8_t i = 0; i < MHZ19B_PACKET_LENGTH; i++){
		portUsartSendByte(usart, mhz->packet.buff[i]);
	}
}

/**
 * MH-Z19B - Span point calibration
 *
 * Pls do ZERO calibration before span calibration.
 * Please make sure the sensor worked under a certain level co2 for over 20 minutes.
 * Suggest using 2000ppm as span, at least 1000ppm
 * No response.
 *
 * Input:
 * 	portUsartDesc_t usart - usart port descriptor or address;
 * 	mhz19bData_t* mhz - MH-Z19B working structure;
 * 	uint16_t span - span point value.
 */
void mhzCalSpanPoint(portUsartDesc_t usart, mhz19bData_t* mhz, uint16_t span){
	mhz19bSpan_t _span;
	_span.u16 = span;

	memset(mhz->packet.buff, 0, MHZ19B_PACKET_LENGTH);
	mhz->packet.buff[MHZ19B_CMD_BUF_STX] = MHZ19B_STX;			// STX
	mhz->packet.buff[MHZ19B_CMD_BUF_SEN] = MHZ19B_SENSOR;		// SEN
	mhz->packet.buff[MHZ19B_CMD_BUF_CMD] = MHZ19B_CMD_SPAN;		// CMD
	mhz->packet.buff[MHZ19B_CMD_BUF_DAH] = _span.u8[0];			// SHH
	mhz->packet.buff[MHZ19B_CMD_BUF_DAL] = _span.u8[1];			// SLL
	mhz->packet.buff[MHZ19B_CMD_BUF_CRC] = mhzGetCheckSum(mhz->packet.buff);			// CRC

	for(uint8_t i = 0; i < MHZ19B_PACKET_LENGTH; i++){
		portUsartSendByte(usart, mhz->packet.buff[i]);
	}
}

/**
 * MH-Z19B - Sensor initialization
 *
 * Input:
 * 	portUsartDesc_t usart - usart port descriptor or address;
 * 	mhz19bData_t* mhz - MH-Z19B working structure (usually global variable structure);
 * 	uint16_t range - detection range: can be MHZ19B_RANGE_LOW (max 2000ppm)
 * 						or MHZ19B_RANGE_HIGH (max 5000ppm).
 *
 * Example:
 * 	/.../
 * 	mhz19bData_t mhzSensor;							// create MH-Z19B working structure
 *	mhzSensorInit(&mhzSensor, MHZ19B_RANGE_HIGH);	// init MH-Z19B
 *	/.../
 */
void mhzSensorInit(portUsartDesc_t usart, mhz19bData_t* mhz, uint16_t range){
	memset(mhz, 0, sizeof(mhz19bData_t));
	portUsartInit();
	mhzSetRange(usart, mhz, range);
	mhzSetAutocal(usart, mhz, 0);
	mhzCleanBuffer(mhz);
}

/**
 * MH-Z19B - Request Data
 *
 * Send data request (MHZ19B_CMD_READ command) to the sensor.
 *
 * Input:
 * 	portUsartDesc_t usart - usart port descriptor or address;
 * 	mhz19bData_t* mhz - MH-Z19B working structure.
 */
void mhzRequestData(portUsartDesc_t usart, mhz19bData_t* mhz){
	memset(mhz->packet.buff, 0, MHZ19B_PACKET_LENGTH);
	mhz->packet.buff[MHZ19B_CMD_BUF_STX] = MHZ19B_STX;			// STX
	mhz->packet.buff[MHZ19B_CMD_BUF_SEN] = MHZ19B_SENSOR;		// SEN
	mhz->packet.buff[MHZ19B_CMD_BUF_CMD] = MHZ19B_CMD_READ;		// CMD
	mhz->packet.buff[MHZ19B_CMD_BUF_CRC] = 0x79;				// CRC

	for(uint8_t i = 0; i < MHZ19B_PACKET_LENGTH; i++){
		portUsartSendByte(usart, mhz->packet.buff[i]);
	}

	mhzCleanBuffer(mhz);
	mhz->packet.rxState = mhzRxWait;
}

/**
 * MH-Z19B - Receive byte Callback
 *
 * Should be called from USART RxD ISR.
 *
 * Input:
 * 	mhz19bData_t* mhz - MH-Z19B working structure;
 * 	uint8_t byte - received byte.
 */
mhz19bResult_t mhzRxByteCallback(mhz19bData_t* mhz, uint8_t byte){
	mhz19bResult_t result = mhzResError;
	result = mhzCheckBufferOverflow(mhz);

	if((result == mhzResOk) && (mhz->packet.rxState != mhzRxData)){
		*(mhz->packet.buffHead) = byte;
		switch (mhz->packet.rxState) {
			case mhzRxWait:
				if(*(mhz->packet.buffHead) == MHZ19B_STX){
					mhz->packet.rxState = mhzRxStx;
					mhz->packet.bytes_received = 1;
				}else{
					*(mhz->packet.buffHead) = 0x00;
					return result;
				}
				break;
			case mhzRxStx:
				mhz->packet.bytes_received++;

				if(mhz->packet.bytes_received >= 9){
					mhz->packet.rxState = mhzRxData;
					if((mhz->crc = mhzGetCheckSum(mhz->packet.buff)) == mhz->packet.buff[MHZ19B_RSP_BUF_CRC]){
						return mhzResRxDone;
					}else{
						mhz->result = mhzResCrcError;
					}
				}
				break;
			default:
				break;
		}

		mhz->packet.buffHead++;
	}else{
		if(mhz->packet.rxState == mhzRxData){
			result = mhzResError;
		}
		if(mhz->packet.buffHead != mhz->packet.buff){
			mhzCleanBuffer(mhz);
		}
		return result;
	}

	return result;
}

/**
 * MH-Z19B - Get CO2 Concentration Value
 *
 * Get CO2 concentration value from raw data in internal buffer.
 *
 * Input:
 * 	mhz19bDataPacket_t* packet - pointer to the first byte of received packet structure;
 * 	uint16_t range - detection range (used for overrange check);
 * 	mhz19bResult_t* result - pointer to the parsing result variable, can set the result to:
 * 											 mhzResError - (shouldn't occur),
 * 											 mhzResOk - successful parsing with apropriate result,
 * 											 mhzResCO2Overrange - parsing ok, but the value is out of range.
 * Return:
 * 	uint16_t _co2 - CO2 concentration value [ppm].
 */
uint16_t mhzGetCo2(mhz19bDataPacket_t* packet, uint16_t range, mhz19bResult_t* result){
	mhz19bResult_t _result = mhzResError;
	uint16_t _co2 = 0;

	_co2 = packet->buff[MHZ19B_RSP_BUF_CCH];
	_co2 = _co2 << 8;
	_co2 += packet->buff[MHZ19B_RSP_BUF_CCL];

	// Check for overrange
	(_co2 <= range) ? (_result = mhzResOk) : (_result = mhzResCO2Overrange);

	*result = _result;
	return _co2;
}

/**
 * MH-Z19B - Get Temperature Value
 *
 * Get temperature value from raw data in internal buffer.
 *
 * Input:
 * 	mhz19bDataPacket_t* packet - pointer to the first byte of received packet structure;
 * 	mhz19bResult_t* result - pointer to the parsing result variable, can set the result to:
 * 							mhzResError - (shouldn't occur),
 * 							mhzResOk - successful parsing with apropriate result,
 * 							mhzResTempOverrange - parsing ok, but the value is out of range.
 * Return:
 * 	uint8_t _temperature - temperature value [degC].
 */
uint8_t mhzGetTemperature(mhz19bDataPacket_t* packet, mhz19bResult_t* result){
	mhz19bResult_t _result = mhzResError;
	uint8_t _temperature = 0;

	_temperature = packet->buff[MHZ19B_RSP_BUF_TT];
	_temperature -= 40; 	// temperature offset correction

	// Check for overrange
	(_temperature <= MHZ19B_MAX_TEMPERATURE) ? (_result = mhzResOk) : (_result = mhzResTempOverrange);

	*result = _result;
	return _temperature;
}

/**
 * MH-Z19B - Get S Value
 *
 * Get S value from raw data in internal buffer.
 *
 * Input:
 * 	mhz19bDataPacket_t* packet - pointer to the first byte of received packet structure;
 * 	mhz19bResult_t* result - pointer to the parsing result variable, should always set the result to mhzResOk.
 * Return:
 * 	uint8_t _s - S value [].
 */
uint8_t mhzGetSS(mhz19bDataPacket_t* packet, mhz19bResult_t* result){
	mhz19bResult_t _result = mhzResError;	// Unused (stub)
	uint8_t _ss = 0;

	_ss = packet->buff[MHZ19B_RSP_BUF_SS];
	_result = mhzResOk;						// Stub

	*result = _result;
	return _ss;
}

/**
 * MH-Z19B - Get U Value
 *
 * Get U value from raw data in internal buffer.
 *
 * Input:
 * 	mhz19bDataPacket_t* packet - pointer to the first byte of received packet structure;
 * 	mhz19bResult_t* result - pointer to the parsing result variable, should always set the result to mhzResOk.
 * Return:
 * 	uint16_t _s - U value [].
 */
uint16_t mhzGetUU(mhz19bDataPacket_t* packet, mhz19bResult_t* result){
	mhz19bResult_t _result = mhzResError;	// Unused (stub)
	uint16_t _uu = 0;

	_uu = packet->buff[MHZ19B_RSP_BUF_UUH];
	_uu = _uu << 8;
	_uu += packet->buff[MHZ19B_RSP_BUF_UUL];
	_result = mhzResOk;						// Stub

	*result = _result;
	return _uu;
}

/**
 * MH-Z19B - Parse Packet
 *
 * Parse entire packet and fill MH-Z19B working structure.
 *
 * Input:
 * 	mhz19bData_t* mhz - MH-Z19B working structure;
 *
 * Return:
 * 	mhz19bResult_t result - parsing result: mhzResError - (shouldn't occur),
 * 							 mhzResOk - successful parsing with apropriate result,
 * 							 mhzResCO2Overrange - parsing ok, but the value of CO2 concentration is out of range,
 * 							 mhzResTempOverrange - parsing ok, but temperature value is out of range.
 */
mhz19bResult_t mhzParsePacket(mhz19bData_t* mhz){
	mhz19bResult_t result = mhzResError;

	uint16_t _co2 	= mhzGetCo2			(&(mhz->packet), mhz->range.u16, &result); 	(result != mhzResOk) ? ({return result;}) : (mhz->co2 = _co2);
	uint8_t _tmp 	= mhzGetTemperature	(&(mhz->packet), &result); 					(result != mhzResOk) ? ({return result;}) : (mhz->temperature = _tmp);
	uint8_t _s 		= mhzGetSS			(&(mhz->packet), &result); 					(result != mhzResOk) ? ({return result;}) : (mhz->_s = _s);
	uint16_t _u 	= mhzGetUU			(&(mhz->packet), &result); 					(result != mhzResOk) ? ({return result;}) : (mhz->_u = _u);

	return result;
}
