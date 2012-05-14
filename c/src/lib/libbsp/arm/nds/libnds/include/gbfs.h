/*---------------------------------------------------------------------------------
	access object in a GBFS file

	Copyright 2002-2004 Damian Yerrick

	Additional code Dave Murphy

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
	BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
	AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
	OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
	IN THE SOFTWARE.

---------------------------------------------------------------------------------*/


/* Dependency on prior include files

Before you #include "gbfs.h", you should define the following types:
  typedef (unsigned 16-bit integer) u16;
  typedef (unsigned 32-bit integer) u32;
Your system header should do this for you.
*/

#ifndef _INCLUDE_GBFS_H_
#define _INCLUDE_GBFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* to make a 300 KB space called samples do GBFS_SPACE(samples, 300) */

#define GBFS_SPACE(filename, kbytes) \
	const char filename[(kbytes)*1024] __attribute__ ((aligned (16))) = \
	"PinEightGBFSSpace-" #filename "-" #kbytes ;

typedef struct GBFS_FILE {
	char magic[16];    /* "PinEightGBFS\r\n\032\n" */
	u32  total_len;    /* total length of archive */
	u16  dir_off;      /* offset in bytes to directory */
	u16  dir_nmemb;    /* number of files */
	char reserved[8];  /* for future use */
} GBFS_FILE;

typedef struct GBFS_ENTRY {
	char name[24];     /* filename, nul-padded */
	u32  len;          /* length of object in bytes */
	u32  data_offset;  /* in bytes from beginning of file */
} GBFS_ENTRY;


/*---------------------------------------------------------------------------------
	Set the search limits and stride for searching
---------------------------------------------------------------------------------*/
void gbfs_search_range(	u32 gbfs_1st_limit,
						u32 gbfs_2nd_start, u32 gbfs_2nd_limit,
						u32 gbfs_stride );

const GBFS_FILE * find_first_gbfs_file(const void *start);

const void *skip_gbfs_file(const GBFS_FILE * file);

const void *gbfs_get_obj(const GBFS_FILE *file,
                         const char *name,
                         u32 *len);

const void *gbfs_get_nth_obj(GBFS_FILE const * file,
                             size_t n,
                             char *name,
                             u32 *len);

void *gbfs_copy_obj(void *dst,
                    const GBFS_FILE *file,
                    const char *name);

size_t gbfs_count_objs(const GBFS_FILE *file);


#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_GBFS_H_
