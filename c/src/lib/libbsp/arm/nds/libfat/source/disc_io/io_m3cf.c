/*
	io_m3cf.c based on

	compact_flash.c
	By chishm (Michael Chisholm)

	Hardware Routines for reading a compact flash card
	using the M3 Perfect CF Adapter

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


#include "io_m3cf.h"
#include "io_m3_common.h"
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
// M3 CF Addresses
#define REG_M3CF_STS		((vu16*)0x080C0000)	// Status of the CF Card / Device control
#define REG_M3CF_CMD		((vu16*)0x088E0000)	// Commands sent to control chip and status return
#define REG_M3CF_ERR		((vu16*)0x08820000)	// Errors / Features

#define REG_M3CF_SEC		((vu16*)0x08840000)	// Number of sector to transfer
#define REG_M3CF_LBA1		((vu16*)0x08860000)	// 1st byte of sector address
#define REG_M3CF_LBA2		((vu16*)0x08880000)	// 2nd byte of sector address
#define REG_M3CF_LBA3		((vu16*)0x088A0000)	// 3rd byte of sector address
#define REG_M3CF_LBA4		((vu16*)0x088C0000)	// last nibble of sector address | 0xE0

#define REG_M3CF_DATA		((vu16*)0x08800000)		// Pointer to buffer of CF data transered from card

static const CF_REGISTERS _M3CF_Registers = {
	REG_M3CF_DATA,
	REG_M3CF_STS,
	REG_M3CF_CMD,
	REG_M3CF_ERR,
	REG_M3CF_SEC,
	REG_M3CF_LBA1,
	REG_M3CF_LBA2,
	REG_M3CF_LBA3,
	REG_M3CF_LBA4
};


bool _M3CF_startup(void) {
	_M3_changeMode (M3_MODE_MEDIA);
	return _CF_startup (&_M3CF_Registers);
}


const IO_INTERFACE _io_m3cf = {
	DEVICE_TYPE_M3CF,
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_GBA,
	(FN_MEDIUM_STARTUP)&_M3CF_startup,
	(FN_MEDIUM_ISINSERTED)&_CF_isInserted,
	(FN_MEDIUM_READSECTORS)&_CF_readSectors,
	(FN_MEDIUM_WRITESECTORS)&_CF_writeSectors,
	(FN_MEDIUM_CLEARSTATUS)&_CF_clearStatus,
	(FN_MEDIUM_SHUTDOWN)&_CF_shutdown
} ;
