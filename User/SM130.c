/**
 * 	@file	SM130.cpp
 * 	@brief	SM130 library for STM32
 *	@author	Dennis Wollersheim
 *	Adapted from Marc Boon <http://www.marcboon.com>
 *	@date	August 2014
 *
 *	<p>
 *	Controls a SonMicro SM130/mini RFID reader  by USART
 *	</p>
 *	@see	http://www.arduino.cc
 *	@see	http://www.sonmicro.com/1356/sm130.php
 *	@see	http://rfid.marcboon.com
 */

#include "SM130.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_usart.h"
#include "main.h"
#include <math.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define debug 1

// local functions
void arrayToHex(char *s, char array[], int len);
char toHex(char b);

	char* SM130_getRawData() { return inpacket.data; };
	//! Returns the last executed command
	char SM130_getCommand() { return inpacket.command; };
	//! Returns the packet length, excluding checksum
	char SM130_getPacketLength() { return inpacket.length+1; };
	//! Returns the checksum
	char SM130_getCheckSum() { return inpacket.csum; };
	//! Returns the block number for read/write commands
	char SM130_getBlockNumber() { return inpacket.data[0]; };
	//! Returns a pointer to the read block (with a length of 16 chars)
	char* SM130_getBlock() { return inpacket.data+1; };
	//! Returns the tag's serial number as a char array
	char* SM130_getTagNumber() { return tagNumber; };
	//! Returns the length of the tag's serial number obtained by getTagNumer()
	char SM130_getTagLength() { return tagLength; };
	//! Returns the tag's serial number as a hexadecimal null-terminated string
	char* SM130_getTagString() { return tagString; };
	//! Returns the tag type (SM130::MIFARE_XX)
	char SM130_getTagType() { return tagType; };
	//! Returns the tag type as a null-terminated string
	char* SM130_getTagName() { return SM130_tagName(tagType); };
	//! Returns the error code of the last executed command
	char SM130_getErrorCode() { return errorCode; };
	//! Returns the antenna power level (0 or 1)
	char SM130_getAntennaPower() { return antennaPower; };
	//! Sends a SEEK_TAG command
	void SM130_seekTag() { SM130_sendCommand(CMD_SEEK_TAG); };
	//! Sends a SELECT_TAG command
	void SM130_selectTag() { SM130_sendCommand(CMD_SELECT_TAG); };
	//! Sends a HALT_TAG command
	void SM130_haltTag() { SM130_sendCommand(CMD_HALT_TAG); };
	//! Sends a SLEEP command (can only wake-up with hardware reset!)
	void SM130_sleep() { SM130_sendCommand(CMD_SLEEP); };


/* Public member functions ****************************************************/


/**	Reset the SM130 module
 *
 * 	This function should be called in setup(). It initializes the IO pins and
 *	issues a hardware or software reset, depending on the definition of pinRESET.
 *
 *	Wire.begin() should also be called in setup(), and Wire.h should be included.
 *
 *	If pinRESET has the value 0xff (-1), software reset over I2C will be used.
 *	If pinDREADY has the value 0xff (-1), the SM130 will be polled over I2C while
 *	in SEEK mode, otherwise the DREADY pin will be polled in SEEK mode.
 *	For other commands, response polling is always over I2C.
 */
void SM130_reset()
{
    TM_USART_Init(USART1, TM_USART_PinsPack_1, 19200);

	SM130_sendCommand(CMD_RESET);

	// Allow enough time for reset
	Delay(200);

	// Set antenna power
	SM130_setAntennaPower(1);

	// To cancel automatic seek mode after reset, we send a HALT_TAG command
	SM130_haltTag();
}

/**	Get the firmware version string.
 */
char* SM130_getFirmwareVersion()
{
	// return immediately if version string already retrieved
	if (*versionString != 0)
		return versionString;

	// else send VERSION command and retry a few times if no response
	for (char n = 0; n < 10; n++)
	{
		SM130_sendCommand(CMD_VERSION);
		if (SM130_available() && SM130_getCommand() == CMD_VERSION)
			return versionString;
		Delay(100);
	}
	// time-out after 1s
	return 0;
}

/**	Checks for availability of a valid response packet.
 *
 *	This function should always be called and return TRUE prior to using results
 *	of a command.
 *
 *	@returns	TRUE if a valid response packet is available
 */
int SM130_available()
{
	
    sendStringViaUSB("checking For Available Data \n\r");
	
	// Set the maximum length of the expected response packet
	int len;
	switch(cmd)
	{
	case CMD_ANTENNA_POWER:
	case CMD_AUTHENTICATE:
	case CMD_DEC_VALUE:
	case CMD_INC_VALUE:
	case CMD_WRITE_KEY:
	case CMD_HALT_TAG:
	case CMD_SLEEP:
		len = 4;
		break;
	case CMD_WRITE4:
	case CMD_WRITE_VALUE:
	case CMD_READ_VALUE:
		len = 8;
	case CMD_SEEK_TAG:
	case CMD_SELECT_TAG:
		len = 11;
		break;
	default:
		len = SIZE_PACKET;
	}

	// If valid data received, process the response packet
	if (SM130_receiveData() > 0)
	{
        sendStringViaUSB("found data\n\r");
		// Init response variables
		tagType = tagLength = *tagString = 0;

		// If packet length is 2, the command failed. Set error code.
		errorCode = SM130_getPacketLength() < 3 ? inpacket.data[1] : 0;

		// Process command response
		switch (SM130_getCommand())
		{
		case CMD_RESET:
		case CMD_VERSION:
			// RESET and VERSION commands produce the firmware version
			len = fmin(SM130_getPacketLength(), sizeof(versionString)) - 1;
			memcpy(versionString, inpacket.data + 1, len);
			versionString[len] = 0;
			break;

		case CMD_SEEK_TAG:
		case CMD_SELECT_TAG:
			// If no error, get tag number
			if(errorCode == 0 && SM130_getPacketLength() >= 6)
			{
				tagLength = SM130_getPacketLength() - 2;
				tagType = inpacket.data[1];
				memcpy(tagNumber, inpacket.data + 2, tagLength);
				arrayToHex(tagString, tagNumber, tagLength);
			}
			break;

		case CMD_AUTHENTICATE:
			break;

		case CMD_READ16:
			break;

		case CMD_WRITE16:
		case CMD_WRITE4:
			break;

		case CMD_ANTENNA_POWER:
			errorCode = 0;
			antennaPower = inpacket.data[0];
			break;

		case CMD_SLEEP:
			// If in SLEEP mode, no data is available
			return FALSE;
		}

		// Data available
		return TRUE;
	}
	// No data available
	return FALSE;
}

/**	Get error message for last command.
 *
 *	@return	Human-readable error message as a null-terminated string
 */
char* SM130_getErrorMessage()
{
	switch(errorCode)
	{
	case 'L':
		if(SM130_getCommand() == CMD_SEEK_TAG) return "Seek in progress";
	case 0:
		return "OK";
	case 'N':
		if(SM130_getCommand() == CMD_WRITE_KEY) return "Write master key failed";
		if(SM130_getCommand() == CMD_SET_BAUD) return "Set baud rate failed";
		if(SM130_getCommand() == CMD_AUTHENTICATE) return "No tag present or login failed";
		return "No tag present";
	case 'U':
		if(SM130_getCommand() == CMD_AUTHENTICATE) return "Authentication failed";
		if(SM130_getCommand() == CMD_WRITE16 || SM130_getCommand() == CMD_WRITE4) return "Verification failed";
		return "Antenna off";
	case 'F':
		if(SM130_getCommand() == CMD_READ16) return "Read failed";
		return "Write failed";
	case 'I':
		return "Invalid value block";
	case 'X':
		return "Block is read-protected";
	case 'E':
		return "Invalid key format in EEPROM";
	default:
		return "Unknown error";
	}
}

/**	Turns on/off the RF field.
 *
 *	@param level 0 is off, anything else is on
 */
void SM130_setAntennaPower(char level)
{
	antennaPower = level;
	data[0] = 2;
	data[1] = CMD_ANTENNA_POWER;
	data[2] = antennaPower;
	SM130_transmitData();
}

/** Authenticate with transport key (0xFFFFFFFFFFFF).
 *
 *	@param block Block number
 */
/*
void SM130_authenticate(char block)
{
	data[0] = 3;
	data[1] = CMD_AUTHENTICATE;
	data[2] = block;
	data[3] = 0xff;
	transmitData();
}
*/
/** Authenticate with specified key A or key B.
 *
 *	@param block Block number
 *	@param keyType Which key to use: 0xAA for key A or 0xBB for key B
 *	@param key Key value (6 chars)
 */
/*
void SM130_authenticate(char block, char keyType, char key[6])
{
	data[0] = 9;
	data[1] = CMD_AUTHENTICATE;
	data[2] = block;
	data[3] = keyType;
	memcpy(data + 4, key, 6);
	transmitData();
}
*/
/**	Read 16-char block.
 *
 *	@param block Block number
 */
void SM130_readBlock(char block)
{
	data[0] = 2;
	data[1] = CMD_READ16;
	data[2] = block;
	SM130_transmitData();
}

/**	Write 16-char block.
 *
 *	The block will be padded with zeroes if the message is shorter
 *	than 15 characters.
 *
 *	@param block Block number
 *	@param message Null-terminated string of up to 15 characters
 */
void SM130_writeBlock(char block, char* message)
{
	data[0] = 18;
	data[1] = CMD_WRITE16;
	data[2] = block;
	strncpy((char*)data + 3, message, 15);
	data[18] = 0;
	SM130_transmitData();
}

/**	Write 4-char block.
 *
 *	This command is used for Mifare Ultralight tags which have 4 char blocks.
 *
 *	@param block Block number
 *	@param message Null-terminated string of up to 3 characters
 */
void SM130_writeFourcharBlock(char block, char* message)
{
	data[0] = 6;
	data[1] = CMD_WRITE4;
	data[2] = block;
	strncpy((char*)data + 3, message, 3);
	data[6] = 0;
	SM130_transmitData();
}

/**	Send 1-char command.
 *
 *	@param cmd Command
 */
void SM130_sendCommand(char cmd)
{
	data[0] = 1;
	data[1] = cmd;
	SM130_transmitData();
}

/* Private member functions ****************************************************/


/**	Transmit a packet with checksum to the SM130.
 */
void SM130_transmitData()
{

    // init checksum and packet length
    char sum = 0;
    int len = data[0] + 2;

    // remember which command was sent
    cmd = data[1];
    TM_USART_Putc(USART1, 0xFF);
    TM_USART_Putc(USART1, 0x00);

    // transmit packet with checksum
    for (int i = 0; i < len; i++)
    {
        TM_USART_Putc(USART1, data[i]);
        sum += data[i];
    }
    TM_USART_Putc(USART1, sum);
    // show transmitted packet for debugging
    if (debug)
    {
        sendStringViaUSB("> ");
        printArrayHex(data, len);
        sendStringViaUSB("checksum=");
        printHex(sum);
        sendStringViaUSB("\n\r");
    }
}


static int state=0;
static int pos=0;
static int haserror=0;

void TM_USART1_ReceiveHandler(uint8_t c) {
    TM_USB_VCP_Putc('.');
    TM_USB_VCP_Putc(c);
    TM_USB_VCP_Putc('.');
    
    if (state==0) {
        if (c==0xFF) {
            inpacket.length=0;
            inpacket.csum=0;
            state = 1;
            inpacket.inprogress=1; 
            haserror=0; 
        } else {
            haserror = 1;
        };
    } else if (state ==1) {
        if (c==0x00) {
            state = 2;
        } else { 
            state=0;
            pos=0;
            haserror=2;
        };
    } else if (state == 2 ) { // len
        inpacket.length = c;
        pos=0;
        inpacket.csum += c;
        state =3;
    } else if (state == 3 ) { //command
        inpacket.command = c;
        inpacket.csum += c;
        state =4;
        if (inpacket.length==0) state = 5;
    } else if (state == 4 ) { //response;
        if (pos==(inpacket.length-1))  {
            state=5;
        }
        inpacket.csum += c;
        inpacket.data[ pos++ ] = c;
    } else if (state == 5 ) {
        if (c==inpacket.csum) {
            state = 0;
            inpacket.inprogress=0;
        } else { 
            haserror=4;
            state=0;
        };
    };
}
/**	Receives a packet from the SM130 and verifies the checksum.
 *
 *	@param length the number of chars to receive
 *	@return the number of chars in the payload, or -1 if bad checksum
 */


int SM130_receiveData()
{
    while ((haserror ==0 ) && (inpacket.inprogress ==1));
    if( haserror) {
        if(debug) {
            sendStringViaUSB("Error < ");
            printHex(haserror);
            sendStringViaUSB("\n\r");
        }
        return (-1);
    } 
    if (debug ) {
        sendStringViaUSB("Command < ");
        printArrayHex(inpacket.data, inpacket.length);
        sendStringViaUSB("\n\r");
    }
    return inpacket.length;
}

/**	Maps tag types to names.
 *
 *	@param	type numeric tag type
 *	@return	Human-readable tag name as null-terminated string
 */
char* SM130_tagName(char type)
{
	switch(type)
	{
	case 1: return "Mifare UL";
	case 2: return "Mifare 1K";
	case 3:	return "Mifare 4K";
	default: return "Unknown Tag";
	}
}

// Global helper functions

/**	Convert char array to null-terminated hexadecimal string.
 *
 *	@param	s	pointer to destination string
 *	@param	array	char array to convert
 *	@param	len		length of char array to convert
 */
void arrayToHex(char *s, char array[], int len)
{
	for (int i = 0; i < len; i++)
	{
		*s++ = toHex(array[i] >> 4);
		*s++ = toHex(array[i]);
		*s++ = '-';
	}
	*s = 0;
}

/**	Convert low-nibble of char to ASCII hex.
 *
 *	@param	b	char to convert
 *	$return	uppercase hexadecimal character [0-9A-F]
 */
char toHex(char b)
{
	b = b & 0x0f;
	return b < 10 ? b + '0' : b + 'A' - 10;
}

/**	Print char array as ASCII string.
 *
 *	Non-printable characters (<0x20 or >0x7E) are printed as dot.
 *
 *	@param	array char array
 *	@param	len length of char array
 */
void printArrayAscii(char array[], int len)
{
  for (int i = 0; i < len;)
  {
    char c = array[i++];
    if (c < 0x20 || c > 0x7e)
    {
      sendCharViaUSB('.');
    }
    else
    {
      sendCharViaUSB(c);
    }
  }
}

/**	Print char array as hexadecimal character pairs.
 *
 *	@param	array char array
 *	@param	len length of char array
 */
void printArrayHex(char array[], int len)
{
    char s[ len * 3 +1 ];
    arrayToHex( s, array, len);
    sendStringViaUSB(s);
    sendStringViaUSB("\n\r");
}

void printHex(char val)
{
		sendCharViaUSB( toHex(val >> 4));
		sendCharViaUSB( toHex(val ));
}

