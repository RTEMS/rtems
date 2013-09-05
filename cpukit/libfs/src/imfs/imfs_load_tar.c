/**
 * @file
 *
 * @brief RTEMS Load Tarfs
 * @ingroup IMFS
 */

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

#include "imfs.h"

#include <sys/stat.h>
#include <string.h>
#include <tar.h>

#include <rtems/untar.h>

#define MAX_NAME_FIELD_SIZE      99

#define MIN(a,b)   ((a)>(b)?(b):(a))

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
      int len;
      strncpy(full_filename, mountpoint, 255);
      if (full_filename[(len=strlen(full_filename))-1] != '/')
        strcat(full_filename, "/");
      ++len;
      strncat(full_filename, filename, 256-len-1);
      rv = mkdir(full_filename, S_IRWXU | S_IRWXG | S_IRWXO);
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

