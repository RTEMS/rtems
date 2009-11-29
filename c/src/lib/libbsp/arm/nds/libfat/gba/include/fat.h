/*
	fat.h
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

	2006-07-14
		* fatInitialise renamed to fatInit

	2006-07-16 - Chishm
		* Added fatInitDefault
*/


#ifndef _LIBFAT_H
#define _LIBFAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gba_types.h"

typedef enum {PI_CART_SLOT} PARTITION_INTERFACE;

struct IO_INTERFACE_STRUCT ;

/*
Initialise any inserted block-devices.
Add the fat device driver to the devoptab, making it available for standard file functions.
cacheSize: The number of pages to allocate for each inserted block-device
setAsDefaultDevice: if true, make this the default device driver for file operations
*/
bool fatInit (u32 cacheSize, bool setAsDefaultDevice);

/*
Calls fatInit with setAsDefaultDevice = true and cacheSize optimised for the host system.
*/
bool fatInitDefault (void);

/*
Mount the device specified by partitionNumber
PD_DEFAULT is not allowed, use _FAT_partition_setDefaultDevice
PD_CUSTOM is not allowed, use _FAT_partition_mountCustomDevice
*/
bool fatMountNormalInterface (PARTITION_INTERFACE partitionNumber, u32 cacheSize);

/*
Mount a partition on a custom device
*/
bool fatMountCustomInterface (struct IO_INTERFACE_STRUCT* device, u32 cacheSize);

/*
Unmount the partition specified by partitionNumber
If there are open files, it will fail
*/
bool fatUnmount (PARTITION_INTERFACE partitionNumber);


/*
Forcibly unmount the partition specified by partitionNumber
Any open files on the partition will become invalid
The cache will be invalidated, and any unflushed writes will be lost
*/
bool fatUnsafeUnmount (PARTITION_INTERFACE partitionNumber);

/*
Set the default device for access by fat: and fat0:
PD_DEFAULT is unallowed.
Doesn't do anything useful on GBA, since there is only one device
*/
bool fatSetDefaultInterface (PARTITION_INTERFACE partitionNumber);

#ifdef __cplusplus
}
#endif

#endif // _LIBFAT_H
