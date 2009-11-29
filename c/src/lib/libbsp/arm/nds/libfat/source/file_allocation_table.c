/*
 file_allocation_table.c
 Reading, writing and manipulation of the FAT structure on
 a FAT partition

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

	2006-07-11 - Chishm
		* Made several fixes related to free clusters, thanks to Loopy

	2006-10-01 - Chishm
		* Added _FAT_fat_linkFreeClusterCleared to clear a cluster when it is allocated
*/


#include "file_allocation_table.h"
#include "partition.h"
#include <string.h>

/*
Gets the cluster linked from input cluster
*/
u32 _FAT_fat_nextCluster(PARTITION* partition, u32 cluster)
{
	u32 nextCluster = CLUSTER_FREE;
	u32 sector;
	int offset;

	switch (partition->filesysType)
	{
		case FS_UNKNOWN:
			nextCluster = CLUSTER_FREE;
			break;

		case FS_FAT12:
			sector = partition->fat.fatStart + (((cluster * 3) / 2) / BYTES_PER_READ);
			offset = ((cluster * 3) / 2) % BYTES_PER_READ;


			_FAT_cache_readPartialSector (partition->cache, &nextCluster, sector, offset, sizeof(u8));

			offset++;

			if (offset >= BYTES_PER_READ) {
				offset = 0;
				sector++;
			}

			_FAT_cache_readPartialSector (partition->cache, ((u8*)&nextCluster) + sizeof(u8), sector, offset, sizeof(u8));

			if (cluster & 0x01) {
				nextCluster = nextCluster >> 4;
			} else 	{
				nextCluster &= 0x0FFF;
			}

			if (nextCluster >= 0x0FF7)
			{
				nextCluster = CLUSTER_EOF;
			}

			break;

		case FS_FAT16:
			sector = partition->fat.fatStart + ((cluster << 1) / BYTES_PER_READ);
			offset = (cluster % (BYTES_PER_READ >> 1)) << 1;

			_FAT_cache_readPartialSector (partition->cache, &nextCluster, sector, offset, sizeof(u16));

			if (nextCluster >= 0xFFF7)
			{
				nextCluster = CLUSTER_EOF;
			}
			break;

		case FS_FAT32:
			sector = partition->fat.fatStart + ((cluster << 2) / BYTES_PER_READ);
			offset = (cluster % (BYTES_PER_READ >> 2)) << 2;

			_FAT_cache_readPartialSector (partition->cache, &nextCluster, sector, offset, sizeof(u32));

			if (nextCluster >= 0x0FFFFFF7)
			{
				nextCluster = CLUSTER_EOF;
			}
			break;

		default:
			nextCluster = CLUSTER_FREE;
			break;
	}

	return nextCluster;
}

/*
writes value into the correct offset within a partition's FAT, based
on the cluster number.
*/
static bool _FAT_fat_writeFatEntry (PARTITION* partition, u32 cluster, u32 value) {
	u32 sector;
	int offset;
	u8 oldValue;

	if ((cluster < 0x0002) || (cluster > partition->fat.lastCluster))
	{
		return false;
	}

	switch (partition->filesysType)
	{
		case FS_UNKNOWN:
			return false;
			break;

		case FS_FAT12:
			sector = partition->fat.fatStart + (((cluster * 3) / 2) / BYTES_PER_READ);
			offset = ((cluster * 3) / 2) % BYTES_PER_READ;

			if (cluster & 0x01) {

				_FAT_cache_readPartialSector (partition->cache, &oldValue, sector, offset, sizeof(u8));

				value = (value << 4) | (oldValue & 0x0F);

				_FAT_cache_writePartialSector (partition->cache, &value, sector, offset, sizeof(u8));

				offset++;
				if (offset >= BYTES_PER_READ) {
					offset = 0;
					sector++;
				}

				_FAT_cache_writePartialSector (partition->cache, ((u8*)&value) + sizeof(u8), sector, offset, sizeof(u8));

			} else {

				_FAT_cache_writePartialSector (partition->cache, &value, sector, offset, sizeof(u8));

				offset++;
				if (offset >= BYTES_PER_READ) {
					offset = 0;
					sector++;
				}

				_FAT_cache_readPartialSector (partition->cache, &oldValue, sector, offset, sizeof(u8));

				value = ((value >> 8) & 0x0F) | (oldValue & 0xF0);

				_FAT_cache_writePartialSector (partition->cache, &value, sector, offset, sizeof(u8));
			}

			break;

		case FS_FAT16:
			sector = partition->fat.fatStart + ((cluster << 1) / BYTES_PER_READ);
			offset = (cluster % (BYTES_PER_READ >> 1)) << 1;

			_FAT_cache_writePartialSector (partition->cache, &value, sector, offset, sizeof(u16));

			break;

		case FS_FAT32:
			sector = partition->fat.fatStart + ((cluster << 2) / BYTES_PER_READ);
			offset = (cluster % (BYTES_PER_READ >> 2)) << 2;

			_FAT_cache_writePartialSector (partition->cache, &value, sector, offset, sizeof(u32));

			break;

		default:
			return false;
			break;
	}

	return true;
}

/*-----------------------------------------------------------------
gets the first available free cluster, sets it
to end of file, links the input cluster to it then returns the
cluster number
If an error occurs, return CLUSTER_FREE
-----------------------------------------------------------------*/
u32 _FAT_fat_linkFreeCluster(PARTITION* partition, u32 cluster) {
	u32 firstFree;
	u32 curLink;
	u32 lastCluster;
	bool loopedAroundFAT = false;

	lastCluster =  partition->fat.lastCluster;

	if (cluster > lastCluster) {
		return CLUSTER_FREE;
	}

	// Check if the cluster already has a link, and return it if so
	curLink = _FAT_fat_nextCluster(partition, cluster);
	if ((curLink >= CLUSTER_FIRST) && (curLink <= lastCluster)) {
		return curLink;	// Return the current link - don't allocate a new one
	}

	// Get a free cluster
	firstFree = partition->fat.firstFree;
	// Start at first valid cluster
	if (firstFree < CLUSTER_FIRST) {
		firstFree = CLUSTER_FIRST;
	}

	// Search until a free cluster is found
	while (_FAT_fat_nextCluster(partition, firstFree) != CLUSTER_FREE) {
		firstFree++;
		if (firstFree > lastCluster) {
			if (loopedAroundFAT) {
				// If couldn't get a free cluster then return, saying this fact
				partition->fat.firstFree = firstFree;
				return CLUSTER_FREE;
			} else {
				// Try looping back to the beginning of the FAT
				// This was suggested by loopy
				firstFree = CLUSTER_FIRST;
				loopedAroundFAT = true;
			}
		}
	}
	partition->fat.firstFree = firstFree;

	if ((cluster >= CLUSTER_FIRST) && (cluster < lastCluster))
	{
		// Update the linked from FAT entry
		_FAT_fat_writeFatEntry (partition, cluster, firstFree);
	}
	// Create the linked to FAT entry
	_FAT_fat_writeFatEntry (partition, firstFree, CLUSTER_EOF);

	return firstFree;
}

/*-----------------------------------------------------------------
gets the first available free cluster, sets it
to end of file, links the input cluster to it, clears the new
cluster to 0 valued bytes, then returns the cluster number
If an error occurs, return CLUSTER_FREE
-----------------------------------------------------------------*/
u32 _FAT_fat_linkFreeClusterCleared (PARTITION* partition, u32 cluster) {
	u32 newCluster;
	int i;
	u8 emptySector[BYTES_PER_READ];

	// Link the cluster
	newCluster = _FAT_fat_linkFreeCluster(partition, cluster);

	if (newCluster == CLUSTER_FREE) {
		return CLUSTER_FREE;
	}

	// Clear all the sectors within the cluster
	memset (emptySector, 0, BYTES_PER_READ);
	for (i = 0; i < partition->sectorsPerCluster; i++) {
		_FAT_disc_writeSectors (partition->disc,
			_FAT_fat_clusterToSector (partition, newCluster) + i,
			1, emptySector);
	}

	return newCluster;
}


/*-----------------------------------------------------------------
_FAT_fat_clearLinks
frees any cluster used by a file
-----------------------------------------------------------------*/
bool _FAT_fat_clearLinks (PARTITION* partition, u32 cluster) {
	u32 nextCluster;

	if ((cluster < 0x0002) || (cluster > partition->fat.lastCluster))
		return false;

	// If this clears up more space in the FAT before the current free pointer, move it backwards
	if (cluster < partition->fat.firstFree) {
		partition->fat.firstFree = cluster;
	}

	while ((cluster != CLUSTER_EOF) && (cluster != CLUSTER_FREE)) {
		// Store next cluster before erasing the link
		nextCluster = _FAT_fat_nextCluster (partition, cluster);

		// Erase the link
		_FAT_fat_writeFatEntry (partition, cluster, CLUSTER_FREE);

		// Move onto next cluster
		cluster = nextCluster;
	}

	return true;
}

/*-----------------------------------------------------------------
_FAT_fat_lastCluster
Trace the cluster links until the last one is found
-----------------------------------------------------------------*/
u32 _FAT_fat_lastCluster (PARTITION* partition, u32 cluster) {
	while ((_FAT_fat_nextCluster(partition, cluster) != CLUSTER_FREE) && (_FAT_fat_nextCluster(partition, cluster) != CLUSTER_EOF)) {
		cluster = _FAT_fat_nextCluster(partition, cluster);
	}
	return cluster;
}
