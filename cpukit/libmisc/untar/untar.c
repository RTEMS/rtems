/* FIXME:
 *   1. Symbolic links are not created.
 *   2. Untar_FromMemory has printfs.
 *   3. Untar_FromMemory uses FILE *fp.
 *   4. How to determine end of archive?
 *
 *  Written by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "untar.h"


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
      if ((octascii[i] < '0') || (octascii[i] > '9'))
      {
         continue;
      }
      num  += mult*((unsigned long)(octascii[i] - '0'));
      mult *= 8;
   }
   return(num);
}


/**************************************************************************
 * Function: Untar_FromMemory                                             *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This is a simple subroutine used to rip links, directories, and     *
 *    files out of a block of memory.                                     *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    unsigned char *tar_buf    - Pointer to TAR buffer.                  *
 *    unsigned long size        - Length of TAR buffer.                   *
 *                                                                        *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    int - UNTAR_SUCCESSFUL (0)    on successful completion.             *
 *          UNTAR_INVALID_CHECKSUM  for an invalid header checksum.       *
 *          UNTAR_INVALID_HEADER    for an invalid header.                *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/30/1998 - Creation (JWJ)                                           *
 *************************************************************************/
int
Untar_FromMemory(unsigned char *tar_buf, unsigned long size)
{
   FILE           *fp;
   char           *bufr;
   size_t         n;
   char           fname[100];
   char           linkname[100];
   int            sum;
   int            hdr_chksum;
   int            retval;
   unsigned long  ptr;
   unsigned long  i;
   unsigned long  nblocks;
   unsigned long  file_size;
   unsigned char  linkflag;


   ptr = 0;
   while (1)
   {
      if (ptr + 512 > size)
      {
         retval = UNTAR_SUCCESSFUL;
         break;
      }

      /* Read the header */
      bufr = &tar_buf[ptr];
      ptr += 512;
      if (strncmp(&bufr[257], "ustar  ", 7))
      {
         retval = UNTAR_SUCCESSFUL;
         break;
      }

      strncpy(fname, bufr, MAX_NAME_FIELD_SIZE);
      fname[MAX_NAME_FIELD_SIZE] = '\0';

      linkflag   = bufr[156];
      file_size  = octal2ulong(&bufr[124], 12);

      /******************************************************************
       * Compute the TAR checksum and check with the value in 
       * the archive.  The checksum is computed over the entire
       * header, but the checksum field is substituted with blanks.
       ******************************************************************/
      hdr_chksum = (int)octal2ulong(&bufr[148], 8);
      sum = 0;
      for (i=0; i<512; i++)
      {
         if ((i >= 148) && (i < 156))
         {
            sum += 0xff & ' ';
         }
         else
         {
            sum += 0xff & bufr[i];
         }
      }
      if (sum != hdr_chksum)
      {
         retval = UNTAR_INVALID_CHECKSUM;
         break;
      }


      /******************************************************************
       * We've decoded the header, now figure out what it contains and
       * do something with it.
       *****************************************************************/
      if (linkflag == LF_SYMLINK)
      {
         strncpy(linkname, &bufr[157], MAX_NAME_FIELD_SIZE);
         linkname[MAX_NAME_FIELD_SIZE] = '\0';
         /* symlink(fname, linkname); */
      }
      else if (linkflag == LF_NORMAL)
      {
         nblocks = (((file_size) + 511) & ~511) / 512;
         if ((fp = fopen(fname, "w")) == NULL)
         {
            printf("Untar failed to create file %s\n", fname);
            ptr += 512 * nblocks;
         }
         else
         {
            unsigned long sizeToGo = file_size;
            unsigned long len;

            /***************************************************************
             * Read out the data.  There are nblocks of data where nblocks
             * is the file_size rounded to the nearest 512-byte boundary.
             **************************************************************/
            for (i=0; i<nblocks; i++)
            {
               len = ((sizeToGo < 512L)?(sizeToGo):(512L));
               n = fwrite(&tar_buf[ptr], 1, len, fp);
               if (n != len)
               {
                  printf("Error during write\n");
                  break;
               }
               ptr += 512;
               sizeToGo -= n;
            }
            fclose(fp);
         }
      }
      else if (linkflag == LF_DIR)
      {
         mkdir(fname, S_IRWXU | S_IRWXG | S_IRWXO);
      }
   }

   return(retval);
}


/**************************************************************************
 * Function: Untar_FromFile                                               *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This is a simple subroutine used to rip links, directories, and     *
 *    files out of a TAR file.                                            *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *tar_name   - TAR filename.                                    *
 *                                                                        *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    int - UNTAR_SUCCESSFUL (0)    on successful completion.             *
 *          UNTAR_INVALID_CHECKSUM  for an invalid header checksum.       *
 *          UNTAR_INVALID_HEADER    for an invalid header.                *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/30/1998 - Creation (JWJ)                                           *
 *************************************************************************/
int
Untar_FromFile(char *tar_name)
{
   int            fd;
   char           *bufr;
   size_t         n;
   char           fname[100];
   char           linkname[100];
   int            sum;
   int            hdr_chksum;
   int            retval;
   unsigned long  i;
   unsigned long  nblocks;
   unsigned long  size;
   unsigned char  linkflag;


   retval = UNTAR_SUCCESSFUL;
   bufr = (char *)malloc(512);
   if (bufr == NULL)
   {
      return(UNTAR_FAIL);
   }

   fd = open(tar_name, O_RDONLY);
   while (1)
   {
      /* Read the header */
      /* If the header read fails, we just consider it the end
         of the tarfile. */
      if ((n = read(fd, bufr, 512)) != 512)
      {
         break;
      }

      if (strncmp(&bufr[257], "ustar  ", 7))
      {
         break;
      }

      strncpy(fname, bufr, MAX_NAME_FIELD_SIZE);
      fname[MAX_NAME_FIELD_SIZE] = '\0';

      linkflag   = bufr[156];
      size       = octal2ulong(&bufr[124], 12);

      /******************************************************************
       * Compute the TAR checksum and check with the value in 
       * the archive.  The checksum is computed over the entire
       * header, but the checksum field is substituted with blanks.
       ******************************************************************/
      hdr_chksum = (int)octal2ulong(&bufr[148], 8);
      sum = 0;
      for (i=0; i<512; i++)
      {
         if ((i >= 148) && (i < 156))
         {
            sum += 0xff & ' ';
         }
         else
         {
            sum += 0xff & bufr[i];
         }
      }
      if (sum != hdr_chksum)
      {
         retval = UNTAR_INVALID_CHECKSUM;
         break;
      }


      /******************************************************************
       * We've decoded the header, now figure out what it contains and
       * do something with it.
       *****************************************************************/
      if (linkflag == LF_SYMLINK)
      {
         strncpy(linkname, &bufr[157], MAX_NAME_FIELD_SIZE);
         linkname[MAX_NAME_FIELD_SIZE] = '\0';
      }
      else if (linkflag == LF_NORMAL)
      {
         int out_fd;

         /******************************************************************
          * Read out the data.  There are nblocks of data where nblocks
          * is the size rounded to the nearest 512-byte boundary.
          *****************************************************************/
         nblocks = (((size) + 511) & ~511) / 512;

         if ((out_fd = creat(fname, 0644)) == -1)
         {
            for (i=0; i<nblocks; i++)
            {
               n = read(fd, bufr, 512);
            }
         }
         else
         {
            for (i=0; i<nblocks; i++)
            {
               n = read(fd, bufr, 512);
               n = MIN(n, size - i*512);
               write(out_fd, bufr, n);
            }
            close(out_fd);
         }
      }
      else if (linkflag == LF_DIR)
      {
         mkdir(fname, S_IRWXU | S_IRWXG | S_IRWXO);
      }
   }
   free(bufr);
   close(fd);

   return(retval);
}

