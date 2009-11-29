/*
	io_cf_common.c based on

	compact_flash.c
	By chishm (Michael Chisholm)

	Common hardware routines for using a compact flash card. This is not reentrant
	and does not do range checking on the supplied addresses. This is designed to
	be as fast as possible.

	CF routines modified with help from Darkfader

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
*/


#include "io_cf_common.h"

//---------------------------------------------------------------
// DMA
#ifdef _IO_USE_DMA
 #ifndef NDS
  #include "gba_dma.h"
 #else
  #include <nds/dma.h>
  #ifdef ARM9
   #include <nds/arm9/cache.h>
  #endif
 #endif
#endif

//---------------------------------------------------------------
// CF Addresses & Commands

CF_REGISTERS cfRegisters = {0};


/*-----------------------------------------------------------------
_CF_isInserted
Is a compact flash card inserted?
bool return OUT:  true if a CF card is inserted
-----------------------------------------------------------------*/
bool _CF_isInserted (void) {
	// Change register, then check if value did change
	*(cfRegisters.status) = CF_STS_INSERTED;
	return ((*(cfRegisters.status) & 0xff) == CF_STS_INSERTED);
}


/*-----------------------------------------------------------------
_CF_clearStatus
Tries to make the CF card go back to idle mode
bool return OUT:  true if a CF card is idle
-----------------------------------------------------------------*/
bool _CF_clearStatus (void) {
	int i;

	// Wait until CF card is finished previous commands
	i=0;
	while ((*(cfRegisters.command) & CF_STS_BUSY) && (i < CF_CARD_TIMEOUT)) {
		i++;
	}

	// Wait until card is ready for commands
	i = 0;
	while ((!(*(cfRegisters.status) & CF_STS_INSERTED)) && (i < CF_CARD_TIMEOUT)) {
		i++;
	}
	if (i >= CF_CARD_TIMEOUT)
		return false;

	return true;
}


/*-----------------------------------------------------------------
_CF_readSectors
Read 512 byte sector numbered "sector" into "buffer"
u32 sector IN: address of first 512 byte sector on CF card to read
u32 numSectors IN: number of 512 byte sectors to read,
 1 to 256 sectors can be read
void* buffer OUT: pointer to 512 byte buffer to store data in
bool return OUT: true if successful
-----------------------------------------------------------------*/
bool _CF_readSectors (u32 sector, u32 numSectors, void* buffer) {
	int i;

	u16 *buff = (u16*)buffer;
#ifdef _IO_ALLOW_UNALIGNED
	u8 *buff_u8 = (u8*)buffer;
	int temp;
#endif

#if (defined _IO_USE_DMA) && (defined NDS) && (defined ARM9)
	DC_FlushRange( buffer, j * BYTES_PER_READ);
#endif

	// Wait until CF card is finished previous commands
	i=0;
	while ((*(cfRegisters.command) & CF_STS_BUSY) && (i < CF_CARD_TIMEOUT)) {
		i++;
	}

	// Wait until card is ready for commands
	i = 0;
	while ((!(*(cfRegisters.status) & CF_STS_INSERTED)) && (i < CF_CARD_TIMEOUT)) {
		i++;
	}
	if (i >= CF_CARD_TIMEOUT)
		return false;

	// Set number of sectors to read
	*(cfRegisters.sectorCount) = (numSectors < 256 ? numSectors : 0);	// Read a maximum of 256 sectors, 0 means 256

	// Set read sector
	*(cfRegisters.lba1) = sector & 0xFF;						// 1st byte of sector number
	*(cfRegisters.lba2) = (sector >> 8) & 0xFF;					// 2nd byte of sector number
	*(cfRegisters.lba3) = (sector >> 16) & 0xFF;				// 3rd byte of sector number
	*(cfRegisters.lba4) = ((sector >> 24) & 0x0F )| CF_CMD_LBA;	// last nibble of sector number

	// Set command to read
	*(cfRegisters.command) = CF_CMD_READ;


	while (numSectors--)
	{
		// Wait until card is ready for reading
		i = 0;
		while (((*(cfRegisters.status) & 0xff)!= CF_STS_READY) && (i < CF_CARD_TIMEOUT))
		{
			i++;
		}
		if (i >= CF_CARD_TIMEOUT)
			return false;

		// Read data
#ifdef _IO_USE_DMA
 #ifdef NDS
		DMA3_SRC = (u32)(cfRegisters.data);
		DMA3_DEST = (u32)buff;
		DMA3_CR = 256 | DMA_COPY_HALFWORDS | DMA_SRC_FIX;
 #else
		DMA3COPY ( (cfRegisters.data), buff, 256 | DMA16 | DMA_ENABLE | DMA_SRC_FIXED);
 #endif
		buff += BYTES_PER_READ / 2;
#elif defined _IO_ALLOW_UNALIGNED
		i=256;
		if ((u32)buff_u8 & 0x01) {
			while(i--)
			{
				temp = *(cfRegisters.data);
				*buff_u8++ = temp & 0xFF;
				*buff_u8++ = temp >> 8;
			}
		} else {
			while(i--)
				*buff++ = *(cfRegisters.data);
		}
#else
		i=256;
		while(i--)
			*buff++ = *(cfRegisters.data);
#endif
	}
#if (defined _IO_USE_DMA) && (defined NDS)
	// Wait for end of transfer before returning
	while(DMA3_CR & DMA_BUSY);
#endif
	return true;
}



/*-----------------------------------------------------------------
_CF_writeSectors
Write 512 byte sector numbered "sector" from "buffer"
u32 sector IN: address of 512 byte sector on CF card to read
u32 numSectors IN: number of 512 byte sectors to read,
 1 to 256 sectors can be read
void* buffer IN: pointer to 512 byte buffer to read data from
bool return OUT: true if successful
-----------------------------------------------------------------*/
bool _CF_writeSectors (u32 sector, u32 numSectors, void* buffer) {
	int i;

	u16 *buff = (u16*)buffer;
#ifdef _IO_ALLOW_UNALIGNED
	u8 *buff_u8 = (u8*)buffer;
	int temp;
#endif

#if defined _IO_USE_DMA && defined NDS && defined ARM9
	DC_FlushRange( buffer, j * BYTES_PER_READ);
#endif

	// Wait until CF card is finished previous commands
	i=0;
	while ((*(cfRegisters.command) & CF_STS_BUSY) && (i < CF_CARD_TIMEOUT))
	{
		i++;
	}

	// Wait until card is ready for commands
	i = 0;
	while ((!(*(cfRegisters.status) & CF_STS_INSERTED)) && (i < CF_CARD_TIMEOUT))
	{
		i++;
	}
	if (i >= CF_CARD_TIMEOUT)
		return false;

	// Set number of sectors to write
	*(cfRegisters.sectorCount) = (numSectors < 256 ? numSectors : 0);	// Write a maximum of 256 sectors, 0 means 256

	// Set write sector
	*(cfRegisters.lba1) = sector & 0xFF;						// 1st byte of sector number
	*(cfRegisters.lba2) = (sector >> 8) & 0xFF;					// 2nd byte of sector number
	*(cfRegisters.lba3) = (sector >> 16) & 0xFF;				// 3rd byte of sector number
	*(cfRegisters.lba4) = ((sector >> 24) & 0x0F )| CF_CMD_LBA;	// last nibble of sector number

	// Set command to write
	*(cfRegisters.command) = CF_CMD_WRITE;

	while (numSectors--)
	{
		// Wait until card is ready for writing
		i = 0;
		while (((*(cfRegisters.status) & 0xff) != CF_STS_READY) && (i < CF_CARD_TIMEOUT))
		{
			i++;
		}
		if (i >= CF_CARD_TIMEOUT)
			return false;

		// Write data
#ifdef _IO_USE_DMA
 #ifdef NDS
		DMA3_SRC = (u32)buff;
		DMA3_DEST = (u32)(cfRegisters.data);
		DMA3_CR = 256 | DMA_COPY_HALFWORDS | DMA_DST_FIX;
 #else
		DMA3COPY( buff, (cfRegisters.data), 256 | DMA16 | DMA_ENABLE | DMA_DST_FIXED);
 #endif
		buff += BYTES_PER_READ / 2;
#elif defined _IO_ALLOW_UNALIGNED
		i=256;
		if ((u32)buff_u8 & 0x01) {
			while(i--)
			{
				temp = *buff_u8++;
				temp |= *buff_u8++ << 8;
				*(cfRegisters.data) = temp;
			}
		} else {
		while(i--)
			*(cfRegisters.data) = *buff++;
		}
#else
		i=256;
		while(i--)
			*(cfRegisters.data) = *buff++;
#endif
	}
#if defined _IO_USE_DMA && defined NDS
	// Wait for end of transfer before returning
	while(DMA3_CR & DMA_BUSY);
#endif

	return true;
}

/*-----------------------------------------------------------------
_CF_shutdown
shutdown the CF interface
-----------------------------------------------------------------*/
bool _CF_shutdown(void) {
	return _CF_clearStatus() ;
}

/*-----------------------------------------------------------------
_CF_startUp
Initializes the CF interface using the supplied registers
returns true if successful, otherwise returns false
-----------------------------------------------------------------*/
bool _CF_startup(const CF_REGISTERS *usableCfRegs) {
	cfRegisters = *usableCfRegs;
	// See if there is a read/write register
	u16 temp = *(cfRegisters.lba1);
	*(cfRegisters.lba1) = (~temp & 0xFF);
	temp = (~temp & 0xFF);
	if (!(*(cfRegisters.lba1) == temp)) {
		return false;
	}
	// Make sure it is 8 bit
	*(cfRegisters.lba1) = 0xAA55;
	if (*(cfRegisters.lba1) == 0xAA55) {
		return false;
	}
	return true;
}

