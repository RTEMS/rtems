/*
 * $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

/**************************************************************************
 * This file implements the "mount" procedure for tar-based IMFS
 * extensions.  The TAR is not actually mounted under the IMFS.
 * Directories from the TAR file are created as usual in the IMFS.
 * File entries are created as IMFS_LINEAR_FILE nodes with their nods
 * pointing to addresses in the TAR image.
 *************************************************************************/

#include <rtems.h>
#include <rtems/libio_.h>
#include <string.h>
#include <chain.h>
#include <imfs.h>


/**************************************************************************
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
 *   257      8 bytes  Magic ("ustar  \0")
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
 *************************************************************************/

#define LF_OLDNORMAL  '\0'     /* Normal disk file, Unix compatible */
#define LF_NORMAL     '0'      /* Normal disk file                  */
#define LF_LINK       '1'      /* Link to previously dumped file    */
#define LF_SYMLINK    '2'      /* Symbolic link                     */
#define LF_CHR        '3'      /* Character special file            */
#define LF_BLK        '4'      /* Block special file                */
#define LF_DIR        '5'      /* Directory                         */
#define LF_FIFO       '6'      /* FIFO special file                 */
#define LF_CONFIG     '7'      /* Contiguous file                   */

#define MAX_NAME_FIELD_SIZE      99

#define MIN(a,b)   ((a)>(b)?(b):(a))

static unsigned long octal2ulong(char *octascii, int len);
static int compute_tar_header_checksum(char *bufr);

/**************************************************************************
 * rtems_tarfs_load
 *
 * Here we create the mountpoint directory and load the tarfs at
 * that node.  Once the IMFS has been mounted, we work through the
 * tar image and perform as follows:
 *  - For directories, simply call mkdir().  The IMFS creates nodes as
 *    needed.
 *  - For files, we make our own calls to IMFS eval_for_make and 
 *    create_node.
 *************************************************************************/
int
rtems_tarfs_load(char *mountpoint,
                 unsigned char *tar_image,
                 unsigned long tar_size)
{
   rtems_filesystem_location_info_t root_loc;
   rtems_filesystem_location_info_t loc;
   char            *hdr_ptr;
   char            filename[100];
   char            full_filename[256];
   int             hdr_chksum;
   unsigned char   linkflag;
   unsigned long   file_size;
   unsigned long   file_mode;
   int             offset;
   unsigned long   nblocks;
   IMFS_jnode_t    *node;
   int             status;


   status = rtems_filesystem_evaluate_path(mountpoint, 0, &root_loc, 0);
   if (status != 0)
      return(-1);

   if (root_loc.ops != &IMFS_ops)
      return(-1);

   /***********************************************************************
    * Create an IMFS node structure pointing to tar image memory.
    **********************************************************************/
   offset = 0;
   while (1)
   {
      if (offset + 512 > tar_size)
         break;

      /******************************************************************
       * Read a header.
       ******************************************************************/
      hdr_ptr = &tar_image[offset];
      offset += 512;
      if (strncmp(&hdr_ptr[257], "ustar  ", 7))
         break;

      strncpy(filename, hdr_ptr, MAX_NAME_FIELD_SIZE);
      filename[MAX_NAME_FIELD_SIZE] = '\0';

      linkflag   = hdr_ptr[156];
      file_mode  = octal2ulong(&hdr_ptr[100], 8);
      file_size  = octal2ulong(&hdr_ptr[124], 12);
      hdr_chksum = (int)octal2ulong(&hdr_ptr[148], 8);

      if (compute_tar_header_checksum(hdr_ptr) != hdr_chksum)
         break;

      /******************************************************************
       * Generate an IMFS node depending on the file type.
       * - For directories, just create directories as usual.  IMFS 
       *   will take care of the rest.
       * - For files, create a file node with special tarfs properties.
       *****************************************************************/
      if (linkflag == LF_DIR)
      {
         strcpy(full_filename, mountpoint);
         if (full_filename[strlen(full_filename)-1] != '/')
            strcat(full_filename, "/");
         strcat(full_filename, filename);
         mkdir(full_filename, S_IRWXU | S_IRWXG | S_IRWXO);
      }
      /******************************************************************
       * Create a LINEAR_FILE node if no user write permission.
       *****************************************************************/
      else if ((linkflag == LF_NORMAL) &&
               ((file_mode & 0200) == 0000))
      {
         const char  *name;

         loc = root_loc;
         if (IMFS_evaluate_for_make(filename, &loc, &name) == 0)
         {
            node = IMFS_create_node(&loc,
                                    IMFS_LINEAR_FILE, (char *)name,
                                    (S_IRUSR | S_IRGRP | S_IROTH) | S_IFREG,
                                    NULL);
            node->info.linearfile.size   = file_size;
            node->info.linearfile.direct = &tar_image[offset];
         }

         nblocks = (((file_size) + 511) & ~511) / 512;
         offset += 512 * nblocks;
      }
      /******************************************************************
       * Create a regular MEMORY_FILE if write permission exists.
       *****************************************************************/
      else if ((linkflag == LF_NORMAL) &&
               ((file_mode & 0200) == 0200))
      {
         int fd;
         int n, left, ptr;

         strcpy(full_filename, mountpoint);
         if (full_filename[strlen(full_filename)-1] != '/')
            strcat(full_filename, "/");
         strcat(full_filename, filename);

         fd = creat(full_filename, S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP);
         if (fd != -1)
         {
            left = file_size;
            ptr  = offset;
            while ((n = write(fd, &tar_image[ptr], left)) > 0)
            {
               left -= n;
               ptr += n;
            }
            close(fd);
         }

         nblocks = (((file_size) + 511) & ~511) / 512;
         offset += 512 * nblocks;
      }
   }

   return(status);
}

/**************************************************************************
 * This converts octal ASCII number representations into an
 * unsigned long.  Only support 32-bit numbers for now.
 *************************************************************************/
static unsigned long
octal2ulong(char *octascii, int len)
{
   int           i;
   unsigned long num;
   unsigned long mult;

   num = 0;
   mult = 1;
   for (i=len-1; i>=0; i--)
   {
      if (octascii[i] < '0') 
         continue;
      if (octascii[i] > '9')
         continue;

      num  += mult*((unsigned long)(octascii[i] - '0'));
      mult *= 8;
   }
   return(num);
}


/************************************************************************
 * Compute the TAR checksum and check with the value in 
 * the archive.  The checksum is computed over the entire
 * header, but the checksum field is substituted with blanks.
 ************************************************************************/
static int
compute_tar_header_checksum(char *bufr)
{
   int  i, sum;


   sum = 0;
   for (i=0; i<512; i++)
   {
      if ((i >= 148) && (i < 156))
         sum += 0xff & ' ';
      else
         sum += 0xff & bufr[i];
   }
   return(sum);
}
