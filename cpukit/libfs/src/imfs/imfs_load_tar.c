/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

/*
 * This file implements the "mount" procedure for tar-based IMFS
 * extensions.  The TAR is not actually mounted under the IMFS.
 * Directories from the TAR file are created as usual in the IMFS.
 * File entries are created as IMFS_LINEAR_FILE nodes with their nods
 * pointing to addresses in the TAR image.
 */

#include "imfs.h"

#include <sys/stat.h>
#include <string.h>
#include <tar.h>

#include <rtems/untar.h>

/*
 * TAR file format:
 *
 *   Offset   Length   Contents
 *     0    100 bytes  File name ('\0' terminated, 99 maxmum length)
 *   100      8 bytes  File mode (in octal ascii)
 *   108      8 bytes  User ID (in octal ascii)
 *   116      8 bytes  Group ID (in octal ascii)
 *   124     12 bytes  File size (s) (in octal ascii)
 *   136     12 bytes  Modify time (in octal ascii)
 *   148      8 bytes  Header checksum (in octal ascii)
 *   156      1 bytes  Link flag
 *   157    100 bytes  Linkname ('\0' terminated, 99 maxmum length)
 *   257      8 bytes  Magic PAX ("ustar\0" + 2 bytes padding)
 *   257      8 bytes  Magic GNU tar ("ustar  \0")
 *   265     32 bytes  User name ('\0' terminated, 31 maxmum length)
 *   297     32 bytes  Group name ('\0' terminated, 31 maxmum length)
 *   329      8 bytes  Major device ID (in octal ascii)
 *   337      8 bytes  Minor device ID (in octal ascii)
 *   345    167 bytes  Padding
 *   512   (s+p)bytes  File contents (s+p) := (((s) + 511) & ~511),
 *                     round up to 512 bytes
 *
 *   Checksum:
 *   int i, sum;
 *   char* header = tar_header_pointer;
 *   sum = 0;
 *   for(i = 0; i < 512; i++)
 *       sum += 0xFF & header[i];
 */

#define MAX_NAME_FIELD_SIZE      99

#define MIN(a,b)   ((a)>(b)?(b):(a))

/*
 * rtems_tarfs_load
 *
 * Here we create the mountpoint directory and load the tarfs at
 * that node.  Once the IMFS has been mounted, we work through the
 * tar image and perform as follows:
 *  - For directories, simply call mkdir().  The IMFS creates nodes as
 *    needed.
 *  - For files, we make our own calls to IMFS eval_for_make and
 *    create_node.
 */
int rtems_tarfs_load(
  const char *mountpoint,
  uint8_t *tar_image,
  size_t tar_size
)
{
   const char                       *hdr_ptr;
   char                             filename[100];
   char                             full_filename[256];
   int                              hdr_chksum;
   unsigned char                    linkflag;
   unsigned long                    file_size;
   unsigned long                    file_mode;
   int                              offset;
   unsigned long                    nblocks;
   IMFS_jnode_t                    *node;
   int rv = 0;
   int eval_flags = RTEMS_FS_FOLLOW_LINK;
   rtems_filesystem_eval_path_context_t ctx;
   rtems_filesystem_location_info_t rootloc;
   rtems_filesystem_location_info_t *currentloc =
     rtems_filesystem_eval_path_start( &ctx, mountpoint, eval_flags );

   rtems_filesystem_eval_path_extract_currentloc( &ctx, &rootloc );
   rtems_filesystem_eval_path_set_flags(
     &ctx,
     RTEMS_FS_MAKE | RTEMS_FS_EXCLUSIVE
   );

   if (
     rootloc.mt_entry->ops != &IMFS_ops
       && rootloc.mt_entry->ops != &fifoIMFS_ops
   ) {
     rv = -1;
   }

   /*
    * Create an IMFS node structure pointing to tar image memory.
    */
   offset = 0;
   while ( rv == 0 ) {
    if (offset + 512 > tar_size)
      break;

    /*
     * Read a header.
     */
    hdr_ptr = (char *) &tar_image[offset];
    offset += 512;
    if (strncmp(&hdr_ptr[257], "ustar", 5))
      break;

    strncpy(filename, hdr_ptr, MAX_NAME_FIELD_SIZE);
    filename[MAX_NAME_FIELD_SIZE] = '\0';

    linkflag   = hdr_ptr[156];
    file_mode  = _rtems_octal2ulong(&hdr_ptr[100], 8);
    file_size  = _rtems_octal2ulong(&hdr_ptr[124], 12);
    hdr_chksum = _rtems_octal2ulong(&hdr_ptr[148], 8);

    if (_rtems_tar_header_checksum(hdr_ptr) != hdr_chksum)
      break;

    /*
     * Generate an IMFS node depending on the file type.
     * - For directories, just create directories as usual.  IMFS
     *   will take care of the rest.
     * - For files, create a file node with special tarfs properties.
     */
    if (linkflag == DIRTYPE) {
      strcpy(full_filename, mountpoint);
      if (full_filename[strlen(full_filename)-1] != '/')
        strcat(full_filename, "/");
      strcat(full_filename, filename);
      mkdir(full_filename, S_IRWXU | S_IRWXG | S_IRWXO);
    }
    /*
     * Create a LINEAR_FILE node
     */
    else if (linkflag == REGTYPE) {
      rtems_filesystem_location_free( currentloc );
      rtems_filesystem_location_clone( currentloc, &rootloc );
      rtems_filesystem_eval_path_set_path(
        &ctx,
        filename,
        strlen( filename )
      );
      rtems_filesystem_eval_path_continue( &ctx );

      if ( !rtems_filesystem_location_is_null( currentloc ) ) {
        node = IMFS_create_node(
          currentloc,
          IMFS_LINEAR_FILE,
          rtems_filesystem_eval_path_get_token( &ctx ),
          rtems_filesystem_eval_path_get_tokenlen( &ctx ),
          (file_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) | S_IFREG,
          NULL
        );
        node->info.linearfile.size   = file_size;
        node->info.linearfile.direct = &tar_image[offset];
      }

      nblocks = (((file_size) + 511) & ~511) / 512;
      offset += 512 * nblocks;
    }
  }

  rtems_filesystem_location_free( &rootloc );
  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}

