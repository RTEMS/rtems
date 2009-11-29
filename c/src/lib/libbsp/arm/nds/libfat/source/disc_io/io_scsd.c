/*
	io_scsd.c

	Hardware Routines for reading a Secure Digital card
	using the SC SD

	Some code based on scsd_c.c, written by Amadeus
	and Jean-Pierre Thomasset as part of DSLinux.

 Copyright (c) 2006 Michael "Chishm" Chisholm

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
  3. The name of the author may not be used to endorse or promote products derived
     from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	2006-07-22 - Chishm
		* First release of stable code

	2006-07-25 - Chishm
		* Improved startup function that doesn't delay hundreds of seconds
		  before reporting no card inserted.

	2006-08-05 - Chishm
		* Tries multiple times to get a Relative Card Address at startup

	2006-08-07 - Chishm
		* Moved the SD initialization to a common function

	2006-08-19 - Chishm
		* Added SuperCard Lite support
*/

#include "io_scsd.h"
#include "io_sd_common.h"
#include "io_sc_common.h"

//---------------------------------------------------------------
// SCSD register addresses
#define REG_SCSD_CMD	(*(vu16*)(0x09800000))
	/* bit 0: command bit to read  		*/
	/* bit 7: command bit to write 		*/

#define REG_SCSD_DATAWRITE	(*(vu16*)(0x09000000))
#define REG_SCSD_DATAREAD	(*(vu16*)(0x09100000))
#define REG_SCSD_DATAREAD_32	(*(vu32*)(0x09100000))
#define REG_SCSD_LITE_ENABLE	(*(vu16*)(0x09440000))
#define REG_SCSD_LOCK		(*(vu16*)(0x09FFFFFE))
	/* bit 0: 1				*/
	/* bit 1: enable IO interface (SD,CF)	*/
	/* bit 2: enable R/W SDRAM access 	*/

//---------------------------------------------------------------
// Responses
#define SCSD_STS_BUSY			0x100
#define SCSD_STS_INSERTED		0x300

//---------------------------------------------------------------
// Send / receive timeouts, to stop infinite wait loops
#define NUM_STARTUP_CLOCKS 100	// Number of empty (0xFF when sending) bytes to send/receive to/from the card
#define TRANSMIT_TIMEOUT 100000 // Time to wait for the SC to respond to transmit or receive requests
#define RESPONSE_TIMEOUT 256	// Number of clocks sent to the SD card before giving up
#define BUSY_WAIT_TIMEOUT 500000
#define WRITE_TIMEOUT	3000	// Time to wait for the card to finish writing
//---------------------------------------------------------------
// Variables required for tracking SD state
static u32 _SCSD_relativeCardAddress = 0;	// Preshifted Relative Card Address

//---------------------------------------------------------------
// Internal SC SD functions

extern bool _SCSD_writeData_s (u8 *data, u16* crc);

static inline void _SCSD_unlock (void) {
	_SC_changeMode (SC_MODE_MEDIA);
}

static inline void _SCSD_enable_lite (void) {
	REG_SCSD_LITE_ENABLE = 0;
}

static bool _SCSD_sendCommand (u8 command, u32 argument) {
	u8 databuff[6];
	u8 *tempDataPtr = databuff;
	int length = 6;
	u16 dataByte;
	int curBit;
	int i;

	*tempDataPtr++ = command | 0x40;
	*tempDataPtr++ = argument>>24;
	*tempDataPtr++ = argument>>16;
	*tempDataPtr++ = argument>>8;
	*tempDataPtr++ = argument;
	*tempDataPtr = _SD_CRC7 (databuff, 5);

	i = BUSY_WAIT_TIMEOUT;
	while (((REG_SCSD_CMD & 0x01) == 0) && (--i));
	if (i == 0) {
		return false;
	}

	dataByte = REG_SCSD_CMD;

	tempDataPtr = databuff;

	while (length--) {
		dataByte = *tempDataPtr++;
		for (curBit = 7; curBit >=0; curBit--){
			REG_SCSD_CMD = dataByte;
			dataByte = dataByte << 1;
		}
	}

	return true;
}

// Returns the response from the SD card to a previous command.
static bool _SCSD_getResponse (u8* dest, u32 length) {
	u32 i;
	int dataByte;
	int numBits = length * 8;

	// Wait for the card to be non-busy
	i = BUSY_WAIT_TIMEOUT;
	while (((REG_SCSD_CMD & 0x01) != 0) && (--i));
	if (dest == NULL) {
		return true;
	}

	if (i == 0) {
		// Still busy after the timeout has passed
		return false;
	}

	// The first bit is always 0
	dataByte = 0;
	numBits--;
	// Read the remaining bits in the response.
	// It's always most significant bit first
	while (numBits--) {
		dataByte = (dataByte << 1) | (REG_SCSD_CMD & 0x01);
		if ((numBits & 0x7) == 0) {
			// It's read a whole byte, so store it
			*dest++ = (u8)dataByte;
			dataByte = 0;
		}
	}

	// Send 16 more clocks, 8 more than the delay required between a response and the next command
	for (i = 0; i < 16; i++) {
		dataByte = REG_SCSD_CMD;
	}

	return true;
}

static inline bool _SCSD_getResponse_R1 (u8* dest) {
	return _SCSD_getResponse (dest, 6);
}

static inline bool _SCSD_getResponse_R1b (u8* dest) {
	return _SCSD_getResponse (dest, 6);
}

static inline bool _SCSD_getResponse_R2 (u8* dest) {
	return _SCSD_getResponse (dest, 17);
}

static inline bool _SCSD_getResponse_R3 (u8* dest) {
	return _SCSD_getResponse (dest, 6);
}

static inline bool _SCSD_getResponse_R6 (u8* dest) {
	return _SCSD_getResponse (dest, 6);
}

static void _SCSD_sendClocks (u32 numClocks) {
	u16 temp;
	do {
		temp = REG_SCSD_CMD;
	} while (numClocks--);
}

bool _SCSD_cmd_6byte_response (u8* responseBuffer, u8 command, u32 data) {
	_SCSD_sendCommand (command, data);
	return _SCSD_getResponse (responseBuffer, 6);
}

bool _SCSD_cmd_17byte_response (u8* responseBuffer, u8 command, u32 data) {
	_SCSD_sendCommand (command, data);
	return _SCSD_getResponse (responseBuffer, 17);
}


static bool _SCSD_initCard (void) {
	_SCSD_enable_lite();

	// Give the card time to stabilise
	_SCSD_sendClocks (NUM_STARTUP_CLOCKS);

	// Reset the card
	if (!_SCSD_sendCommand (GO_IDLE_STATE, 0)) {
		return false;
	}

	_SCSD_sendClocks (NUM_STARTUP_CLOCKS);

	// Card is now reset, including it's address
	_SCSD_relativeCardAddress = 0;

	// Init the card
	return _SD_InitCard (_SCSD_cmd_6byte_response,
				_SCSD_cmd_17byte_response,
				true,
				&_SCSD_relativeCardAddress);
}

static bool _SCSD_readData (void* buffer) {
	u8* buff_u8 = (u8*)buffer;
	u16* buff = (u16*)buffer;
	volatile register u32 temp;
	int i;

	i = BUSY_WAIT_TIMEOUT;
	while ((REG_SCSD_DATAREAD & SCSD_STS_BUSY) && (--i));
	if (i == 0) {
		return false;
	}

	i=256;
	if ((u32)buff_u8 & 0x01) {
		while(i--) {
			temp = REG_SCSD_DATAREAD_32;
			temp = REG_SCSD_DATAREAD_32 >> 16;
			*buff_u8++ = (u8)temp;
			*buff_u8++ = (u8)(temp >> 8);
		}
	} else {
		while(i--) {
			temp = REG_SCSD_DATAREAD_32;
			temp = REG_SCSD_DATAREAD_32 >> 16;
			*buff++ = temp;
		}
	}


	for (i = 0; i < 8; i++) {
		temp = REG_SCSD_DATAREAD_32;
	}

	temp = REG_SCSD_DATAREAD;

	return true;
}

//---------------------------------------------------------------
// Functions needed for the external interface

bool _SCSD_startUp (void) {
	_SCSD_unlock();
	return _SCSD_initCard();
}

bool _SCSD_isInserted (void) {
	u8 responseBuffer [6];

	// Make sure the card receives the command
	if (!_SCSD_sendCommand (SEND_STATUS, 0)) {
		return false;
	}
	// Make sure the card responds
	if (!_SCSD_getResponse_R1 (responseBuffer)) {
		return false;
	}
	// Make sure the card responded correctly
	if (responseBuffer[0] != SEND_STATUS) {
		return false;
	}
	return true;
}

bool _SCSD_readSectors (u32 sector, u32 numSectors, void* buffer) {
	u32 i;
	u8* dest = (u8*) buffer;
	u8 responseBuffer[6];

	if (numSectors == 1) {
		// If it's only reading one sector, use the (slightly faster) READ_SINGLE_BLOCK
		if (!_SCSD_sendCommand (READ_SINGLE_BLOCK, sector * BYTES_PER_READ)) {
			return false;
		}

		if (!_SCSD_readData (buffer)) {
			return false;
		}

	} else {
		// Stream the required number of sectors from the card
		if (!_SCSD_sendCommand (READ_MULTIPLE_BLOCK, sector * BYTES_PER_READ)) {
			return false;
		}

		for(i=0; i < numSectors; i++, dest+=BYTES_PER_READ) {
			if (!_SCSD_readData(dest)) {
				return false;
			}
		}

		// Stop the streaming
		_SCSD_sendCommand (STOP_TRANSMISSION, 0);
		_SCSD_getResponse_R1b (responseBuffer);
	}

	_SCSD_sendClocks(0x10);
	return true;
}

bool _SCSD_writeSectors (u32 sector, u32 numSectors, const void* buffer) {
	u16 crc[4];	// One per data line
	u8 responseBuffer[6];
	u32 offset = sector * BYTES_PER_READ;
	u8* data = (u8*) buffer;
	int i;

	while (numSectors--) {
		// Calculate the CRC16
		_SD_CRC16 ( data, BYTES_PER_READ, (u8*)crc);

		// Send write command and get a response
		_SCSD_sendCommand (WRITE_BLOCK, offset);
		if (!_SCSD_getResponse_R1 (responseBuffer)) {
			return false;
		}

		// Send the data and CRC
		if (! _SCSD_writeData_s (data, crc)) {
			return false;
		}

		// Send a few clocks to the SD card
		_SCSD_sendClocks(0x10);

		offset += BYTES_PER_READ;
		data += BYTES_PER_READ;

		// Wait until card is finished programming
		i = WRITE_TIMEOUT;
		responseBuffer[3] = 0;
		do {
			_SCSD_sendCommand (SEND_STATUS, _SCSD_relativeCardAddress);
			_SCSD_getResponse_R1 (responseBuffer);
			i--;
			if (i <= 0) {
				return false;
			}
		} while (((responseBuffer[3] & 0x1f) != ((SD_STATE_TRAN << 1) | READY_FOR_DATA)));
	}

	return true;
}

bool _SCSD_clearStatus (void) {
	return _SCSD_initCard ();
}

bool _SCSD_shutdown (void) {
	_SC_changeMode (SC_MODE_RAM_RO);
	return true;
}

const IO_INTERFACE _io_scsd = {
	DEVICE_TYPE_SCSD,
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_GBA,
	(FN_MEDIUM_STARTUP)&_SCSD_startUp,
	(FN_MEDIUM_ISINSERTED)&_SCSD_isInserted,
	(FN_MEDIUM_READSECTORS)&_SCSD_readSectors,
	(FN_MEDIUM_WRITESECTORS)&_SCSD_writeSectors,
	(FN_MEDIUM_CLEARSTATUS)&_SCSD_clearStatus,
	(FN_MEDIUM_SHUTDOWN)&_SCSD_shutdown
} ;


