/**
 * @file
 *
 * @brief Untar an Image
 *
 * @ingroup libmisc_untar_img Untar Image
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

static int _rtems_tar_header_checksum(const char *bufr);

/*
 * This converts octal ASCII number representations into an
 * unsigned long.  Only support 32-bit numbers for now.
 */
static unsigned long
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
 * Make the directory path for a file if it does not exist.
 */
static int
Make_Path(const rtems_printer *printer, char *path)
{
  char *p;

  /*
   * Skip leading path separators.
   */
  while (*path == '/') {
    ++path;
  }

  p = path;

  for (; ; ++p) {
    if (p[0] == '\0') {
      return 0;
    } else if (p[0] != '/') {
      continue;
    }

    *p = '\0';
    if (p[1] == '\0') {
      /* Speculatively unlink the last component so that it can be re-created */
      unlink(path);
      return 0;
    }

    if (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
      if (errno == EEXIST || errno == EISDIR) {
        struct stat sb;

        if (stat(path, &sb) != 0) {
          return -1;
        }

        if (!S_ISDIR(sb.st_mode)) {
          if (unlink(path) != 0) {
            Print_Error(printer, "unlink", path);
            return -1;
          }

          if (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
            Print_Error(printer, "mkdir (unlink)", path);
            return -1;
          }
        }
      }
    }

    *p = '/';
  }

  return 0;
}

int
Untar_ProcessHeader(
  Untar_HeaderContext *ctx,
  const char          *bufr
)
{
  int sum;
  int hdr_chksum;
  int retval = UNTAR_SUCCESSFUL;
  int r;

  ctx->file_name[0] = '\0';
  ctx->file_size = 0;
  ctx->nblocks = 0;
  ctx->linkflag = -1;

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
    rtems_printf(ctx->printer, "untar: file header checksum error\n");
    return UNTAR_INVALID_CHECKSUM;
  }

  strlcpy(ctx->file_name, bufr, UNTAR_FILE_NAME_SIZE);

  ctx->mode = strtoul(&bufr[100], NULL, 8);

  ctx->linkflag   = bufr[156];
  ctx->file_size = _rtems_octal2ulong(&bufr[124], 12);

  /*
   * We've decoded the header, now figure out what it contains and do something
   * with it.
   */

  if (Make_Path(ctx->printer, ctx->file_path) != 0) {
    retval = UNTAR_FAIL;
  }

  if (ctx->linkflag == SYMTYPE) {
    strlcpy(ctx->link_name, &bufr[157], sizeof(ctx->link_name));
    rtems_printf(ctx->printer, "untar: symlink: %s -> %s\n",
                 ctx->link_name, ctx->file_path);
    r = symlink(ctx->link_name, ctx->file_path);
    if (r != 0) {
      Print_Error(ctx->printer, "symlink", ctx->file_path);
      retval = UNTAR_FAIL;
    }
  } else if (ctx->linkflag == REGTYPE) {
    rtems_printf(ctx->printer, "untar: file: %s (s:%lu,m:%04lo)\n",
                 ctx->file_path, ctx->file_size, ctx->mode);
    ctx->nblocks = (((ctx->file_size) + 511) & ~511) / 512;
  } else if (ctx->linkflag == DIRTYPE) {
    rtems_printf(ctx->printer, "untar: dir: %s\n", ctx->file_path);
    r = mkdir(ctx->file_path, ctx->mode);
    if (r != 0) {
      Print_Error(ctx->printer, "mkdir", ctx->file_path);
      retval = UNTAR_FAIL;
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
  int                  fd;
  const char          *tar_ptr = (const char *)tar_buf;
  const char          *bufr;
  char                 buf[UNTAR_FILE_NAME_SIZE];
  Untar_HeaderContext  ctx;
  int                  retval = UNTAR_SUCCESSFUL;
  unsigned long        ptr;

  ctx.file_path = buf;
  ctx.file_name = buf;
  ctx.printer = printer;
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

    retval = Untar_ProcessHeader(&ctx, bufr);

    if (retval != UNTAR_SUCCESSFUL)
      break;

    if (ctx.linkflag == REGTYPE) {
      if ((fd = open(ctx.file_path,
                     O_TRUNC | O_CREAT | O_WRONLY, ctx.mode)) == -1) {
        Print_Error(printer, "open", ctx.file_path);
        ptr += 512 * ctx.nblocks;
      } else {
        unsigned long sizeToGo = ctx.file_size;
        ssize_t       len;
        ssize_t       i;
        ssize_t       n;

        /*
         * Read out the data.  There are nblocks of data where nblocks is the
         * file_size rounded to the nearest 512-byte boundary.
         */
        for (i = 0; i < ctx.nblocks; i++) {
          len = ((sizeToGo < 512L) ? (sizeToGo) : (512L));
          n = write(fd, &tar_ptr[ptr], len);
          if (n != len) {
            Print_Error(printer, "write", ctx.file_path);
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
  int                  fd;
  char                *bufr;
  ssize_t              n;
  int                  retval;
  unsigned long        i;
  char                 buf[UNTAR_FILE_NAME_SIZE];
  Untar_HeaderContext  ctx;

  retval = UNTAR_SUCCESSFUL;

  if ((fd = open(tar_name, O_RDONLY)) < 0) {
    return UNTAR_FAIL;
  }

  bufr = (char *)malloc(512);
  if (bufr == NULL) {
    close(fd);
    return(UNTAR_FAIL);
  }

  ctx.file_path = buf;
  ctx.file_name = buf;
  ctx.printer = printer;

  while (1) {
    /* Read the header */
    /* If the header read fails, we just consider it the end of the tarfile. */
    if ((n = read(fd, bufr, 512)) != 512) {
      break;
    }

    retval = Untar_ProcessHeader(&ctx, bufr);

    if (retval != UNTAR_SUCCESSFUL)
      break;

    if (ctx.linkflag == REGTYPE) {
      int out_fd;

      /*
       * Read out the data.  There are nblocks of data where nblocks
       * is the size rounded to the nearest 512-byte boundary.
       */

      if ((out_fd = creat(ctx.file_path, ctx.mode)) == -1) {
        (void) lseek(fd, SEEK_CUR, 512UL * ctx.nblocks);
      } else {
        for (i = 0; i < ctx.nblocks; i++) {
          n = read(fd, bufr, 512);
          n = MIN(n, ctx.file_size - (i * 512UL));
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
  context->base.file_path = context->buf;
  context->base.file_name = context->buf;
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

  buf = chunk;
  done = 0;
  todo = chunk_size;

  context->base.printer = printer;

  while (todo > 0) {
    switch (context->state) {
      case UNTAR_CHUNK_HEADER:
        remaining = 512 - context->done_bytes;
        consume = MIN(remaining, todo);
        memcpy(&context->header[context->done_bytes], &buf[done], consume);
        context->done_bytes += consume;

        if (context->done_bytes == 512) {
          retval = Untar_ProcessHeader(
            &context->base,
            &context->header[0]
          );

          if (retval != UNTAR_SUCCESSFUL) {
            context->state = UNTAR_CHUNK_ERROR;
            return retval;
          }

          if (context->base.linkflag == REGTYPE) {
            context->out_fd = creat(context->base.file_path,
                                    context->base.mode);

            if (context->out_fd >= 0) {
              context->state = UNTAR_CHUNK_WRITE;
              context->done_bytes = 0;
            } else {
              context->state = UNTAR_CHUNK_SKIP;
              context->base.file_size = 512 * context->base.nblocks;
              context->done_bytes = 0;
            }
          } else {
              context->done_bytes = 0;
          }
        }

        break;
      case UNTAR_CHUNK_SKIP:
        remaining = context->base.file_size - context->done_bytes;
        consume = MIN(remaining, todo);
        context->done_bytes += consume;

        if (context->done_bytes == context->base.file_size) {
          context->state = UNTAR_CHUNK_HEADER;
          context->done_bytes = 0;
        }

        break;
      case UNTAR_CHUNK_WRITE:
        remaining = context->base.file_size - context->done_bytes;
        consume = MIN(remaining, todo);
        write(context->out_fd, &buf[done], consume);
        context->done_bytes += consume;

        if (context->done_bytes == context->base.file_size) {
          close(context->out_fd);
          context->out_fd = -1;
          context->state = UNTAR_CHUNK_SKIP;
          context->base.file_size = 512 * context->base.nblocks
            - context->base.file_size;
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
static int
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
