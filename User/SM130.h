/**
 * 	@file	SM130.h
 * 	@brief	Header file for SM130 library
 *	@author	Marc Boon <http://www.marcboon.com>
 *	@date	February 2012
 */

#ifndef SM130_h
#define SM130_h

#include "misc.h"

#define SIZE_PAYLOAD 18 // maximum payload size of I2C packet
#define SIZE_PACKET (SIZE_PAYLOAD + 2) // total I2C packet size, including length char and checksum

#define halt haltTag // deprecated function halt() renamed to haltTag()

// Global functions
void printArrayAscii(char array[], int len);
void printArrayHex(char array[], int len);
void printHex(char val);

/**	Class representing a <a href="http://www.sonmicro.com/en/index.php?option=com_content&view=article&id=57&Itemid=70">SonMicro SM130 RFID module</a>.
 *
 *	Nearly complete implementation of the <a href="http://www.sonmicro.com/en/downloads/Mifare/ds_SM130.pdf">SM130 datasheet</a>.<br>
 *	Functions dealing with value blocks and stored keys are not implemented.
 */

static struct transmission {
    char length;
    char command;
    char data[SIZE_PACKET];
    char csum;
    char inprogress; //1 = inprogress
} inpacket = {0,0,"",0,1};

	char data[SIZE_PACKET]; //!< packet data
	char versionString[8]; //!< version string
	char tagNumber[7]; //!< tag number as char array
	int tagLength; //!< length of tag number in chars (4 or 7)
	char tagString[15]; //!< tag number as hex string
	char tagType; //!< type of tag
	char errorCode; //!< error code from some commands
	char antennaPower; //!< antenna power level
	char cmd; //!< last sent command

	#define VERSION  1  //!< version of this library

	#define MIFARE_ULTRALIGHT  1
	#define MIFARE_1K  2
	#define MIFARE_4K  3

	#define CMD_RESET  0x80
	#define CMD_VERSION  0x81
	#define CMD_SEEK_TAG  0x82
	#define CMD_SELECT_TAG  0x83
	#define CMD_AUTHENTICATE  0x85
	#define CMD_READ16  0x86
	#define CMD_READ_VALUE  0x87
	#define CMD_WRITE16  0x89
	#define CMD_WRITE_VALUE  0x8a
	#define CMD_WRITE4  0x8b
	#define CMD_WRITE_KEY  0x8c
	#define CMD_INC_VALUE  0x8d
	#define CMD_DEC_VALUE  0x8e
	#define CMD_ANTENNA_POWER  0x90
	#define CMD_READ_PORT  0x91
	#define CMD_WRITE_PORT  0x92
	#define CMD_HALT_TAG  0x93
	#define CMD_SET_BAUD  0x94
	#define CMD_SLEEP  0x96


	//! Send single-char command
	void SM130_sendCommand(char cmd);
	void SM130_transmitData();
	int SM130_receiveData();
	//! Returns human-readable tag name corresponding to tag type
    char* SM130_tagName(char type);
	//! Hardware or software reset of the SM130 module
	void SM130_reset();
	//! Returns a null-terminated string with the firmware version of the SM130 module
	char* SM130_getFirmwareVersion();
	//! Returns true if a response packet is available
	int SM130_available();
	//! Returns a pointer to the response packet
	char* SM130_getRawData() ;
	//! Returns the last executed command
	char SM130_getCommand() ;
	//! Returns the packet length, excluding checksum
	char SM130_getPacketLength() ;
	//! Returns the checksum
	char SM130_getCheckSum() ;
	//! Returns a pointer to the packet payload
	char* SM130_getPayload() ;
	//! Returns the block number for read/write commands
	char SM130_getBlockNumber() ;
	//! Returns a pointer to the read block (with a length of 16 chars)
	char* SM130_getBlock() ;
	//! Returns the tag's serial number as a char array
	char* SM130_getTagNumber() ;
	//! Returns the length of the tag's serial number obtained by getTagNumer()
	char SM130_getTagLength() ;
	//! Returns the tag's serial number as a hexadecimal null-terminated string
	char* SM130_getTagString() ;
	//! Returns the tag type (SM130::MIFARE_XX)
	char SM130_getTagType() ;
	//! Returns the tag type as a null-terminated string
	char* SM130_getTagName() ;
	//! Returns the error code of the last executed command
	char SM130_getErrorCode() ;
	//! Returns a human-readable error message corresponding to the error code
	char* SM130_getErrorMessage();
	//! Returns the antenna power level (0 or 1)
	char SM130_getAntennaPower() ;
	//! Sends a SEEK_TAG command
	void SM130_seekTag() ;
	//! Sends a SELECT_TAG command
	void SM130_selectTag() ;
	//! Sends a HALT_TAG command
	void SM130_haltTag() ;
	//! Set antenna power (on/off)
	void SM130_setAntennaPower(char level);
	//! Sends a SLEEP command (can only wake-up with hardware reset!)
	void SM130_sleep() ;
	//! Writes a null-terminated string of maximum 15 characters
	void SM130_writeBlock(char block, char* message);
	//! Writes a null-terminated string of maximum 3 characters to a Mifare Ultralight
	void SM130_writeFourcharBlock(char block, char* message);
	//! Sends a AUTHENTICATE command using the specified key
	void SM130_authenticate(char block, char keyType, char key[6]);
	//! Reads a 16-char block
	void SM130_readBlock(char block);

    void TM_USART1_ReceiveHandler(uint8_t c);
#endif // SM130_h
