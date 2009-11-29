/*
 partition.c
 Functions for mounting and dismounting partitions
 on various block devices.

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

	2006-08-10 - Chishm
		* Fixed problem when openning files starting with "fat"

	2006-10-28 - Chishm
		* _partitions changed to _FAT_partitions to maintain the same style of naming as the functions
*/


#include "partition.h"
#include "bit_ops.h"
#include "file_allocation_table.h"
#include "directory.h"

#include <string.h>
#include <ctype.h>

#include "mem_allocate.h"

/*
This device name, as known by DevKitPro
*/
const char* DEVICE_NAME = "fat";

/*
Data offsets
*/

// BIOS Parameter Block offsets
enum BPB {
	BPB_jmpBoot = 0x00,
	BPB_OEMName = 0x03,
	// BIOS Parameter Block
	BPB_bytesPerSector = 0x0B,
	BPB_sectorsPerCluster = 0x0D,
	BPB_reservedSectors = 0x0E,
	BPB_numFATs = 0x10,
	BPB_rootEntries = 0x11,
	BPB_numSectorsSmall = 0x13,
	BPB_mediaDesc = 0x15,
	BPB_sectorsPerFAT = 0x16,
	BPB_sectorsPerTrk = 0x18,
	BPB_numHeads = 0x1A,
	BPB_numHiddenSectors = 0x1C,
	BPB_numSectors = 0x20,
	// Ext BIOS Parameter Block for FAT16
	BPB_FAT16_driveNumber = 0x24,
	BPB_FAT16_reserved1 = 0x25,
	BPB_FAT16_extBootSig = 0x26,
	BPB_FAT16_volumeID = 0x27,
	BPB_FAT16_volumeLabel = 0x2B,
	BPB_FAT16_fileSysType = 0x36,
	// Bootcode
	BPB_FAT16_bootCode = 0x3E,
	// FAT32 extended block
	BPB_FAT32_sectorsPerFAT32 = 0x24,
	BPB_FAT32_extFlags = 0x28,
	BPB_FAT32_fsVer = 0x2A,
	BPB_FAT32_rootClus = 0x2C,
	BPB_FAT32_fsInfo = 0x30,
	BPB_FAT32_bkBootSec = 0x32,
	// Ext BIOS Parameter Block for FAT32
	BPB_FAT32_driveNumber = 0x40,
	BPB_FAT32_reserved1 = 0x41,
	BPB_FAT32_extBootSig = 0x42,
	BPB_FAT32_volumeID = 0x43,
	BPB_FAT32_volumeLabel = 0x47,
	BPB_FAT32_fileSysType = 0x52,
	// Bootcode
	BPB_FAT32_bootCode = 0x5A,
	BPB_bootSig_55 = 0x1FE,
	BPB_bootSig_AA = 0x1FF
};


#ifdef NDS
#define MAXIMUM_PARTITIONS 4
PARTITION* _FAT_partitions[MAXIMUM_PARTITIONS] = {NULL};
#else // not defined NDS
#define MAXIMUM_PARTITIONS 1
PARTITION* _FAT_partitions[MAXIMUM_PARTITIONS] = {NULL};
#endif // defined NDS

// Use a single static buffer for the partitions


static PARTITION* _FAT_partition_constructor ( const IO_INTERFACE* disc, u32 cacheSize) {
	PARTITION* partition;
	int i;
	u32 bootSector;
	u8 sectorBuffer[BYTES_PER_READ] = {0};

	// Read first sector of disc
	if ( !_FAT_disc_readSectors (disc, 0, 1, sectorBuffer)) {
		return NULL;
	}

	// Make sure it is a valid MBR or boot sector
	if ( (sectorBuffer[BPB_bootSig_55] != 0x55) || (sectorBuffer[BPB_bootSig_AA] != 0xAA)) {
		return NULL;
	}

	// Check if there is a FAT string, which indicates this is a boot sector
	if ((sectorBuffer[0x36] == 'F') && (sectorBuffer[0x37] == 'A') && (sectorBuffer[0x38] == 'T')) {
		bootSector = 0;
	} else if ((sectorBuffer[0x52] == 'F') && (sectorBuffer[0x53] == 'A') && (sectorBuffer[0x54] == 'T')) {
		// Check for FAT32
		bootSector = 0;
	} else {
		// This is an MBR
		// Find first valid partition from MBR
		// First check for an active partition
		for (i=0x1BE; (i < 0x1FE) && (sectorBuffer[i] != 0x80); i+= 0x10);
		// If it didn't find an active partition, search for any valid partition
		if (i == 0x1FE) {
			for (i=0x1BE; (i < 0x1FE) && (sectorBuffer[i+0x04] == 0x00); i+= 0x10);
		}

		// Go to first valid partition
		if ( i != 0x1FE) {
			// Make sure it found a partition
			bootSector = u8array_to_u32(sectorBuffer, 0x8 + i);
		} else {
			bootSector = 0;	// No partition found, assume this is a MBR free disk
		}
	}

	// Read in boot sector
	if ( !_FAT_disc_readSectors (disc, bootSector, 1, sectorBuffer)) {
		return NULL;
	}

	partition = (PARTITION*) _FAT_mem_allocate (sizeof(PARTITION));
	if (partition == NULL) {
		return NULL;
	}

	// Set partition's disc interface
	partition->disc = disc;

	// Store required information about the file system
	partition->fat.sectorsPerFat = u8array_to_u16(sectorBuffer, BPB_sectorsPerFAT);
	if (partition->fat.sectorsPerFat == 0) {
		partition->fat.sectorsPerFat = u8array_to_u32( sectorBuffer, BPB_FAT32_sectorsPerFAT32);
	}

	partition->numberOfSectors = u8array_to_u16( sectorBuffer, BPB_numSectorsSmall);
	if (partition->numberOfSectors == 0) {
		partition->numberOfSectors = u8array_to_u32( sectorBuffer, BPB_numSectors);
	}

	partition->bytesPerSector = BYTES_PER_READ;	// Sector size is redefined to be 512 bytes
	partition->sectorsPerCluster = sectorBuffer[BPB_sectorsPerCluster] * u8array_to_u16(sectorBuffer, BPB_bytesPerSector) / BYTES_PER_READ;
	partition->bytesPerCluster = partition->bytesPerSector * partition->sectorsPerCluster;
	partition->fat.fatStart = bootSector + u8array_to_u16(sectorBuffer, BPB_reservedSectors);

	partition->rootDirStart = partition->fat.fatStart + (sectorBuffer[BPB_numFATs] * partition->fat.sectorsPerFat);
	partition->dataStart = partition->rootDirStart + (( u8array_to_u16(sectorBuffer, BPB_rootEntries) * DIR_ENTRY_DATA_SIZE) / partition->bytesPerSector);

	partition->totalSize = (partition->numberOfSectors - partition->dataStart) * partition->bytesPerSector;

	// Store info about FAT
	partition->fat.lastCluster = (partition->numberOfSectors - partition->dataStart) / partition->sectorsPerCluster;
	partition->fat.firstFree = CLUSTER_FIRST;

	if (partition->fat.lastCluster < CLUSTERS_PER_FAT12) {
		partition->filesysType = FS_FAT12;	// FAT12 volume
	} else if (partition->fat.lastCluster < CLUSTERS_PER_FAT16) {
		partition->filesysType = FS_FAT16;	// FAT16 volume
	} else {
		partition->filesysType = FS_FAT32;	// FAT32 volume
	}

	if (partition->filesysType != FS_FAT32) {
		partition->rootDirCluster = FAT16_ROOT_DIR_CLUSTER;
	} else {
		// Set up for the FAT32 way
		partition->rootDirCluster = u8array_to_u32(sectorBuffer, BPB_FAT32_rootClus);
		// Check if FAT mirroring is enabled
		if (!(sectorBuffer[BPB_FAT32_extFlags] & 0x80)) {
			// Use the active FAT
			partition->fat.fatStart = partition->fat.fatStart + ( partition->fat.sectorsPerFat * (sectorBuffer[BPB_FAT32_extFlags] & 0x0F));
		}
	}

	// Create a cache to use
	partition->cache = _FAT_cache_constructor (cacheSize, partition->disc);

	// Set current directory to the root
	partition->cwdCluster = partition->rootDirCluster;

	// Check if this disc is writable, and set the readOnly property appropriately
	partition->readOnly = !(_FAT_disc_features(disc) & FEATURE_MEDIUM_CANWRITE);

	// There are currently no open files on this partition
	partition->openFileCount = 0;

	return partition;
}

static void _FAT_partition_destructor (PARTITION* partition) {
	_FAT_cache_destructor (partition->cache);
	_FAT_disc_shutdown (partition->disc);
	_FAT_mem_free (partition);
}

bool _FAT_partition_mount (PARTITION_INTERFACE partitionNumber, u32 cacheSize) {
#ifdef NDS
	int i;
	const IO_INTERFACE* disc = NULL;

	if (_FAT_partitions[partitionNumber] != NULL) {
		return false;
	}

	switch (partitionNumber) {
		case PI_SLOT_1:
			// Mount the disc in slot 1
			disc = _FAT_disc_dsSlotFindInterface ();
			break;
		case PI_SLOT_2:
			// Mount the disc in slot 2
			disc = _FAT_disc_gbaSlotFindInterface ();
			break;
		case PI_DEFAULT:
		case PI_CUSTOM:
		default:
			// Anything else has to be handled specially
			return false;
			break;
	}

	if (disc == NULL) {
		return false;
	}

	// See if that disc is already in use, if so, then just copy the partition pointer
	for (i = 0; i < MAXIMUM_PARTITIONS; i++) {
		if ((_FAT_partitions[i] != NULL) && (_FAT_partitions[i]->disc == disc)) {
			_FAT_partitions[partitionNumber] = _FAT_partitions[i];
			return true;
		}
	}

	_FAT_partitions[partitionNumber] = _FAT_partition_constructor (disc, cacheSize);

	if (_FAT_partitions[partitionNumber] == NULL) {
		return false;
	}

#else // not defined NDS
	const IO_INTERFACE* disc = NULL;

	if (_FAT_partitions[partitionNumber] != NULL) {
		return false;
	}

	// Only ever one partition on GBA
	disc = _FAT_disc_gbaSlotFindInterface ();
	_FAT_partitions[partitionNumber] = _FAT_partition_constructor (disc, cacheSize);

#endif // defined NDS

	return true;
}

bool _FAT_partition_mountCustomInterface (const IO_INTERFACE* device, u32 cacheSize) {
#ifdef NDS
	int i;

	if (_FAT_partitions[PI_CUSTOM] != NULL) {
		return false;
	}

	if (device == NULL) {
		return false;
	}

	// See if that disc is already in use, if so, then just copy the partition pointer
	for (i = 0; i < MAXIMUM_PARTITIONS; i++) {
		if ((_FAT_partitions[i] != NULL) && (_FAT_partitions[i]->disc == device)) {
			_FAT_partitions[PI_CUSTOM] = _FAT_partitions[i];
			return true;
		}
	}

	_FAT_partitions[PI_CUSTOM] = _FAT_partition_constructor (device, cacheSize);

	if (_FAT_partitions[PI_CUSTOM] == NULL) {
		return false;
	}

#else // not defined NDS
	if (_FAT_partitions[PI_CART_SLOT] != NULL) {
		return false;
	}

	if (device == NULL) {
		return false;
	}

	// Only ever one partition on GBA
	_FAT_partitions[PI_CART_SLOT] = _FAT_partition_constructor (device, cacheSize);

#endif // defined NDS

	return true;
}

bool _FAT_partition_setDefaultInterface (PARTITION_INTERFACE partitionNumber) {
#ifdef NDS	// Can only set the default partition when there is more than 1, so doesn't apply to GBA
	if ((partitionNumber < 1) || (partitionNumber >= MAXIMUM_PARTITIONS)) {
		return false;
	}

	if (_FAT_partitions[partitionNumber] == NULL) {
		return false;
	}

	_FAT_partitions[PI_DEFAULT] = _FAT_partitions[partitionNumber];
#endif
	return true;
}

bool _FAT_partition_setDefaultPartition (PARTITION* partition) {
#ifdef NDS	// Can only set the default partition when there is more than 1, so doesn't apply to GBA
	int i;

	if (partition == NULL) {
		return false;
	}

	// Ensure that this device is already in the list
	for (i = 1; i < MAXIMUM_PARTITIONS; i++) {
		if (_FAT_partitions[i] == partition) {
			break;
		}
	}

	// It wasn't in the list, so fail
	if (i == MAXIMUM_PARTITIONS) {
		return false;
	}

	// Change the default partition / device to this one
	_FAT_partitions[PI_DEFAULT] = partition;

#endif
	return true;
}

bool _FAT_partition_unmount (PARTITION_INTERFACE partitionNumber) {
	int i;
	PARTITION* partition = _FAT_partitions[partitionNumber];

	if (partition == NULL) {
		return false;
	}

	if (partition->openFileCount > 0) {
		// There are still open files that need closing
		return false;
	}

	// Remove all references to this partition
	for (i = 0; i < MAXIMUM_PARTITIONS; i++) {
		if (_FAT_partitions[i] == partition) {
			_FAT_partitions[i] = NULL;
		}
	}

	_FAT_partition_destructor (partition);
	return true;
}

bool _FAT_partition_unsafeUnmount (PARTITION_INTERFACE partitionNumber) {
	int i;
	PARTITION* partition = _FAT_partitions[partitionNumber];

	if (partition == NULL) {
		return false;
	}

	// Remove all references to this partition
	for (i = 0; i < MAXIMUM_PARTITIONS; i++) {
		if (_FAT_partitions[i] == partition) {
			_FAT_partitions[i] = NULL;
		}
	}

	_FAT_cache_invalidate (partition->cache);
	_FAT_partition_destructor (partition);
	return true;
}

PARTITION* _FAT_partition_getPartitionFromPath (const char* path) {
#ifdef NDS
	int namelen;
	int partitionNumber;

	// Device name extraction code taken from DevKitPro
	namelen = strlen(DEVICE_NAME);
	if (strchr (path, ':') == NULL) {
		// No device specified
		partitionNumber = PI_DEFAULT;
	} else if( strncmp(DEVICE_NAME, path, namelen) == 0 ) {
		if ( path[namelen] == ':' ) {
			// Only the device name is specified
			partitionNumber = PI_DEFAULT;
		} else if (isdigit(path[namelen]) && path[namelen+1] ==':' ) {
			// Device name and number specified
			partitionNumber = path[namelen] - '0';
		} else {
			// Incorrect device name
			return NULL;
		}
	} else {
		// Incorrect device name
		return NULL;
	}

	if ((partitionNumber < 0) || (partitionNumber >= MAXIMUM_PARTITIONS)) {
		return NULL;
	}

	return _FAT_partitions[partitionNumber];
#else // not defined NDS
	// Only one possible partition on GBA
	return _FAT_partitions[PI_CART_SLOT];
#endif // defined NDS
}
