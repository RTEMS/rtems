/*---------------------------------------------------------------------------------
	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/

#ifndef NDS_CARD_INCLUDE
#define NDS_CARD_INCLUDE


#include "jtypes.h"

// Card bus
#define CARD_CR1       (*(vuint16*)0x040001A0)
#define CARD_CR1H      (*(vuint8*)0x040001A1)
#define CARD_EEPDATA   (*(vuint8*)0x040001A2)
#define CARD_CR2       (*(vuint32*)0x040001A4)
#define CARD_COMMAND   ((vuint8*)0x040001A8)

#define CARD_DATA_RD   (*(vuint32*)0x04100010)

#define CARD_1B0       (*(vuint32*)0x040001B0)
#define CARD_1B4       (*(vuint32*)0x040001B4)
#define CARD_1B8       (*(vuint16*)0x040001B8)
#define CARD_1BA       (*(vuint16*)0x040001BA)


#define CARD_CR1_ENABLE  0x80  // in byte 1, i.e. 0x8000
#define CARD_CR1_IRQ     0x40  // in byte 1, i.e. 0x4000


// CARD_CR2 register:

#define CARD_ACTIVATE   (1<<31)  // when writing, get the ball rolling
// 1<<30
#define CARD_nRESET     (1<<29)  // value on the /reset pin (1 = high out, not a reset state, 0 = low out = in reset)
#define CARD_28         (1<<28)  // when writing
#define CARD_27         (1<<27)  // when writing
#define CARD_26         (1<<26)
#define CARD_22         (1<<22)
#define CARD_19         (1<<19)
#define CARD_ENCRYPTED  (1<<14)  // when writing, this command should be encrypted
#define CARD_13         (1<<13)  // when writing
#define CARD_4          (1<<4)   // when writing

// 3 bits in b10..b8 indicate something
// read bits
#define CARD_BUSY       (1<<31)  // when reading, still expecting incomming data?
#define CARD_DATA_READY (1<<23)  // when reading, CARD_DATA_RD or CARD_DATA has another word of data and is good to go


#ifdef __cplusplus
extern "C" {
#endif


void cardWriteCommand(const uint8 * command);

void cardPolledTransfer(uint32 flags, uint32 * destination, uint32 length, const uint8 * command);
void cardStartTransfer(const uint8 * command, uint32 * destination, int channel, uint32 flags);
uint32 cardWriteAndRead(const uint8 * command, uint32 flags);

// These commands require the cart to not be initialized yet, which may mean the user
// needs to eject and reinsert the cart or they will return random data.
void cardRead00(uint32 address, uint32 * destination, uint32 length, uint32 flags);
void cardReadHeader(uint8 * header);
int cardReadID(uint32 flags);

// Reads from the EEPROM
void cardReadEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype);

// Writes to the EEPROM. TYPE 3 EEPROM must be erased first (I think?)
void cardWriteEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype);

// Returns the ID of the EEPROM chip? Doesn't work well, most chips give ff,ff
// i = 0 or 1
uint8 cardEepromReadID(uint8 i);

// Sends a command to the EEPROM
uint8 cardEepromCommand(uint8 command, uint32 address);

/*
 * -1:no card or no EEPROM
 *  0:unknown                 PassMe?
 *  1:TYPE 1  4Kbit(512Byte)  EEPROM
 *  2:TYPE 2 64Kbit(8KByte)or 512kbit(64Kbyte)   EEPROM
 *  3:TYPE 3  2Mbit(256KByte) FLASH MEMORY (some rare 4Mbit and 8Mbit chips also)
 */
int cardEepromGetType(void);

// Returns the size in bytes of EEPROM
uint32 cardEepromGetSize(void);

// Erases the entire chip. TYPE 3 chips MUST be erased before writing to them. (I think?)
void cardEepromChipErase(void);

// Erases a single sector of the TYPE 3 chip
void cardEepromSectorErase(uint32 address);

#ifdef __cplusplus
}
#endif


#endif

