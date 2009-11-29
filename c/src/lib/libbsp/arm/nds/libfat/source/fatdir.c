/*
 fatdir.c

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

	2006-08-13 - Chishm
		* Moved all externally visible directory related functions to fatdir
		* Added _FAT_mkdir_r

	2006-08-14 - Chishm
		* Added directory iterator functions

	2006-08-19 - Chishm
		* Updated dirnext return values to return correctly

	2006-10-01 - Chishm
		* Now clears the whole cluster when creating a new directory, bug found by Hermes

	2007-01-10 - Chishm
		* Updated directory iterator functions for DevkitPro r20
*/

#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/dir.h>

#include "fatdir.h"

#include "cache.h"
#include "file_allocation_table.h"
#include "partition.h"
#include "directory.h"
#include "bit_ops.h"
#include "filetime.h"


int _FAT_stat_r (struct _reent *r, const char *path, struct stat *st) {
	PARTITION* partition = NULL;

	DIR_ENTRY dirEntry;

	// Get the partition this file is on
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

	// Search for the file on the disc
	if (!_FAT_directory_entryFromPath (partition, &dirEntry, path, NULL)) {
		r->_errno = ENOENT;
		return -1;
	}

	// Fill in the stat struct
	_FAT_directory_entryStat (partition, &dirEntry, st);

	return 0;
}

int _FAT_link_r (struct _reent *r, const char *existing, const char *newLink) {
	r->_errno = ENOTSUP;
	return -1;
}

int _FAT_unlink_r (struct _reent *r, const char *path) {
	PARTITION* partition = NULL;
	DIR_ENTRY dirEntry;
	DIR_ENTRY dirContents;
	u32 cluster;
	bool nextEntry;
	bool errorOccured = false;

	// Get the partition this directory is on
	partition = _FAT_partition_getPartitionFromPath (path);

	if (partition == NULL) {
		r->_errno = ENODEV;
		return -1;
	}

	// Make sure we aren't trying to write to a read-only disc
	if (partition->readOnly) {
		r->_errno = EROFS;
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

	// Search for the file on the disc
	if (!_FAT_directory_entryFromPath (partition, &dirEntry, path, NULL)) {
		r->_errno = ENOENT;
		return -1;
	}

	cluster = _FAT_directory_entryGetCluster (dirEntry.entryData);


	// If this is a directory, make sure it is empty
	if (_FAT_directory_isDirectory (&dirEntry)) {
		nextEntry = _FAT_directory_getFirstEntry (partition, &dirContents, cluster);

		while (nextEntry) {
			if (!_FAT_directory_isDot (&dirContents)) {
				// The directory had something in it that isn't a reference to itself or it's parent
				r->_errno = EPERM;
				return -1;
			}
			nextEntry = _FAT_directory_getNextEntry (partition, &dirContents);
		}
	}

	if (cluster != CLUSTER_FREE) {
		// Remove the cluster chain for this file
		if (!_FAT_fat_clearLinks (partition, cluster)) {
			r->_errno = EIO;
			errorOccured = true;
		}
	}

	// Remove the directory entry for this file
	if (!_FAT_directory_removeEntry (partition, &dirEntry)) {
		r->_errno = EIO;
		errorOccured = true;
	}

	// Flush any sectors in the disc cache
	if (!_FAT_cache_flush(partition->cache)) {
		r->_errno = EIO;
		errorOccured = true;
	}

	if (errorOccured) {
		return -1;
	} else {
		return 0;
	}
}

int _FAT_chdir_r (struct _reent *r, const char *path) {
	PARTITION* partition = NULL;

	// Get the partition this directory is on
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

	// Set the default device to match this one
	if (!_FAT_partition_setDefaultPartition (partition)) {
		r->_errno = ENOENT;
		return -1;
	}

	// Try changing directory
	if (_FAT_directory_chdir (partition, path)) {
		// Successful
		return 0;
	} else {
		// Failed
		r->_errno = ENOTDIR;
		return -1;
	}
}

int _FAT_rename_r (struct _reent *r, const char *oldName, const char *newName) {
	PARTITION* partition = NULL;
	DIR_ENTRY oldDirEntry;
	DIR_ENTRY newDirEntry;
	const char *pathEnd;
	u32 dirCluster;

	// Get the partition this directory is on
	partition = _FAT_partition_getPartitionFromPath (oldName);

	if (partition == NULL) {
		r->_errno = ENODEV;
		return -1;
	}

	// Make sure the same partition is used for the old and new names
	if (partition != _FAT_partition_getPartitionFromPath (newName)) {
		r->_errno = EXDEV;
		return -1;
	}

	// Make sure we aren't trying to write to a read-only disc
	if (partition->readOnly) {
		r->_errno = EROFS;
		return -1;
	}

	// Move the path pointer to the start of the actual path
	if (strchr (oldName, ':') != NULL) {
		oldName = strchr (oldName, ':') + 1;
	}
	if (strchr (oldName, ':') != NULL) {
		r->_errno = EINVAL;
		return -1;
	}
	if (strchr (newName, ':') != NULL) {
		newName = strchr (newName, ':') + 1;
	}
	if (strchr (newName, ':') != NULL) {
		r->_errno = EINVAL;
		return -1;
	}

	// Search for the file on the disc
	if (!_FAT_directory_entryFromPath (partition, &oldDirEntry, oldName, NULL)) {
		r->_errno = ENOENT;
		return -1;
	}

	// Make sure there is no existing file / directory with the new name
	if (_FAT_directory_entryFromPath (partition, &newDirEntry, newName, NULL)) {
		r->_errno = EEXIST;
		return -1;
	}

	// Create the new file entry
	// Get the directory it has to go in
	pathEnd = strrchr (newName, DIR_SEPARATOR);
	if (pathEnd == NULL) {
		// No path was specified
		dirCluster = partition->cwdCluster;
		pathEnd = newName;
	} else {
		// Path was specified -- get the right dirCluster
		// Recycling newDirEntry, since it needs to be recreated anyway
		if (!_FAT_directory_entryFromPath (partition, &newDirEntry, newName, pathEnd) ||
			!_FAT_directory_isDirectory(&newDirEntry)) {
			r->_errno = ENOTDIR;
			return -1;
		}
		dirCluster = _FAT_directory_entryGetCluster (newDirEntry.entryData);
		// Move the pathEnd past the last DIR_SEPARATOR
		pathEnd += 1;
	}

	// Copy the entry data
	memcpy (&newDirEntry, &oldDirEntry, sizeof(DIR_ENTRY));

	// Set the new name
	strncpy (newDirEntry.filename, pathEnd, MAX_FILENAME_LENGTH - 1);

	// Write the new entry
	if (!_FAT_directory_addEntry (partition, &newDirEntry, dirCluster)) {
		r->_errno = ENOSPC;
		return -1;
	}

	// Remove the old entry
	if (!_FAT_directory_removeEntry (partition, &oldDirEntry)) {
		r->_errno = EIO;
		return -1;
	}

	// Flush any sectors in the disc cache
	if (!_FAT_cache_flush (partition->cache)) {
		r->_errno = EIO;
		return -1;
	}

	return 0;
}

int _FAT_mkdir_r (struct _reent *r, const char *path, int mode) {
	PARTITION* partition = NULL;
	bool fileExists;
	DIR_ENTRY dirEntry;
	const char* pathEnd;
	u32 parentCluster, dirCluster;
	u8 newEntryData[DIR_ENTRY_DATA_SIZE];

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

	// Search for the file/directory on the disc
	fileExists = _FAT_directory_entryFromPath (partition, &dirEntry, path, NULL);

	// Make sure it doesn't exist
	if (fileExists) {
		r->_errno = EEXIST;
		return -1;
	}

	if (partition->readOnly) {
		// We can't write to a read-only partition
		r->_errno = EROFS;
		return -1;
	}

	// Get the directory it has to go in
	pathEnd = strrchr (path, DIR_SEPARATOR);
	if (pathEnd == NULL) {
		// No path was specified
		parentCluster = partition->cwdCluster;
		pathEnd = path;
	} else {
		// Path was specified -- get the right parentCluster
		// Recycling dirEntry, since it needs to be recreated anyway
		if (!_FAT_directory_entryFromPath (partition, &dirEntry, path, pathEnd) ||
			!_FAT_directory_isDirectory(&dirEntry)) {
			r->_errno = ENOTDIR;
			return -1;
		}
		parentCluster = _FAT_directory_entryGetCluster (dirEntry.entryData);
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

	// Set the directory attribute
	dirEntry.entryData[DIR_ENTRY_attributes] = ATTRIB_DIR;

	// Get a cluster for the new directory
	dirCluster = _FAT_fat_linkFreeClusterCleared (partition, CLUSTER_FREE);
	if (dirCluster == CLUSTER_FREE) {
		// No space left on disc for the cluster
		r->_errno = ENOSPC;
		return -1;
	}
	u16_to_u8array (dirEntry.entryData, DIR_ENTRY_cluster, dirCluster);
	u16_to_u8array (dirEntry.entryData, DIR_ENTRY_clusterHigh, dirCluster >> 16);

	// Write the new directory's entry to it's parent
	if (!_FAT_directory_addEntry (partition, &dirEntry, parentCluster)) {
		r->_errno = ENOSPC;
		return -1;
	}

	// Create the dot entry within the directory
	memset (newEntryData, 0, DIR_ENTRY_DATA_SIZE);
	memset (newEntryData, ' ', 11);
	newEntryData[DIR_ENTRY_name] = '.';
	newEntryData[DIR_ENTRY_attributes] = ATTRIB_DIR;
	u16_to_u8array (newEntryData, DIR_ENTRY_cluster, dirCluster);
	u16_to_u8array (newEntryData, DIR_ENTRY_clusterHigh, dirCluster >> 16);

	// Write it to the directory, erasing that sector in the process
	_FAT_cache_eraseWritePartialSector ( partition->cache, newEntryData,
		_FAT_fat_clusterToSector (partition, dirCluster), 0, DIR_ENTRY_DATA_SIZE);


	// Create the double dot entry within the directory
	newEntryData[DIR_ENTRY_name + 1] = '.';
	u16_to_u8array (newEntryData, DIR_ENTRY_cluster, parentCluster);
	u16_to_u8array (newEntryData, DIR_ENTRY_clusterHigh, parentCluster >> 16);

	// Write it to the directory
	_FAT_cache_writePartialSector ( partition->cache, newEntryData,
		_FAT_fat_clusterToSector (partition, dirCluster), DIR_ENTRY_DATA_SIZE, DIR_ENTRY_DATA_SIZE);

	// Flush any sectors in the disc cache
	if (!_FAT_cache_flush(partition->cache)) {
		r->_errno = EIO;
		return -1;
	}

	return 0;
}

DIR_ITER* _FAT_diropen_r(struct _reent *r, DIR_ITER *dirState, const char *path) {
	DIR_ENTRY dirEntry;
	DIR_STATE_STRUCT* state = (DIR_STATE_STRUCT*) (dirState->dirStruct);
	bool fileExists;

	state->partition = _FAT_partition_getPartitionFromPath (path);

	if (state->partition == NULL) {
		r->_errno = ENODEV;
		return NULL;
	}

	// Move the path pointer to the start of the actual path
	if (strchr (path, ':') != NULL) {
		path = strchr (path, ':') + 1;
	}
	if (strchr (path, ':') != NULL) {
		r->_errno = EINVAL;
		return NULL;
	}
	// Get the start cluster of the directory
	fileExists = _FAT_directory_entryFromPath (state->partition, &dirEntry, path, NULL);

	if (!fileExists) {
		r->_errno = ENOENT;
		return NULL;
	}

	// Make sure it is a directory
	if (! _FAT_directory_isDirectory (&dirEntry)) {
		r->_errno = ENOTDIR;
		return NULL;
	}

	// Save the start cluster for use when resetting the directory data
	state->startCluster = _FAT_directory_entryGetCluster (dirEntry.entryData);

	// Get the first entry for use with a call to dirnext
	state->validEntry =
		_FAT_directory_getFirstEntry (state->partition, &(state->currentEntry), state->startCluster);

	// We are now using this entry
	state->inUse = true;
	return (DIR_ITER*) state;
}

int _FAT_dirreset_r (struct _reent *r, DIR_ITER *dirState) {
	DIR_STATE_STRUCT* state = (DIR_STATE_STRUCT*) (dirState->dirStruct);

	// Make sure we are still using this entry
	if (!state->inUse) {
		r->_errno = EBADF;
		return -1;
	}

	// Get the first entry for use with a call to dirnext
	state->validEntry =
		_FAT_directory_getFirstEntry (state->partition, &(state->currentEntry), state->startCluster);

	return 0;
}

int _FAT_dirnext_r (struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat) {
	DIR_STATE_STRUCT* state = (DIR_STATE_STRUCT*) (dirState->dirStruct);

	// Make sure we are still using this entry
	if (!state->inUse) {
		r->_errno = EBADF;
		return -1;
	}

	// Make sure there is another file to report on
	if (! state->validEntry) {
		r->_errno = ENOENT;
		return -1;
	}

	// Get the filename
	strncpy (filename, state->currentEntry.filename, MAX_FILENAME_LENGTH);
	// Get the stats, if requested
	if (filestat != NULL) {
		_FAT_directory_entryStat (state->partition, &(state->currentEntry), filestat);
	}

	// Look for the next entry for use next time
	state->validEntry =
		_FAT_directory_getNextEntry (state->partition, &(state->currentEntry));

	return 0;
}

int _FAT_dirclose_r (struct _reent *r, DIR_ITER *dirState) {
	DIR_STATE_STRUCT* state = (DIR_STATE_STRUCT*) (dirState->dirStruct);

	// We are no longer using this entry
	state->inUse = false;

	return 0;
}
