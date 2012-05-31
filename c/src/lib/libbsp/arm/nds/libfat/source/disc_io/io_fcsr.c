/*
	io_fcsr.c based on

	compact_flash.c
	By chishm (Michael Chisholm)

	Hardware Routines for using a GBA Flash Cart and SRAM as a
	block device.

	The file system must be 512 byte aligned, in cart address space.
	SRAM is supported.

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


#include "io_fcsr.h"

#include <string.h>

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

#ifdef NDS
 #define SRAM_START 0x0A000000
#else
 #define SRAM_START 0x0E000000
#endif

#define NO_SRAM 0xFFFFFFFF

#define FCSR 0x52534346
const char _FCSR_LabelString[] = " Chishm FAT";

u8* _FCSR_FileSysPointer = 0;
u8* _FCSR_SramSectorPointer[4] = {0,0,0,0};
u32 _FCSR_SramSectorStart[4] = {0,0,0,0};
u32 _FCSR_SramSectorEnd[4] = {0,0,0,0};

/*-----------------------------------------------------------------
_FCSR_isInserted
Is a GBA Flash Cart with a valid file system inserted?
bool return OUT:  true if a GBA FC card is inserted
-----------------------------------------------------------------*/
bool _FCSR_isInserted (void)
{
	bool flagFoundFileSys = false;

	u32* fileSysPointer = (u32*)0x08000100;		// Start at beginning of cart address space, offset by expected location of string

	// Search for file system
	while ((fileSysPointer < (u32*)0x0A000000) && !flagFoundFileSys)	// Only search while not at end of cart address space
	{
		while ((*fileSysPointer != FCSR) && (fileSysPointer < (u32*)0x0A000000))
			fileSysPointer += 0x40;
		if ((strncmp(_FCSR_LabelString, (char*)(fileSysPointer + 1), 12) == 0) && (fileSysPointer < (u32*)0x0A000000))
		{
			flagFoundFileSys = true;
		} else {
			fileSysPointer += 0x80;
		}
	}

	return flagFoundFileSys;
}


/*-----------------------------------------------------------------
_FCSR_clearStatus
Finish any pending operations
bool return OUT:  always true for GBA FC
-----------------------------------------------------------------*/
bool _FCSR_clearStatus (void)
{
	return true;
}


/*-----------------------------------------------------------------
_FCSR_readSectors
Read 512 byte sector numbered "sector" into "buffer"
u32 sector IN: address of first 512 byte sector on Flash Cart to read
u32 numSectors IN: number of 512 byte sectors to read,
 1 to 256 sectors can be read
void* buffer OUT: pointer to 512 byte buffer to store data in
bool return OUT: true if successful
-----------------------------------------------------------------*/
bool _FCSR_readSectors (u32 sector, u32 numSectors, void* buffer)
{
	int i;
	bool flagSramSector = false;
	int readLength = numSectors * BYTES_PER_READ;
	u8* src;
	u8* dst;

	// Find which region this read is in
	for (i = 0; (i < 4) && !flagSramSector; i++)
	{
		if ((sector >= _FCSR_SramSectorStart[i]) && (sector < _FCSR_SramSectorEnd[i]))
		{
			flagSramSector = true;
			break;
		}
	}

	// Make sure read will be completely in SRAM range if it is partially there
	if ( flagSramSector && ((sector + numSectors) > _FCSR_SramSectorEnd[i]))
		return false;

	// Copy data to buffer
	if (flagSramSector)
	{
		src = _FCSR_SramSectorPointer[i] + (sector - _FCSR_SramSectorStart[i]) * BYTES_PER_READ;
	} else {
		src = _FCSR_FileSysPointer + sector * BYTES_PER_READ;
	}
	dst = (u8*)buffer;

	if (flagSramSector)
	{
		while (readLength--)
		{
			*dst++ = *src++;
		}
	} else {	// Reading from Cart ROM

#ifdef _IO_USE_DMA
 #ifdef NDS
  #ifdef ARM9
		DC_FlushRange( buffer, readLength);
  #endif	// ARM9
		DMA3_SRC = (u32)src;
		DMA3_DEST = (u32)buffer;
		DMA3_CR = (readLength >> 1) | DMA_COPY_HALFWORDS;
 #else	// ! NDS
		DMA3COPY ( src, buffer, (readLength >> 1) | DMA16 | DMA_ENABLE);
 #endif	// NDS
#else	// !_IO_USE_DMA
		memcpy (buffer, src, readLength);
#endif	// _IO_USE_DMA

	}	// if (flagSramSector)

	return true;
}

/*-----------------------------------------------------------------
_FCSR_writeSectors
Write 512 byte sector numbered "sector" from "buffer"
u32 sector IN: address of 512 byte sector on Flash Cart to read
u32 numSectors IN: number of 512 byte sectors to read,
 1 to 256 sectors can be read
void* buffer IN: pointer to 512 byte buffer to read data from
bool return OUT: true if successful
-----------------------------------------------------------------*/
bool _FCSR_writeSectors (u32 sector, u8 numSectors, void* buffer)
{
	int i;
	bool flagSramSector = false;
	u32 writeLength = numSectors * BYTES_PER_READ;
	u8* src = (u8*) buffer;
	u8* dst;

	// Find which region this sector belongs in
	for (i = 0; (i < 4) && !flagSramSector; i++)
	{
		if ((sector >= _FCSR_SramSectorStart[i]) && (sector < _FCSR_SramSectorEnd[i]))
		{
			flagSramSector = true;
			break;
		}
	}

	if (!flagSramSector)
		return false;

	// Entire write must be within an SRAM region
	if ((sector + numSectors) > _FCSR_SramSectorEnd[i])
		return false;

	// Copy data to SRAM
	dst = _FCSR_SramSectorPointer[i] + (sector - _FCSR_SramSectorStart[i]) * BYTES_PER_READ;
	while (writeLength--)
	{
		*dst++ = *src++;
	}

	return true;
}

/*-----------------------------------------------------------------
_FCSR_shutdown
unload the Flash Cart interface
-----------------------------------------------------------------*/
bool _FCSR_shutdown(void)
{
	int i;
	if (_FCSR_clearStatus() == false)
		return false;

	_FCSR_FileSysPointer = 0;

	for (i=0; i < 4; i++)
	{
		_FCSR_SramSectorPointer[i] = 0;
		_FCSR_SramSectorStart[i] = 0;
		_FCSR_SramSectorEnd[i] = 0;
	}
	return true;
}

/*-----------------------------------------------------------------
_FCSR_startUp
initializes the Flash Cart interface, returns true if successful,
otherwise returns false
-----------------------------------------------------------------*/
bool _FCSR_startUp(void)
{
	bool flagFoundFileSys = false;
	int i;
	int SramRegionSize[4];
	u8* srcByte;
	u8* destByte;

	u32* fileSysPointer = (u32*)0x08000100;		// Start at beginning of cart address space, offset by expected location of string

	// Search for file system
	while ((fileSysPointer < (u32*)0x0A000000) && !flagFoundFileSys)	// Only search while not at end of cart address space
	{
		while ((*fileSysPointer != FCSR) && (fileSysPointer < (u32*)0x0A000000))
			fileSysPointer += 0x40;
		if ((strncmp(_FCSR_LabelString, (char*)(fileSysPointer + 1), 12) == 0) && (fileSysPointer < (u32*)0x0A000000))
		{
			flagFoundFileSys = true;
		} else {
			fileSysPointer += 0x80;
		}
	}

	if (!flagFoundFileSys)
		return false;

	// Flash cart file system pointer has been found
	_FCSR_FileSysPointer = (u8*)(fileSysPointer - 0x40);

	// Get SRAM sector regions from header block
	for (i = 0; i < 4; i++)
	{
		_FCSR_SramSectorStart[i] = fileSysPointer[i+4];
		SramRegionSize[i] = fileSysPointer[i+8];
		_FCSR_SramSectorEnd[i] = _FCSR_SramSectorStart[i] + SramRegionSize[i];
	}

	// Calculate SRAM region pointers
	_FCSR_SramSectorPointer[0] = (u8*)(SRAM_START + 4);
	for (i = 1; i < 4; i++)
	{
		_FCSR_SramSectorPointer[i] = _FCSR_SramSectorPointer[i-1] + (SramRegionSize[i-1] * BYTES_PER_READ);
	}

	// Initialise SRAM with overlay if it hasn't been done so
	if ( (*((u8*)SRAM_START) != 'F')  || (*((u8*)(SRAM_START+1)) != 'C') || (*((u8*)(SRAM_START+2)) != 'S') || (*((u8*)(SRAM_START+3)) != 'R') )
	{
		*((u8*)SRAM_START) = 'F';
		*((u8*)(SRAM_START+1)) = 'C';
		*((u8*)(SRAM_START+2)) = 'S';
		*((u8*)(SRAM_START+3)) = 'R';

		for (i = 0; i < 4; i++)
		{
			srcByte = _FCSR_FileSysPointer + (_FCSR_SramSectorStart[i] * BYTES_PER_READ);
			destByte = _FCSR_SramSectorPointer[i];
			while (srcByte < _FCSR_FileSysPointer + (_FCSR_SramSectorEnd[i] * BYTES_PER_READ) )
				*destByte++ = *srcByte++;
		}
	}

		// Get SRAM sector regions from header block
	for (i = 0; i < 4; i++)
	{
		if (SramRegionSize[i] == 0)
		{
			_FCSR_SramSectorStart[i] = NO_SRAM;
			_FCSR_SramSectorEnd[i] = NO_SRAM;
		}
	}

	return true;
}

/*-----------------------------------------------------------------
the actual interface structure
-----------------------------------------------------------------*/
const IO_INTERFACE _io_fcsr = {
	DEVICE_TYPE_FCSR,	// 'FCSR'
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_GBA,
	(FN_MEDIUM_STARTUP)&_FCSR_startUp,
	(FN_MEDIUM_ISINSERTED)&_FCSR_isInserted,
	(FN_MEDIUM_READSECTORS)&_FCSR_readSectors,
	(FN_MEDIUM_WRITESECTORS)&_FCSR_writeSectors,
	(FN_MEDIUM_CLEARSTATUS)&_FCSR_clearStatus,
	(FN_MEDIUM_SHUTDOWN)&_FCSR_shutdown
} ;
