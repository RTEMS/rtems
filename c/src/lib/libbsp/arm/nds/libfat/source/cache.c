/*
 cache.c
 The cache is not visible to the user. It should be flushed
 when any file is closed or changes are made to the filesystem.

 This cache implements a least-used-page replacement policy. This will
 distribute sectors evenly over the pages, so if less than the maximum
 pages are used at once, they should all eventually remain in the cache.
 This also has the benefit of throwing out old sectors, so as not to keep
 too many stale pages around.

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
*/

#include <string.h>

#include "common.h"
#include "cache.h"
#include "disc_io/disc.h"

#include "mem_allocate.h"

#define CACHE_FREE 0xFFFFFFFF

CACHE* _FAT_cache_constructor (u32 numberOfPages, const IO_INTERFACE* discInterface) {
	CACHE* cache;
	u32 i;
	CACHE_ENTRY* cacheEntries;

	if (numberOfPages < 2) {
		numberOfPages = 2;
	}

	cache = (CACHE*) _FAT_mem_allocate (sizeof(CACHE));
	if (cache == NULL) {
		return false;
	}

	cache->disc = discInterface;
	cache->numberOfPages = numberOfPages;


	cacheEntries = (CACHE_ENTRY*) _FAT_mem_allocate ( sizeof(CACHE_ENTRY) * numberOfPages);
	if (cacheEntries == NULL) {
		_FAT_mem_free (cache);
		return false;
	}

	for (i = 0; i < numberOfPages; i++) {
		cacheEntries[i].sector = CACHE_FREE;
		cacheEntries[i].count = 0;
		cacheEntries[i].dirty = false;
	}

	cache->cacheEntries = cacheEntries;

	cache->pages = (u8*) _FAT_mem_allocate ( CACHE_PAGE_SIZE * numberOfPages);
	if (cache->pages == NULL) {
		_FAT_mem_free (cache->cacheEntries);
		_FAT_mem_free (cache);
		return false;
	}

	return cache;
}

void _FAT_cache_destructor (CACHE* cache) {
	// Clear out cache before destroying it
	_FAT_cache_flush(cache);

	// Free memory in reverse allocation order
	_FAT_mem_free (cache->pages);
	_FAT_mem_free (cache->cacheEntries);
	_FAT_mem_free (cache);

	return;
}

/*
Retrieve a sector's page from the cache. If it is not found in the cache,
load it into the cache and return the page it was loaded to.
Return CACHE_FREE on error.
*/
static u32 _FAT_cache_getSector (CACHE* cache, u32 sector) {
	u32 i;
	CACHE_ENTRY* cacheEntries = cache->cacheEntries;
	u32 numberOfPages = cache->numberOfPages;

	u32 leastUsed = 0;
	u32 lowestCount = 0xFFFFFFFF;

	for (i = 0; (i < numberOfPages) && (cacheEntries[i].sector != sector); i++) {
		// While searching for the desired sector, also search for the leased used page
		if ( (cacheEntries[i].sector == CACHE_FREE) || (cacheEntries[i].count < lowestCount) ) {
			leastUsed = i;
			lowestCount = cacheEntries[i].count;
		}
	}

	// If it found the sector in the cache, return it
	if ((i < numberOfPages) && (cacheEntries[i].sector == sector)) {
		// Increment usage counter
		cacheEntries[i].count += 1;
		return i;
	}

	// If it didn't, replace the least used cache page with the desired sector
	if ((cacheEntries[leastUsed].sector != CACHE_FREE) && (cacheEntries[leastUsed].dirty == true)) {
		// Write the page back to disc if it has been written to
		if (!_FAT_disc_writeSectors (cache->disc, cacheEntries[leastUsed].sector, 1, cache->pages + CACHE_PAGE_SIZE * leastUsed)) {
			return CACHE_FREE;
		}
		cacheEntries[leastUsed].dirty = false;
	}

	// Load the new sector into the cache
	if (!_FAT_disc_readSectors (cache->disc, sector, 1, cache->pages + CACHE_PAGE_SIZE * leastUsed)) {
		return CACHE_FREE;
	}
	cacheEntries[leastUsed].sector = sector;
	// Increment the usage count, don't reset it
	// This creates a paging policy of least used PAGE, not sector
	cacheEntries[leastUsed].count += 1;
	return leastUsed;
}

/*
Reads some data from a cache page, determined by the sector number
*/
bool _FAT_cache_readPartialSector (CACHE* cache, void* buffer, u32 sector, u32 offset, u32 size) {
	u32 page;

	if (offset + size > BYTES_PER_READ) {
		return false;
	}

	page = _FAT_cache_getSector (cache, sector);
	if (page == CACHE_FREE) {
		return false;
	}
	memcpy (buffer, cache->pages + (CACHE_PAGE_SIZE * page) + offset, size);
	return true;
}

/*
Writes some data to a cache page, making sure it is loaded into memory first.
*/
bool _FAT_cache_writePartialSector (CACHE* cache, const void* buffer, u32 sector, u32 offset, u32 size) {
	u32 page;

	if (offset + size > BYTES_PER_READ) {
		return false;
	}

	page = _FAT_cache_getSector (cache, sector);
	if (page == CACHE_FREE) {
		return false;
	}

	memcpy (cache->pages + (CACHE_PAGE_SIZE * page) + offset, buffer, size);
	cache->cacheEntries[page].dirty = true;

	return true;
}

/*
Writes some data to a cache page, zeroing out the page first
*/
bool _FAT_cache_eraseWritePartialSector (CACHE* cache, const void* buffer, u32 sector, u32 offset, u32 size) {
	u32 page;

	if (offset + size > BYTES_PER_READ) {
		return false;
	}

	page = _FAT_cache_getSector (cache, sector);
	if (page == CACHE_FREE) {
		return false;
	}

	memset (cache->pages + (CACHE_PAGE_SIZE * page), 0, CACHE_PAGE_SIZE);
	memcpy (cache->pages + (CACHE_PAGE_SIZE * page) + offset, buffer, size);
	cache->cacheEntries[page].dirty = true;

	return true;
}


/*
Flushes all dirty pages to disc, clearing the dirty flag.
Also resets all pages' page count to 0.
*/
bool _FAT_cache_flush (CACHE* cache) {
	u32 i;

	for (i = 0; i < cache->numberOfPages; i++) {
		if (cache->cacheEntries[i].dirty) {
			if (!_FAT_disc_writeSectors (cache->disc, cache->cacheEntries[i].sector, 1, cache->pages + CACHE_PAGE_SIZE * i)) {
				return CACHE_FREE;
			}
		}
		cache->cacheEntries[i].count = 0;
		cache->cacheEntries[i].dirty = false;
	}

	return true;
}

void _FAT_cache_invalidate (CACHE* cache) {
	int i;
	for (i = 0; i < cache->numberOfPages; i++) {
		cache->cacheEntries[i].sector = CACHE_FREE;
		cache->cacheEntries[i].count = 0;
		cache->cacheEntries[i].dirty = false;
	}
}
