/*
	io_sccf.c based on

	compact_flash.c
	By chishm (Michael Chisholm)

	Hardware Routines for reading a compact flash card
	using the Super Card CF

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


#include "io_sccf.h"
#include "io_sc_common.h"
#include "io_cf_common.h"

//---------------------------------------------------------------
// SC CF Addresses
#define REG_SCCF_STS		((vu16*)0x098C0000)	// Status of the CF Card / Device control
#define REG_SCCF_CMD		((vu16*)0x090E0000)	// Commands sent to control chip and status return
#define REG_SCCF_ERR		((vu16*)0x09020000)	// Errors / Features

#define REG_SCCF_SEC		((vu16*)0x09040000)	// Number of sector to transfer
#define REG_SCCF_LBA1		((vu16*)0x09060000)	// 1st byte of sector address
#define REG_SCCF_LBA2		((vu16*)0x09080000)	// 2nd byte of sector address
#define REG_SCCF_LBA3		((vu16*)0x090A0000)	// 3rd byte of sector address
#define REG_SCCF_LBA4		((vu16*)0x090C0000)	// last nibble of sector address | 0xE0

#define REG_SCCF_DATA		((vu16*)0x09000000)	// Pointer to buffer of CF data transered from card

static const CF_REGISTERS _SCCF_Registers = {
	REG_SCCF_DATA,
	REG_SCCF_STS,
	REG_SCCF_CMD,
	REG_SCCF_ERR,
	REG_SCCF_SEC,
	REG_SCCF_LBA1,
	REG_SCCF_LBA2,
	REG_SCCF_LBA3,
	REG_SCCF_LBA4
};


bool _SCCF_startup(void) {
	_SC_changeMode (SC_MODE_MEDIA);
	return _CF_startup(&_SCCF_Registers);
}


const IO_INTERFACE _io_sccf = {
	DEVICE_TYPE_SCCF,
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_GBA,
	(FN_MEDIUM_STARTUP)&_SCCF_startup,
	(FN_MEDIUM_ISINSERTED)&_CF_isInserted,
	(FN_MEDIUM_READSECTORS)&_CF_readSectors,
	(FN_MEDIUM_WRITESECTORS)&_CF_writeSectors,
	(FN_MEDIUM_CLEARSTATUS)&_CF_clearStatus,
	(FN_MEDIUM_SHUTDOWN)&_CF_shutdown
} ;
