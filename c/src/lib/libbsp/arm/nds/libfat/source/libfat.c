/*
	libfat.c
	Simple functionality for startup, mounting and unmounting of FAT-based devices.

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

	2006-08-13 - Chishm
		* Moved all externally visible directory related functions to fatdir

	2006-08-14 - Chishm
		* Added extended devoptab_t functions

	2007-01-10 - Chishm
		* fatInit now sets the correct path when setAsDefaultDevice

	2007-01-11 - Chishm
		* Added missing #include <unistd.h>
*/

#include <sys/iosupport.h>
#include <unistd.h>

#include "common.h"
#include "partition.h"
#include "fatfile.h"
#include "fatdir.h"

#define GBA_DEFAULT_CACHE_PAGES 2
#define NDS_DEFAULT_CACHE_PAGES 8


const devoptab_t dotab_fat = {
	"fat",
	sizeof (FILE_STRUCT),
	_FAT_open_r,
	_FAT_close_r,
	_FAT_write_r,
	_FAT_read_r,
	_FAT_seek_r,
	_FAT_fstat_r,
	_FAT_stat_r,
	_FAT_link_r,
	_FAT_unlink_r,
	_FAT_chdir_r,
	_FAT_rename_r,
	_FAT_mkdir_r,
	sizeof (DIR_STATE_STRUCT),
	_FAT_diropen_r,
	_FAT_dirreset_r,
	_FAT_dirnext_r,
	_FAT_dirclose_r
};

bool fatInit (u32 cacheSize, bool setAsDefaultDevice) {
#ifdef NDS
	bool slot1Device, slot2Device;

	// Try mounting both slots
	slot1Device = _FAT_partition_mount (PI_SLOT_1, cacheSize);
	slot2Device = _FAT_partition_mount (PI_SLOT_2, cacheSize);

	// Choose the default device
	if (slot1Device) {
		_FAT_partition_setDefaultInterface (PI_SLOT_1);
	} else if (slot2Device) {
		_FAT_partition_setDefaultInterface (PI_SLOT_2);
	} else {
		return false;
	}

#else	// not defined NDS
	bool cartSlotDevice;

	cartSlotDevice = _FAT_partition_mount (PI_CART_SLOT, cacheSize);

	if (cartSlotDevice) {
		_FAT_partition_setDefaultInterface (PI_CART_SLOT);
	} else {
		return false;
	}

#endif	// defined NDS

	AddDevice (&dotab_fat);

	if (setAsDefaultDevice) {
		chdir ("fat:/");
	}

	return true;
}

bool fatInitDefault (void) {
#ifdef NDS
	return fatInit (NDS_DEFAULT_CACHE_PAGES, true);
#else
	return fatInit (GBA_DEFAULT_CACHE_PAGES, true);
#endif
}

bool fatMountNormalInterface (PARTITION_INTERFACE partitionNumber, u32 cacheSize) {
	return _FAT_partition_mount (partitionNumber, cacheSize);
}

bool fatMountCustomInterface (const IO_INTERFACE* device, u32 cacheSize) {
	return _FAT_partition_mountCustomInterface (device, cacheSize);
}

bool fatUnmount (PARTITION_INTERFACE partitionNumber) {
	return _FAT_partition_unmount (partitionNumber);
}


bool fatUnsafeUnmount (PARTITION_INTERFACE partitionNumber) {
	return _FAT_partition_unsafeUnmount (partitionNumber);
}

bool fatSetDefaultInterface (PARTITION_INTERFACE partitionNumber) {
	return _FAT_partition_setDefaultInterface (partitionNumber);
}
