/*
 io_cf_common.h

 By chishm (Michael Chisholm)

 Common Compact Flash card values

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

	2006-07-11 - Chishm
		* Original release

	2006-07-16 - Chishm
		* Combined all CF interfaces into one common set of routines
*/

#ifndef IO_CF_COMMON_H
#define IO_CF_COMMON_H

#include "disc_io.h"

typedef struct {
	vu16* data;
	vu16* status;
	vu16* command;
	vu16* error;
	vu16* sectorCount;
	vu16* lba1;
	vu16* lba2;
	vu16* lba3;
	vu16* lba4;
} CF_REGISTERS;


// CF Card status
#define CF_STS_INSERTED		0x50
#define CF_STS_REMOVED		0x00
#define CF_STS_READY		0x58

#define CF_STS_DRQ			0x08
#define CF_STS_BUSY			0x80

// CF Card commands
#define CF_CMD_LBA			0xE0
#define CF_CMD_READ			0x20
#define CF_CMD_WRITE		0x30

#define CF_CARD_TIMEOUT	10000000

bool _CF_isInserted (void);
bool _CF_clearStatus (void);
bool _CF_readSectors (u32 sector, u32 numSectors, void* buffer);
bool _CF_writeSectors (u32 sector, u32 numSectors, void* buffer);
bool _CF_shutdown(void);
bool _CF_startup(const CF_REGISTERS *usableCfRegs);

#endif // define IO_CF_COMMON_H
