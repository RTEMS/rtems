/*

	disc.c

	uniformed io-interface to work with Chishm's FAT library

	Written by MightyMax


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

	2005-11-06 - Chishm
		* Added WAIT_CR modifications for NDS

	2006-02-03 www.neoflash.com
		* Added SUPPORT_* defines, comment out any of the SUPPORT_* defines in disc_io.h to remove support
		  for the given interface and stop code being linked to the binary

	    * Added support for MK2 MMC interface

		* Added disc_Cache* functions

	2006-02-05 - Chishm
		* Added Supercard SD support

	2006-02-26 - Cytex
		* Added EFA2 support

	2006-05-18 - Chishm
		* Rewritten for FATlib disc.c

	2006-06-19 - Chishm
		* Changed read and write interface to accept a u32 instead of a u8 for the number of sectors

	2006-07-11 - Chishm
		* Removed disc_Cache* functions, since there is now a proper unified cache
		* Removed SUPPORT_* defines
		* Rewrote device detection functions
		* First libfat release

	2006-07-25 - Chishm
		* Changed IO_INTERFACEs to const

	2006-08-02 - Chishm
		* Added NinjaDS

	2006-12-25 - Chishm
		* Added DLDI
		* Removed experimental interfaces

	2007-05-01 - Chishm
		* Removed FCSR
*/

#include "disc.h"
#include "disc_io.h"

#ifdef NDS
 #include <nds.h>
#endif


// Include known io-interfaces:
#include "io_dldi.h"
#include "io_njsd.h"
#include "io_nmmc.h"
#include "io_mpcf.h"
#include "io_m3cf.h"
#include "io_sccf.h"
#include "io_scsd.h"
#include "io_m3sd.h"

const IO_INTERFACE* ioInterfaces[] = {
	&_io_dldi,		// Reserved for new interfaces
#ifdef NDS
	// Place Slot 1 (DS Card) interfaces here
	&_io_njsd, &_io_nmmc,
#endif
	// Place Slot 2 (GBA Cart) interfaces here
	&_io_mpcf, &_io_m3cf, &_io_sccf, &_io_scsd, &_io_m3sd
};

/*

	Hardware level disc funtions

*/

const IO_INTERFACE* _FAT_disc_gbaSlotFindInterface (void)
{
	// If running on an NDS, make sure the correct CPU can access
	// the GBA cart. First implemented by SaTa.
#ifdef NDS
 #ifdef ARM9
	sysSetCartOwner(BUS_OWNER_ARM9);
 #endif
#endif

	int i;

	for (i = 0; i < (sizeof(ioInterfaces) / sizeof(IO_INTERFACE*)); i++) {
		if ((ioInterfaces[i]->features & FEATURE_SLOT_GBA) && (ioInterfaces[i]->fn_startup())) {
			return ioInterfaces[i];
		}
	}
	return NULL;
}

#ifdef NDS
/*
 * Check the DS card slot for a valid memory card interface
 * If an interface is found, it is set as the default interace
 * and it returns true. Otherwise the default interface is left
 * untouched and it returns false.
 */
const IO_INTERFACE* _FAT_disc_dsSlotFindInterface (void)
{
#ifdef ARM9
	sysSetCardOwner(BUS_OWNER_ARM9);
#endif
	int i;

	for (i = 0; i < (sizeof(ioInterfaces) / sizeof(IO_INTERFACE*)); i++) {
		if ((ioInterfaces[i]->features & FEATURE_SLOT_NDS) && (ioInterfaces[i]->fn_startup())) {
			return ioInterfaces[i];
		}
	}

	return NULL;
}
#endif

/*
 * When running on an NDS, check the either slot for a valid memory
 * card interface.
 * When running on a GBA, call _FAT_disc_gbaSlotFindInterface
 * If an interface is found, it is set as the default interace
 * and it returns true. Otherwise the default interface is left
 * untouched and it returns false.
 */
#ifdef NDS
const IO_INTERFACE* _FAT_disc_findInterface (void)
{
#ifdef ARM9
	sysSetBusOwners(BUS_OWNER_ARM9, BUS_OWNER_ARM9);
#endif

	int i;

	for (i = 0; i < (sizeof(ioInterfaces) / sizeof(IO_INTERFACE*)); i++) {
		if (ioInterfaces[i]->fn_startup()) {
			return ioInterfaces[i];
		}
	}

	return NULL;
}
#else
const IO_INTERFACE* _FAT_disc_findInterface (void)
{
	return _FAT_disc_gbaSlotFindInterface();
}
#endif
