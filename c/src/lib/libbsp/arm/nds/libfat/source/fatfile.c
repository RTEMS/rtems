/*
 fatfile.c

 Functions used by the newlib disc stubs to interface with
 this library

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

	2006-07-17 - Chishm
		* Made all path inputs const char*
		* Added _FAT_rename_r

	2006-08-02 - Chishm
		* Fixed _FAT_seek_r

	2006-08-13 - Chishm
		* Moved all externally visible directory related functions to fatdir
*/


#include "fatfile.h"

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include "cache.h"
#include "file_allocation_table.h"
#include "bit_ops.h"
#include "filetime.h"

int _FAT_open_r (struct _reent *r, void *fileStruct, const char *path, int flags, int mode) {
	PARTITION* partition = NULL;
	bool fileExists;
	DIR_ENTRY dirEntry;
	const char* pathEnd;
	u32 dirCluster;
	FILE_STRUCT* file = (FILE_STRUCT*) fileStruct;
	partition = _FAT_partition_getPartitionFromPath (path);

	if (partition == NULL) {
		r->_errno = ENODEV;
		return -1;
	}

	// Move the path pointer to the start of the actual path
	if (strchr (path, ':') != NULL) {
		path = strchr (path, ':') + 1;
	}
	if (strchr (path, ':') != NULL) {
		r->_errno = EINVAL;
		return -1;
	}

	// Determine which mode the file is openned for
	if ((flags & 0x03) == O_RDONLY) {
		// Open the file for read-only access
		file->read = true;
		file->write = false;
		file->append = false;
	} else if ((flags & 0x03) == O_WRONLY) {
		// Open file for write only access
		file->read = false;
		file->write = true;
		file->append = false;
	} else if ((flags & 0x03) == O_RDWR) {
		// Open file for read/write access
		file->read = true;
		file->write = true;
		file->append = false;
	} else {
		r->_errno = EACCES;
		return -1;
	}

	// Make sure we aren't trying to write to a read-only disc
	if (file->write && partition->readOnly) {
		r->_errno = EROFS;
		return -1;
	}

	// Search for the file on the disc
	fileExists = _FAT_directory_entryFromPath (partition, &dirEntry, path, NULL);

	// The file shouldn't exist if we are trying to create it
	if ((flags & O_CREAT) && (flags & O_EXCL) && fileExists) {
		r->_errno = EEXIST;
		return -1;
	}

	// It should not be a directory if we're openning a file,
	if (fileExists && _FAT_directory_isDirectory(&dirEntry)) {
		r->_errno = EISDIR;
		return -1;
	}

	// If the file doesn't exist, create it if we're allowed to
	if (!fileExists) {
		if (flags & O_CREAT) {
			if (partition->readOnly) {
				// We can't write to a read-only partition
				r->_errno = EROFS;
				return -1;
			}
			// Create the file
			// Get the directory it has to go in
			pathEnd = strrchr (path, DIR_SEPARATOR);
			if (pathEnd == NULL) {
				// No path was specified
				dirCluster = partition->cwdCluster;
				pathEnd = path;
			} else {
				// Path was specified -- get the right dirCluster
				// Recycling dirEntry, since it needs to be recreated anyway
				if (!_FAT_directory_entryFromPath (partition, &dirEntry, path, pathEnd) ||
					!_FAT_directory_isDirectory(&dirEntry)) {
					r->_errno = ENOTDIR;
					return -1;
				}
				dirCluster = _FAT_directory_entryGetCluster (dirEntry.entryData);
				// Move the pathEnd past the last DIR_SEPARATOR
				pathEnd += 1;
			}
			// Create the entry data
			strncpy (dirEntry.filename, pathEnd, MAX_FILENAME_LENGTH - 1);
			memset (dirEntry.entryData, 0, DIR_ENTRY_DATA_SIZE);

			// Set the creation time and date
			dirEntry.entryData[DIR_ENTRY_cTime_ms] = 0;
			u16_to_u8array (dirEntry.entryData, DIR_ENTRY_cTime, _FAT_filetime_getTimeFromRTC());
			u16_to_u8array (dirEntry.entryData, DIR_ENTRY_cDate, _FAT_filetime_getDateFromRTC());

			if (!_FAT_directory_addEntry (partition, &dirEntry, dirCluster)) {
				r->_errno = ENOSPC;
				return -1;
			}
		} else {
			// file doesn't exist, and we aren't creating it
			r->_errno = ENOENT;
			return -1;
		}
	}

	file->filesize = u8array_to_u32 (dirEntry.entryData, DIR_ENTRY_fileSize);

	/* Allow LARGEFILEs with undefined results
	// Make sure that the file size can fit in the available space
	if (!(flags & O_LARGEFILE) && (file->filesize >= (1<<31))) {
		r->_errno = EFBIG;
		return -1;
	}
	*/

	// Make sure we aren't trying to write to a read-only file
	if (file->write && !_FAT_directory_isWritable(&dirEntry)) {
		r->_errno = EROFS;
		return -1;
	}

	// Associate this file with a particular partition
	file->partition = partition;

	file->startCluster = _FAT_directory_entryGetCluster (dirEntry.entryData);

	// Truncate the file if requested
	if ((flags & O_TRUNC) && file->write && (file->startCluster != 0)) {
		_FAT_fat_clearLinks (partition, file->startCluster);
		file->startCluster = 0;
		file->filesize = 0;
	}

	// Get a new cluster for the file if required
	if (file->startCluster == 0) {
		file->startCluster = _FAT_fat_linkFreeCluster (partition, CLUSTER_FREE);
	}

	// Remember the position of this file's directory entry
	file->dirEntryStart = dirEntry.dataStart;		// Points to the start of the LFN entries of a file, or the alias for no LFN
	file->dirEntryEnd = dirEntry.dataEnd;

	file->currentPosition = 0;

	file->rwPosition.cluster = file->startCluster;
	file->rwPosition.sector = 0;
	file->rwPosition.byte = 0;

	file->appendPosition.cluster = _FAT_fat_lastCluster (partition, file->startCluster);
	file->appendPosition.sector = (file->filesize % partition->bytesPerCluster) / BYTES_PER_READ;
	file->appendPosition.byte = file->filesize % BYTES_PER_READ;

	// Check if the end of the file is on the end of a cluster
	if ( (file->filesize > 0) && ((file->filesize % partition->bytesPerCluster)==0) ){
		// Set flag to allocate a new cluster
		file->appendPosition.sector = partition->sectorsPerCluster;
		file->appendPosition.byte = 0;
	}

	if (flags & O_APPEND) {
		file->append = true;
	}

	file->inUse = true;

	partition->openFileCount += 1;

	return (int) file;
}

int _FAT_close_r (struct _reent *r, int fd) {
	FILE_STRUCT* file = (FILE_STRUCT*)  fd;
	u8 dirEntryData[DIR_ENTRY_DATA_SIZE];

	if (!file->inUse) {
		r->_errno = EBADF;
		return -1;
	}
	if (file->write) {
		// Load the old entry
		_FAT_cache_readPartialSector (file->partition->cache, dirEntryData,
			_FAT_fat_clusterToSector(file->partition, file->dirEntryEnd.cluster) + file->dirEntryEnd.sector,
			file->dirEntryEnd.offset * DIR_ENTRY_DATA_SIZE, DIR_ENTRY_DATA_SIZE);

		// Write new data to the directory entry
		// File size
		u32_to_u8array (dirEntryData, DIR_ENTRY_fileSize, file->filesize);

		// Start cluster
		u16_to_u8array (dirEntryData, DIR_ENTRY_cluster, file->startCluster);
		u16_to_u8array (dirEntryData, DIR_ENTRY_clusterHigh, file->startCluster >> 16);

		// Modification time and date
		u16_to_u8array (dirEntryData, DIR_ENTRY_mTime, _FAT_filetime_getTimeFromRTC());
		u16_to_u8array (dirEntryData, DIR_ENTRY_mDate, _FAT_filetime_getDateFromRTC());

		// Access date
		u16_to_u8array (dirEntryData, DIR_ENTRY_aDate, _FAT_filetime_getDateFromRTC());

		// Write the new entry
		_FAT_cache_writePartialSector (file->partition->cache, dirEntryData,
			_FAT_fat_clusterToSector(file->partition, file->dirEntryEnd.cluster) + file->dirEntryEnd.sector,
			file->dirEntryEnd.offset * DIR_ENTRY_DATA_SIZE, DIR_ENTRY_DATA_SIZE);

		// Flush any sectors in the disc cache
		if (!_FAT_cache_flush(file->partition->cache)) {
			r->_errno = EIO;
			return -1;
		}
	}

	file->inUse = false;
	file->partition->openFileCount -= 1;

	return 0;
}

int _FAT_read_r (struct _reent *r, int fd, char *ptr, int len) {
	FILE_STRUCT* file = (FILE_STRUCT*)  fd;

	PARTITION* partition;
	CACHE* cache;

	FILE_POSITION position;
	u32 tempNextCluster;

	int tempVar;

	u32 remain;

	bool flagNoError = true;

	// Make sure we can actually read from the file
	if ((file == NULL) || !file->inUse || !file->read) {
		r->_errno = EBADF;
		return 0;
	}

	// Don't try to read if the read pointer is past the end of file
	if (file->currentPosition >= file->filesize) {
		return 0;
	}

	// Don't read past end of file
	if (len + file->currentPosition > file->filesize) {
		r->_errno = EOVERFLOW;
		len = file->filesize - file->currentPosition;
	}

	remain = len;

	position = file->rwPosition;

	partition = file->partition;
	cache = file->partition->cache;

	// Align to sector
	tempVar = BYTES_PER_READ - position.byte;
	if (tempVar > remain) {
		tempVar = remain;
	}

	if ((tempVar < BYTES_PER_READ) && flagNoError)
	{
		_FAT_cache_readPartialSector ( cache, ptr, _FAT_fat_clusterToSector (partition, position.cluster) + position.sector,
			position.byte, tempVar);

		remain -= tempVar;
		ptr += tempVar;

		position.byte += tempVar;
		if (position.byte >= BYTES_PER_READ) {
			position.byte = 0;
			position.sector++;
		}
	}

	// align to cluster
	// tempVar is number of sectors to read
	if (remain > (partition->sectorsPerCluster - position.sector) * BYTES_PER_READ) {
		tempVar = partition->sectorsPerCluster - position.sector;
	} else {
		tempVar = remain / BYTES_PER_READ;
	}

	if ((tempVar > 0) && flagNoError) {
		_FAT_disc_readSectors (partition->disc, _FAT_fat_clusterToSector (partition, position.cluster) + position.sector,
			tempVar, ptr);
		ptr += tempVar * BYTES_PER_READ;
		remain -= tempVar * BYTES_PER_READ;
		position.sector += tempVar;
	}

	// Move onto next cluster
	// It should get to here without reading anything if a cluster is due to be allocated
	if (position.sector >= partition->sectorsPerCluster) {
		tempNextCluster = _FAT_fat_nextCluster(partition, position.cluster);
		if ((remain == 0) && (tempNextCluster == CLUSTER_EOF)) {
			position.sector = partition->sectorsPerCluster;
		} else if (tempNextCluster == CLUSTER_FREE) {
			r->_errno = EIO;
			flagNoError = false;
		} else {
			position.sector = 0;
			position.cluster = tempNextCluster;
		}
	}

	// Read in whole clusters
	while ((remain >= partition->bytesPerCluster) && flagNoError) {
		_FAT_disc_readSectors (partition->disc, _FAT_fat_clusterToSector (partition, position.cluster), partition->sectorsPerCluster, ptr);
		ptr += partition->bytesPerCluster;
		remain -= partition->bytesPerCluster;

		// Advance to next cluster
		tempNextCluster = _FAT_fat_nextCluster(partition, position.cluster);
		if ((remain == 0) && (tempNextCluster == CLUSTER_EOF)) {
			position.sector = partition->sectorsPerCluster;
		} else if (tempNextCluster == CLUSTER_FREE) {
			r->_errno = EIO;
			flagNoError = false;
		} else {
			position.sector = 0;
			position.cluster = tempNextCluster;
		}
	}

	// Read remaining sectors
	tempVar = remain / BYTES_PER_READ; // Number of sectors left
	if ((tempVar > 0) && flagNoError) {
		_FAT_disc_readSectors (partition->disc, _FAT_fat_clusterToSector (partition, position.cluster),
			tempVar, ptr);
		ptr += tempVar * BYTES_PER_READ;
		remain -= tempVar * BYTES_PER_READ;
		position.sector += tempVar;
	}

	// Last remaining sector
	// Check if anything is left
	if ((remain > 0) && flagNoError) {
		_FAT_cache_readPartialSector ( cache, ptr,
			_FAT_fat_clusterToSector (partition, position.cluster) + position.sector, 0, remain);
		position.byte += remain;
		remain = 0;
	}

	// Length read is the wanted length minus the stuff not read
	len = len - remain;

	// Update file information
	file->rwPosition = position;
	file->currentPosition += len;
	return len;
}

/*
Extend a file so that the size is the same as the rwPosition
*/
static bool file_extend_r (struct _reent *r, FILE_STRUCT* file) {
	PARTITION* partition = file->partition;
	CACHE* cache = file->partition->cache;

	FILE_POSITION position;

	u32 remain;

	u8 zeroBuffer [BYTES_PER_READ] = {0};

	u32 tempNextCluster;

	position.byte = file->filesize % BYTES_PER_READ;
	position.sector = (file->filesize % partition->bytesPerCluster) / BYTES_PER_READ;
	position.cluster = _FAT_fat_lastCluster (partition, file->startCluster);

	remain = file->currentPosition - file->filesize;


	// Only need to clear to the end of the sector
	if (remain + position.byte < BYTES_PER_READ) {
		_FAT_cache_writePartialSector (cache, zeroBuffer,
			_FAT_fat_clusterToSector (partition, position.cluster) + position.sector, position.byte, remain);
		position.byte += remain;
	} else {
		if (position.byte > 0) {
			_FAT_cache_writePartialSector (cache, zeroBuffer,
				_FAT_fat_clusterToSector (partition, position.cluster) + position.sector, position.byte,
				BYTES_PER_READ - position.byte);
			remain -= (BYTES_PER_READ - position.byte);
			position.byte = 0;
			position.sector ++;
		}

		while (remain >= BYTES_PER_READ) {
			if (position.sector >= partition->sectorsPerCluster) {
				position.sector = 0;
				tempNextCluster = _FAT_fat_nextCluster(partition, position.cluster);
				if ((tempNextCluster == CLUSTER_EOF) || (tempNextCluster == CLUSTER_FREE)) {
					// Ran out of clusters so get a new one
					tempNextCluster = _FAT_fat_linkFreeCluster(partition, position.cluster);
				}
				if (tempNextCluster == CLUSTER_FREE) {
					// Couldn't get a cluster, so abort
					r->_errno = ENOSPC;
					return false;
				} else {
					position.cluster = tempNextCluster;
				}
			}

			_FAT_disc_writeSectors (partition->disc,
				_FAT_fat_clusterToSector (partition, position.cluster) + position.sector, 1, zeroBuffer);

			remain -= BYTES_PER_READ;
			position.sector ++;
		}

		if (position.sector >= partition->sectorsPerCluster) {
			position.sector = 0;
			tempNextCluster = _FAT_fat_nextCluster(partition, position.cluster);
			if ((tempNextCluster == CLUSTER_EOF) || (tempNextCluster == CLUSTER_FREE)) {
				// Ran out of clusters so get a new one
				tempNextCluster = _FAT_fat_linkFreeCluster(partition, position.cluster);
			}
			if (tempNextCluster == CLUSTER_FREE) {
				// Couldn't get a cluster, so abort
				r->_errno = ENOSPC;
				return false;
			} else {
				position.cluster = tempNextCluster;
			}
		}

		if (remain > 0) {
			_FAT_cache_writePartialSector (cache, zeroBuffer,
				_FAT_fat_clusterToSector (partition, position.cluster) + position.sector, 0, remain);
			position.byte = remain;
		}
	}

	file->rwPosition = position;
	file->filesize = file->currentPosition;
	return true;
}


int _FAT_write_r (struct _reent *r,int fd, const char *ptr, int len) {
	FILE_STRUCT* file = (FILE_STRUCT*)  fd;

	PARTITION* partition;
	CACHE* cache;

	FILE_POSITION position;
	u32 tempNextCluster;

	int tempVar;

	u32 remain;

	bool flagNoError = true;
	bool flagAppending = false;

	// Make sure we can actually write to the file
	if ((file == NULL) || !file->inUse || !file->write) {
		r->_errno = EBADF;
		return -1;
	}

	partition = file->partition;
	cache = file->partition->cache;
	remain = len;

	if (file->append) {
		position = file->appendPosition;
		flagAppending = true;
	} else {
		// If the write pointer is past the end of the file, extend the file to that size
		if (file->currentPosition > file->filesize) {
			if (!file_extend_r (r, file)) {
				return 0;
			}
		}

		// Write at current read pointer
		position = file->rwPosition;

		// If it is writing past the current end of file, set appending flag
		if (len + file->currentPosition > file->filesize) {
			flagAppending = true;
		}
	}

	// Move onto next cluster if needed
	if (position.sector >= partition->sectorsPerCluster) {
		position.sector = 0;
		tempNextCluster = _FAT_fat_nextCluster(partition, position.cluster);
		if ((tempNextCluster == CLUSTER_EOF) || (tempNextCluster == CLUSTER_FREE)) {
			// Ran out of clusters so get a new one
			tempNextCluster = _FAT_fat_linkFreeCluster(partition, position.cluster);
		}
		if (tempNextCluster == CLUSTER_FREE) {
			// Couldn't get a cluster, so abort
			r->_errno = ENOSPC;
			flagNoError = false;
		} else {
			position.cluster = tempNextCluster;
		}
	}

	// Align to sector
	tempVar = BYTES_PER_READ - position.byte;
	if (tempVar > remain) {
		tempVar = remain;
	}

	if ((tempVar < BYTES_PER_READ) && flagNoError) {
		// Write partial sector to disk
		_FAT_cache_writePartialSector (cache, ptr,
			_FAT_fat_clusterToSector (partition, position.cluster) + position.sector, position.byte, tempVar);

		remain -= tempVar;
		ptr += tempVar;
		position.byte += tempVar;


		// Move onto next sector
		if (position.byte >= BYTES_PER_READ) {
			position.byte = 0;
			position.sector ++;
		}
	}

	// Align to cluster
	// tempVar is number of sectors to write
	if (remain > (partition->sectorsPerCluster - position.sector) * BYTES_PER_READ) {
		tempVar = partition->sectorsPerCluster - position.sector;
	} else {
		tempVar = remain / BYTES_PER_READ;
	}

	if ((tempVar > 0) && flagNoError) {
		_FAT_disc_writeSectors (partition->disc,
			_FAT_fat_clusterToSector (partition, position.cluster) + position.sector, tempVar, ptr);
		ptr += tempVar * BYTES_PER_READ;
		remain -= tempVar * BYTES_PER_READ;
		position.sector += tempVar;
	}

	if ((position.sector >= partition->sectorsPerCluster) && flagNoError && (remain > 0)) {
		position.sector = 0;
		tempNextCluster = _FAT_fat_nextCluster(partition, position.cluster);
		if ((tempNextCluster == CLUSTER_EOF) || (tempNextCluster == CLUSTER_FREE)) {
			// Ran out of clusters so get a new one
			tempNextCluster = _FAT_fat_linkFreeCluster(partition, position.cluster);
		}
		if (tempNextCluster == CLUSTER_FREE) {
			// Couldn't get a cluster, so abort
			r->_errno = ENOSPC;
			flagNoError = false;
		} else {
			position.cluster = tempNextCluster;
		}
	}

	// Write whole clusters
	while ((remain >= partition->bytesPerCluster) && flagNoError) {
		_FAT_disc_writeSectors (partition->disc, _FAT_fat_clusterToSector(partition, position.cluster),
			partition->sectorsPerCluster, ptr);
		ptr += partition->bytesPerCluster;
		remain -= partition->bytesPerCluster;
		if (remain > 0) {
			tempNextCluster = _FAT_fat_nextCluster(partition, position.cluster);
			if ((tempNextCluster == CLUSTER_EOF) || (tempNextCluster == CLUSTER_FREE)) {
				// Ran out of clusters so get a new one
				tempNextCluster = _FAT_fat_linkFreeCluster(partition, position.cluster);
			}
			if (tempNextCluster == CLUSTER_FREE) {
				// Couldn't get a cluster, so abort
				r->_errno = ENOSPC;
				flagNoError = false;
			} else {
				position.cluster = tempNextCluster;
			}
		} else {
			// Allocate a new cluster when next writing the file
			position.sector = partition->sectorsPerCluster;
		}
	}

	// Write remaining sectors
	tempVar = remain / BYTES_PER_READ; // Number of sectors left
	if ((tempVar > 0) && flagNoError) {
		_FAT_disc_writeSectors (partition->disc, _FAT_fat_clusterToSector (partition, position.cluster),
			tempVar, ptr);
		ptr += tempVar * BYTES_PER_READ;
		remain -= tempVar * BYTES_PER_READ;
		position.sector += tempVar;
	}

	// Last remaining sector
	if ((remain > 0) && flagNoError) {
		if (flagAppending) {
			_FAT_cache_eraseWritePartialSector ( cache, ptr,
				_FAT_fat_clusterToSector (partition, position.cluster) + position.sector, 0, remain);
		} else {
			_FAT_cache_writePartialSector ( cache, ptr,
				_FAT_fat_clusterToSector (partition, position.cluster) + position.sector, 0, remain);
		}
		position.byte += remain;
		remain = 0;
	}


	// Amount read is the originally requested amount minus stuff remaining
	len = len - remain;

	// Update file information
	if (file->append) {
		// Appending doesn't affect the read pointer
		file->appendPosition = position;
		file->filesize += len;
	} else {
		// Writing also shifts the read pointer
		file->rwPosition = position;
		file->currentPosition += len;
		if (file->filesize < file->currentPosition) {
			file->filesize = file->currentPosition;
		}
	}

	return len;
}


int _FAT_seek_r (struct _reent *r, int fd, int pos, int dir) {
	FILE_STRUCT* file = (FILE_STRUCT*)  fd;

	PARTITION* partition;

	u32 cluster, nextCluster;
	int clusCount;
	int position;

	if ((file == NULL) || (file->inUse == false))	 {
		// invalid file
		r->_errno = EBADF;
		return -1;
	}

	partition = file->partition;

	switch (dir) {
		case SEEK_SET:
			position = pos;
			break;
		case SEEK_CUR:
			position = file->currentPosition + pos;
			break;
		case SEEK_END:
			position = file->filesize + pos;
			break;
		default:
			r->_errno = EINVAL;
			return -1;
	}

	if ((pos > 0) && (position < 0)) {
		r->_errno = EOVERFLOW;
		return -1;
	}

	if (position < 0) {
		r->_errno = EINVAL;
		return -1;
	}

	// Only change the read/write position if it is within the bounds of the current filesize
	if (file->filesize > position) {

		// Calculate the sector and byte of the current position,
		// and store them
		file->rwPosition.sector = (position % partition->bytesPerCluster) / BYTES_PER_READ;
		file->rwPosition.byte = position % BYTES_PER_READ;

		// Calculate where the correct cluster is
		if (position >= file->currentPosition) {
			clusCount = (position / partition->bytesPerCluster) - (file->currentPosition / partition->bytesPerCluster);
			cluster = file->rwPosition.cluster;
		} else {
			clusCount = position / partition->bytesPerCluster;
			cluster = file->startCluster;
		}

		nextCluster = _FAT_fat_nextCluster (partition, cluster);
		while ((clusCount > 0) && (nextCluster != CLUSTER_FREE) && (nextCluster != CLUSTER_EOF)) {
			clusCount--;
			cluster = nextCluster;
			nextCluster = _FAT_fat_nextCluster (partition, cluster);
		}

		// Check if ran out of clusters, and the file is being written to
		if ((clusCount > 0) && (file->write || file->append)) {
			// Set flag to allocate a new cluster
			file->rwPosition.sector = partition->sectorsPerCluster;
			file->rwPosition.byte = 0;
		}

		file->rwPosition.cluster = cluster;
	}

	// Save position
	file->currentPosition = position;

	return position;
}



int _FAT_fstat_r (struct _reent *r, int fd, struct stat *st) {
	FILE_STRUCT* file = (FILE_STRUCT*)  fd;

	PARTITION* partition;

	DIR_ENTRY fileEntry;

	if ((file == NULL) || (file->inUse == false))	 {
		// invalid file
		r->_errno = EBADF;
		return -1;
	}

	partition = file->partition;

	// Get the file's entry data
	fileEntry.dataStart = file->dirEntryStart;
	fileEntry.dataEnd = file->dirEntryEnd;

	if (!_FAT_directory_entryFromPosition (partition, &fileEntry)) {
		r->_errno = EIO;
		return -1;
	}

	// Fill in the stat struct
	_FAT_directory_entryStat (partition, &fileEntry, st);

	// Fix stats that have changed since the file was openned
  	st->st_ino = (ino_t)(file->startCluster);		// The file serial number is the start cluster
	st->st_size = file->filesize;					// File size

	return 0;
}

