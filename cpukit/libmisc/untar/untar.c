/**
 * @file

 * @brief Untar an Image
 * @ingroup libmisc_untar_img Untar Image

 * FIXME:
 *   1. Symbolic links are not created.
 *   2. Untar_FromMemory uses FILE *fp.
 *   3. How to determine end of archive?

 */

/*
 *  Written by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 *  Copyright 2016 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <rtems/untar.h>
#include <rtems/bspIo.h>

/*
 * TAR file format:

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
 *   345    155 bytes  Prefix
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

/*
 * This converts octal ASCII number representations into an
 * unsigned long.  Only support 32-bit numbers for now.
 *
 * warning: this code is referenced in the IMFS.
 */
unsigned long
_rtems_octal2ulong(
  const char *octascii,
  size_t len
)
{
  size_t        i;
  unsigned long num;

  num = 0;
  for (i=0; i < len; i++) {
    if ((octascii[i] < '0') || (octascii[i] > '9')) {
      continue;
    }
    num  = num * 8 + ((unsigned long)(octascii[i] - '0'));
  }
  return(num);
}

/*
 * Common error message formatter.
 */
static void
Print_Error(const rtems_printer *printer, const char* message, const char* path)
{
  rtems_printf(printer, "untar: %s: %s: (%d) %s\n",
               message, path, errno, strerror(errno));
}

/*
 * Get the type of node on in the file system if present.
 */
static int
Stat_Node(const char* path)
{
  struct stat sb;
  if (stat(path, &sb) < 0)
    return -1;
  if (S_ISDIR(sb.st_mode))
    return DIRTYPE;
  return REGTYPE;
}

/*
 * Make the directory path for a file if it does not exist.
 */
static int
Make_Path(const rtems_printer *printer, const char* filename, bool end_is_dir)
{
  char* copy = strdup(filename);
  char* path = copy;

  /*
   * Skip leading path separators.
   */
  while (*path == '/')
    ++path;

  /*
   * Any path left?
   */
  if (*path != '\0') {
    bool  path_end = false;
    char* end = path;
    int   r;

    /*
     * Split the path into directory components. Check the node and if a file
     * and not the end of the path remove it and create a directory. If a
     * directory and not the end of the path decend into the directory.
     */
    while (!path_end) {
      while (*end != '\0' && *end != '/')
        ++end;

      /*
       * Are we at the end of the path?
       */
      if (*end == '\0')
        path_end = true;

      /*
       * Split the path.
       */
      *end = '\0';

      /*
       * Get the node's status, exists, error, directory or regular? Regular
       * means not a directory.
       */
      r = Stat_Node(path);

      /*
       * If there are errors other than not existing we are finished.
       */
      if (r < 0 && errno != ENOENT) {
        Print_Error(printer, "stat", path);
        return -1;
      }

      /*
       * If a file remove and create a directory if not the end.
       */
      if (r == REGTYPE) {
        r = unlink(path);
        if (r < 0) {
          Print_Error(printer, "unlink", path);
          free(copy);
          return -1;
        }
        if (!path_end) {
          r = mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
          if (r < 0) {
            Print_Error(printer, "mkdir (unlink)", path);
            free(copy);
            return -1;
          }
        }
      }
      else if (r < 0) {
        /*
         * Node does not exist which means the rest of the path will not exist.
         */
        while (!path_end) {
          r = mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
          if (r < 0) {
            Print_Error(printer, "mkdir", path);
            free(copy);
            return -1;
          }
          if (!path_end) {
            *end = '/';
            ++end;
          }
          while (*end != '\0' && *end != '/')
            ++end;
          if (*end == '\0')
            path_end = true;
        }
      }
      else if (path_end && r == DIRTYPE && !end_is_dir) {
        /*
         * We only handle a directory if at the end of the path and the end is
         * a file. If we cannot remove the directory because it is not empty we
         * raise an error. Otherwise this is a directory and we do nothing
         * which lets us decend into it.
         */
        r = rmdir(path);
        if (r < 0) {
          Print_Error(printer, "rmdir", path);
          free(copy);
          return -1;
        }
      }

      /*
       * If not the end of the path put back the directory separator.
       */
      if (!path_end) {
        *end = '/';
        ++end;
      }
    }
  }

  free(copy);

  return 0;
}

static int
Untar_ProcessHeader(
  const char          *bufr,
  char                *fname,
  unsigned long       *mode,
  unsigned long       *file_size,
  unsigned long       *nblocks,
  unsigned char       *linkflag,
  const rtems_printer *printer
)
{
  char           linkname[100];
  int            sum;
  int            hdr_chksum;
  int            retval = UNTAR_SUCCESSFUL;

  fname[0] = '\0';
  *file_size = 0;
  *nblocks = 0;
  *linkflag = -1;

  if (strncmp(&bufr[257], "ustar", 5)) {
    return UNTAR_SUCCESSFUL;
  }

  /*
   * Compute the TAR checksum and check with the value in the archive.  The
   * checksum is computed over the entire header, but the checksum field is
   * substituted with blanks.
   */
  hdr_chksum = _rtems_octal2ulong(&bufr[148], 8);
  sum        = _rtems_tar_header_checksum(bufr);

  if (sum != hdr_chksum) {
    rtems_printf(printer, "untar: file header checksum error\n");
    return UNTAR_INVALID_CHECKSUM;
  }

  strncpy(fname, bufr, MAX_NAME_FIELD_SIZE);
  fname[MAX_NAME_FIELD_SIZE] = '\0';

  *mode = strtoul(&bufr[100], NULL, 8);

  *linkflag   = bufr[156];
  *file_size = _rtems_octal2ulong(&bufr[124], 12);

  /*
   * We've decoded the header, now figure out what it contains and do something
   * with it.
   */
  if (*linkflag == SYMTYPE) {
    strncpy(linkname, &bufr[157], MAX_NAME_FIELD_SIZE);
    linkname[MAX_NAME_FIELD_SIZE] = '\0';
    rtems_printf(printer, "untar: symlink: %s -> %s\n", linkname, fname);
    symlink(linkname, fname);
  } else if (*linkflag == REGTYPE) {
    rtems_printf(printer, "untar: file: %s (s:%i,m:%04o)\n",
                 fname, (int) *file_size, (int) *mode);
    *nblocks = (((*file_size) + 511) & ~511) / 512;
    if (Make_Path(printer, fname, false) < 0) {
      retval  = UNTAR_FAIL;
    }
  } else if (*linkflag == DIRTYPE) {
    int r;
    rtems_printf(printer, "untar:  dir: %s\n", fname);
    if (Make_Path(printer, fname, true) < 0) {
      retval  = UNTAR_FAIL;
    }
    r = mkdir(fname, S_IRWXU | S_IRWXG | S_IRWXO);
    if (r < 0) {
      if (errno == EEXIST) {
        struct stat stat_buf;
        if (stat(fname, &stat_buf) == 0) {
          if (S_ISDIR(stat_buf.st_mode)) {
            r = 0;
          } else {
            r = unlink(fname);
            if (r == 0) {
              r = mkdir(fname, *mode);
            }
          }
        }
      }
      if (r < 0) {
        Print_Error(printer, "mkdir", fname);
        retval = UNTAR_FAIL;
      }
    }
  }

  return retval;
}

/*
 * Function: Untar_FromMemory
 *
 * Description:
 *
 *    This is a simple subroutine used to rip links, directories, and
 *    files out of a block of memory.
 *
 *
 * Inputs:
 *
 *    void *  tar_buf    - Pointer to TAR buffer.
 *    size_t  size       - Length of TAR buffer.
 *
 *
 * Output:
 *
 *    int - UNTAR_SUCCESSFUL (0)    on successful completion.
 *          UNTAR_INVALID_CHECKSUM  for an invalid header checksum.
 *          UNTAR_INVALID_HEADER    for an invalid header.
 *
 */
int
Untar_FromMemory_Print(
  void                *tar_buf,
  size_t               size,
  const rtems_printer *printer
)
{
  int            fd;
  const char     *tar_ptr = (const char *)tar_buf;
  const char     *bufr;
  char           fname[100];
  int            retval = UNTAR_SUCCESSFUL;
  unsigned long  ptr;
  unsigned long  nblocks = 0;
  unsigned long  file_size = 0;
  unsigned long  mode = 0;
  unsigned char  linkflag = 0;

  rtems_printf(printer, "untar: memory at %p (%zu)\n", tar_buf, size);

  ptr = 0;
  while (true) {
    if (ptr + 512 > size) {
      retval = UNTAR_SUCCESSFUL;
      break;
    }

    /* Read the header */
    bufr = &tar_ptr[ptr];
    ptr += 512;

    retval = Untar_ProcessHeader(bufr, fname, &mode, &file_size,
                                 &nblocks, &linkflag, printer);

    if (retval != UNTAR_SUCCESSFUL)
      break;

    if (linkflag == REGTYPE) {
      if ((fd = open(fname, O_TRUNC | O_CREAT | O_WRONLY, mode)) == -1) {
        Print_Error(printer, "open", fname);
        ptr += 512 * nblocks;
      } else {
        unsigned long sizeToGo = file_size;
        ssize_t       len;
        ssize_t       i;
        ssize_t       n;

        /*
         * Read out the data.  There are nblocks of data where nblocks is the
         * file_size rounded to the nearest 512-byte boundary.
         */
        for (i = 0; i < nblocks; i++) {
          len = ((sizeToGo < 512L) ? (sizeToGo) : (512L));
          n = write(fd, &tar_ptr[ptr], len);
          if (n != len) {
            Print_Error(printer, "write", fname);
            retval  = UNTAR_FAIL;
            break;
          }
          ptr += 512;
          sizeToGo -= n;
        }
        close(fd);
      }

    }
  }

  return retval;
}

/*
 * Function: Untar_FromMemory
 *
 * Description:
 *
 *    This is a simple subroutine used to rip links, directories, and
 *    files out of a block of memory.
 *
 *
 * Inputs:
 *
 *    void *  tar_buf    - Pointer to TAR buffer.
 *    size_t  size       - Length of TAR buffer.
 *
 *
 * Output:
 *
 *    int - UNTAR_SUCCESSFUL (0)    on successful completion.
 *          UNTAR_INVALID_CHECKSUM  for an invalid header checksum.
 *          UNTAR_INVALID_HEADER    for an invalid header.
 *
 */
int
Untar_FromMemory(
  void   *tar_buf,
  size_t  size
)
{
  return Untar_FromMemory_Print(tar_buf, size, false);
}

/*
 * Function: Untar_FromFile
 *
 * Description:
 *
 *    This is a simple subroutine used to rip links, directories, and
 *    files out of a TAR file.
 *
 * Inputs:
 *
 *    const char *tar_name   - TAR filename.
 *
 * Output:
 *
 *    int - UNTAR_SUCCESSFUL (0)    on successful completion.
 *          UNTAR_INVALID_CHECKSUM  for an invalid header checksum.
 *          UNTAR_INVALID_HEADER    for an invalid header.
 */
int
Untar_FromFile_Print(
  const char          *tar_name,
  const rtems_printer *printer
)
{
  int            fd;
  char           *bufr;
  ssize_t        n;
  char           fname[100];
  int            retval;
  unsigned long  i;
  unsigned long  nblocks = 0;
  unsigned long  file_size = 0;
  unsigned long  mode = 0;
  unsigned char  linkflag = 0;

  retval = UNTAR_SUCCESSFUL;

  if ((fd = open(tar_name, O_RDONLY)) < 0) {
    return UNTAR_FAIL;
  }

  bufr = (char *)malloc(512);
  if (bufr == NULL) {
    close(fd);
    return(UNTAR_FAIL);
  }

  while (1) {
    /* Read the header */
    /* If the header read fails, we just consider it the end of the tarfile. */
    if ((n = read(fd, bufr, 512)) != 512) {
      break;
    }

    retval = Untar_ProcessHeader(bufr, fname, &mode, &file_size,
                                 &nblocks, &linkflag, printer);

    if (retval != UNTAR_SUCCESSFUL)
      break;

    if (linkflag == REGTYPE) {
      int out_fd;

      /*
       * Read out the data.  There are nblocks of data where nblocks
       * is the size rounded to the nearest 512-byte boundary.
       */

      if ((out_fd = creat(fname, mode)) == -1) {
        (void) lseek(fd, SEEK_CUR, 512UL * nblocks);
      } else {
        for (i = 0; i < nblocks; i++) {
          n = read(fd, bufr, 512);
          n = MIN(n, file_size - (i * 512UL));
          (void) write(out_fd, bufr, n);
        }
        close(out_fd);
      }
    }
  }

  free(bufr);
  close(fd);

  return retval;
}


void Untar_ChunkContext_Init(Untar_ChunkContext *context)
{
  context->state = UNTAR_CHUNK_HEADER;
  context->done_bytes = 0;
  context->out_fd = -1;
}

int Untar_FromChunk_Print(
  Untar_ChunkContext *context,
  void *chunk,
  size_t chunk_size,
  const rtems_printer* printer
)
{
  char *buf;
  size_t done;
  size_t todo;
  size_t remaining;
  size_t consume;
  int retval;
  unsigned char linkflag;

  buf = chunk;
  done = 0;
  todo = chunk_size;

  while (todo > 0) {
    switch (context->state) {
      case UNTAR_CHUNK_HEADER:
        remaining = 512 - context->done_bytes;
        consume = MIN(remaining, todo);
        memcpy(&context->header[context->done_bytes], &buf[done], consume);
        context->done_bytes += consume;

        if (context->done_bytes == 512) {
          retval = Untar_ProcessHeader(
            &context->header[0],
            &context->fname[0],
            &context->mode,
            &context->todo_bytes,
            &context->todo_blocks,
            &linkflag,
            printer
          );

          if (retval != UNTAR_SUCCESSFUL) {
            context->state = UNTAR_CHUNK_ERROR;
            return retval;
          }

          if (linkflag == REGTYPE) {
            context->out_fd = creat(&context->fname[0], context->mode);

            if (context->out_fd >= 0) {
              context->state = UNTAR_CHUNK_WRITE;
              context->done_bytes = 0;
            } else {
              context->state = UNTAR_CHUNK_SKIP;
              context->todo_bytes = 512 * context->todo_blocks;
              context->done_bytes = 0;
            }
          } else {
              context->done_bytes = 0;
          }
        }

        break;
      case UNTAR_CHUNK_SKIP:
        remaining = context->todo_bytes - context->done_bytes;
        consume = MIN(remaining, todo);
        context->done_bytes += consume;

        if (context->done_bytes == context->todo_bytes) {
          context->state = UNTAR_CHUNK_HEADER;
          context->done_bytes = 0;
        }

        break;
      case UNTAR_CHUNK_WRITE:
        remaining = context->todo_bytes - context->done_bytes;
        consume = MIN(remaining, todo);
        write(context->out_fd, &buf[done], consume);
        context->done_bytes += consume;

        if (context->done_bytes == context->todo_bytes) {
          close(context->out_fd);
          context->out_fd = -1;
          context->state = UNTAR_CHUNK_SKIP;
          context->todo_bytes = 512 * context->todo_blocks - context->todo_bytes;
          context->done_bytes = 0;
        }

        break;
      default:
        return UNTAR_FAIL;
    }

    done += consume;
    todo -= consume;
  }

  return UNTAR_SUCCESSFUL;
}

/*
 * Function: Untar_FromFile
 *
 * Description:
 *
 *    This is a simple subroutine used to rip links, directories, and
 *    files out of a TAR file.
 *
 * Inputs:
 *
 *    const char *tar_name   - TAR filename.
 *
 * Output:
 *
 *    int - UNTAR_SUCCESSFUL (0)    on successful completion.
 *          UNTAR_INVALID_CHECKSUM  for an invalid header checksum.
 *          UNTAR_INVALID_HEADER    for an invalid header.
 */
int
Untar_FromFile(
  const char *tar_name
)
{
  return Untar_FromFile_Print(tar_name, NULL);
}

/*
 * Compute the TAR checksum and check with the value in
 * the archive.  The checksum is computed over the entire
 * header, but the checksum field is substituted with blanks.
 */
int
_rtems_tar_header_checksum(
  const char *bufr
)
{
  int  i, sum;

  sum = 0;
  for (i=0; i<512; i++) {
    if ((i >= 148) && (i < 156))
      sum += 0xff & ' ';
    else
     sum += 0xff & bufr[i];
  }
  return(sum);
}
