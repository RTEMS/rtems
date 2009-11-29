/*
 file_allocation_table.h
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

	2006-10-01 - Chishm
		* Added _FAT_fat_linkFreeClusterCleared to clear a cluster when it is allocated
*/

#ifndef _FAT_H
#define _FAT_H

#include "common.h"
#include "partition.h"

#define CLUSTER_EOF_16	0xFFFF
#define	CLUSTER_EOF	0x0FFFFFFF
#define CLUSTER_FREE	0x0000
#define CLUSTER_FIRST	0x0002

#define CLUSTERS_PER_FAT12 4085
#define CLUSTERS_PER_FAT16 65525


u32 _FAT_fat_nextCluster(PARTITION* partition, u32 cluster);

u32 _FAT_fat_linkFreeCluster(PARTITION* partition, u32 cluster);
u32 _FAT_fat_linkFreeClusterCleared (PARTITION* partition, u32 cluster);

bool _FAT_fat_clearLinks (PARTITION* partition, u32 cluster);

u32 _FAT_fat_lastCluster (PARTITION* partition, u32 cluster);

static inline u32 _FAT_fat_clusterToSector (PARTITION* partition, u32 cluster) {
	return (cluster >= 2) ? ((cluster - 2) * partition->sectorsPerCluster) + partition->dataStart : partition->rootDirStart;
}

#endif // _FAT_H
