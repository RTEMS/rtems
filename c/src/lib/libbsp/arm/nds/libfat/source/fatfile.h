/*
 fatfile.h

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

	2006-07-24 - Chishm
		* Removed padding workaround from FILE_STRUCT

	2006-08-13 - Chishm
		* Moved all externally visible directory related functions to fatdir
*/


#ifndef _FATFILE_H
#define _FATFILE_H

#include <sys/reent.h>
#include <sys/stat.h>

#include "common.h"
#include "partition.h"
#include "directory.h"

typedef struct {
	u32 cluster;
	u32 sector;
	s32 byte;
} FILE_POSITION;

typedef struct {
	u32 filesize;
	u32 startCluster;
	u32 currentPosition;
	FILE_POSITION rwPosition;
	FILE_POSITION appendPosition;
	bool read;
	bool write;
	bool append;
	bool inUse;
	PARTITION* partition;
	DIR_ENTRY_POSITION dirEntryStart;		// Points to the start of the LFN entries of a file, or the alias for no LFN
	DIR_ENTRY_POSITION dirEntryEnd;			// Always points to the file's alias entry
} FILE_STRUCT;

extern int _FAT_open_r (struct _reent *r, void *fileStruct, const char *path, int flags, int mode);

extern int _FAT_close_r (struct _reent *r, int fd);

extern int _FAT_write_r (struct _reent *r,int fd, const char *ptr, int len);

extern int _FAT_read_r (struct _reent *r, int fd, char *ptr, int len);

extern int _FAT_seek_r (struct _reent *r, int fd,int pos, int dir);

extern int _FAT_fstat_r (struct _reent *r, int fd, struct stat *st);

extern int _FAT_stat_r (struct _reent *r, const char *path, struct stat *st);

extern int _FAT_link_r (struct _reent *r, const char *existing, const char *newLink);

extern int _FAT_unlink_r (struct _reent *r, const char *name);

extern int _FAT_chdir_r (struct _reent *r, const char *name);

extern int _FAT_rename_r (struct _reent *r, const char *oldName, const char *newName);

#endif // _FATFILE_H
