/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS RFS File Handlers
 *
 * This file contains the set of handlers used to process operations on
 * RFS file nodes.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#if SIZEOF_OFF_T == 8
#define PRIdoff_t PRId64
#elif SIZEOF_OFF_T == 4
#define PRIdoff_t PRId32
#else
#error "unsupported size of off_t"
#endif

#include <rtems/rfs/rtems-rfs-file.h>
#include "rtems-rfs-rtems.h"

/**
 * This routine processes the open() system call.  Note that there is nothing
 * special to be done at open() time.
 */

static int
rtems_rfs_rtems_file_open (rtems_libio_t* iop,
                           const char*    pathname,
                           int            oflag,
                           mode_t         mode)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (&iop->pathinfo);
  rtems_rfs_ino          ino;
  rtems_rfs_file_handle* file;
  int                    flags;
  int                    rc;

  flags = 0;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_FILE_OPEN))
    printf("rtems-rfs: file-open: path:%s ino:%" PRId32 " flags:%04i mode:%04" PRIu32 "\n",
           pathname, ino, flags, mode);

  rtems_rfs_rtems_lock (fs);

  ino = rtems_rfs_rtems_get_iop_ino (iop);

  rc = rtems_rfs_file_open (fs, ino, flags, &file);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("file-open: open", rc);
  }

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_FILE_OPEN))
    printf("rtems-rfs: file-open: handle:%p\n", file);

  rtems_rfs_rtems_set_iop_file_handle (iop, file);

  rtems_rfs_rtems_unlock (fs);
  return 0;
}

/**
 * This routine processes the close() system call.  Note that there is nothing
 * to flush at this point.
 *
 * @param iop
 * @return int
 */
static int
rtems_rfs_rtems_file_close (rtems_libio_t* iop)
{
  rtems_rfs_file_handle* file = rtems_rfs_rtems_get_iop_file_handle (iop);
  rtems_rfs_file_system* fs = rtems_rfs_file_fs (file);
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_FILE_CLOSE))
    printf("rtems-rfs: file-close: handle:%p\n", file);

  rtems_rfs_rtems_lock (fs);

  rc = rtems_rfs_file_close (fs, file);
  if (rc > 0)
    rc = rtems_rfs_rtems_error ("file-close: file close", rc);

  rtems_rfs_rtems_unlock (fs);
  return rc;
}

/**
 * This routine processes the read() system call.
 *
 * @param iop
 * @param buffer
 * @param count
 * @return int
 */
static ssize_t
rtems_rfs_rtems_file_read (rtems_libio_t* iop,
                           void*          buffer,
                           size_t         count)
{
  rtems_rfs_file_handle* file = rtems_rfs_rtems_get_iop_file_handle (iop);
  rtems_rfs_pos          pos;
  uint8_t*               data = buffer;
  ssize_t                read = 0;
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_FILE_READ))
    printf("rtems-rfs: file-read: handle:%p count:%zd\n", file, count);

  rtems_rfs_rtems_lock (rtems_rfs_file_fs (file));

  pos = iop->offset;

  if (pos < rtems_rfs_file_size (file))
  {
    while (count)
    {
      size_t size;

      rc = rtems_rfs_file_io_start (file, &size, true);
      if (rc > 0)
      {
        read = rtems_rfs_rtems_error ("file-read: read: io-start", rc);
        break;
      }

      if (size == 0)
        break;

      if (size > count)
        size = count;

      memcpy (data, rtems_rfs_file_data (file), size);

      data  += size;
      count -= size;
      read  += size;

      rc = rtems_rfs_file_io_end (file, size, true);
      if (rc > 0)
      {
        read = rtems_rfs_rtems_error ("file-read: read: io-end", rc);
        break;
      }
    }
  }

  if (read >= 0)
    iop->offset = pos + read;

  rtems_rfs_rtems_unlock (rtems_rfs_file_fs (file));

  return read;
}

/**
 * This routine processes the write() system call.
 *
 * @param iop
 * @param buffer
 * @param count
 * @return ssize_t
 */
static ssize_t
rtems_rfs_rtems_file_write (rtems_libio_t* iop,
                            const void*    buffer,
                            size_t         count)
{
  rtems_rfs_file_handle* file = rtems_rfs_rtems_get_iop_file_handle (iop);
  rtems_rfs_pos          pos;
  rtems_rfs_pos          file_size;
  const uint8_t*         data = buffer;
  ssize_t                write = 0;
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_FILE_WRITE))
    printf("rtems-rfs: file-write: handle:%p count:%zd\n", file, count);

  rtems_rfs_rtems_lock (rtems_rfs_file_fs (file));

  pos = iop->offset;
  file_size = rtems_rfs_file_size (file);
  if (pos > file_size)
  {
    /*
     * If the iop position is past the physical end of the file we need to set
     * the file size to the new length before writing.  The
     * rtems_rfs_file_io_end() will grow the file subsequently.
     */
    rc = rtems_rfs_file_set_size (file, pos);
    if (rc)
    {
      rtems_rfs_rtems_unlock (rtems_rfs_file_fs (file));
      return rtems_rfs_rtems_error ("file-write: write extend", rc);
    }

    rtems_rfs_file_set_bpos (file, pos);
  }
  else if (pos < file_size && (iop->flags & LIBIO_FLAGS_APPEND) != 0)
  {
    pos = file_size;
    rc = rtems_rfs_file_seek (file, pos, &pos);
    if (rc)
    {
      rtems_rfs_rtems_unlock (rtems_rfs_file_fs (file));
      return rtems_rfs_rtems_error ("file-write: write append seek", rc);
    }
  }

  while (count)
  {
    size_t size = count;

    rc = rtems_rfs_file_io_start (file, &size, false);
    if (rc)
    {
      write = rtems_rfs_rtems_error ("file-write: write open", rc);
      break;
    }

    if (size > count)
      size = count;

    memcpy (rtems_rfs_file_data (file), data, size);

    data  += size;
    count -= size;
    write  += size;

    rc = rtems_rfs_file_io_end (file, size, false);
    if (rc)
    {
      write = rtems_rfs_rtems_error ("file-write: write close", rc);
      break;
    }
  }

  if (write >= 0)
    iop->offset = pos + write;

  rtems_rfs_rtems_unlock (rtems_rfs_file_fs (file));

  return write;
}

/**
 * This routine processes the lseek() system call.
 *
 * @param iop
 * @param offset
 * @param whence
 * @return off_t
 */
static off_t
rtems_rfs_rtems_file_lseek (rtems_libio_t* iop,
                            off_t          offset,
                            int            whence)
{
  rtems_rfs_file_handle* file = rtems_rfs_rtems_get_iop_file_handle (iop);
  off_t                  old_offset;
  off_t                  new_offset;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_FILE_LSEEK))
    printf("rtems-rfs: file-lseek: handle:%p offset:%" PRIdoff_t "\n", file, offset);

  rtems_rfs_rtems_lock (rtems_rfs_file_fs (file));

  old_offset = iop->offset;
  new_offset = rtems_filesystem_default_lseek_file (iop, offset, whence);
  if (new_offset != -1)
  {
    rtems_rfs_pos pos = iop->offset;
    int           rc = rtems_rfs_file_seek (file, pos, &pos);

    if (rc)
    {
      rtems_rfs_rtems_error ("file_lseek: lseek", rc);
      iop->offset = old_offset;
      new_offset = -1;
    }
  }

  rtems_rfs_rtems_unlock (rtems_rfs_file_fs (file));

  return new_offset;
}

/**
 * This routine processes the ftruncate() system call.
 *
 * @param iop
 * @param length
 * @return int
 */
static int
rtems_rfs_rtems_file_ftruncate (rtems_libio_t* iop,
                                off_t          length)
{
  rtems_rfs_file_handle* file = rtems_rfs_rtems_get_iop_file_handle (iop);
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_FILE_FTRUNC))
    printf("rtems-rfs: file-ftrunc: handle:%p length:%" PRIdoff_t "\n", file, length);

  rtems_rfs_rtems_lock (rtems_rfs_file_fs (file));

  rc = rtems_rfs_file_set_size (file, length);
  if (rc)
    rc = rtems_rfs_rtems_error ("file_ftruncate: set size", rc);

  rtems_rfs_rtems_unlock (rtems_rfs_file_fs (file));

  return rc;
}

/*
 *  Set of operations handlers for operations on RFS files.
 */

const rtems_filesystem_file_handlers_r rtems_rfs_rtems_file_handlers = {
  .open_h      = rtems_rfs_rtems_file_open,
  .close_h     = rtems_rfs_rtems_file_close,
  .read_h      = rtems_rfs_rtems_file_read,
  .write_h     = rtems_rfs_rtems_file_write,
  .ioctl_h     = rtems_filesystem_default_ioctl,
  .lseek_h     = rtems_rfs_rtems_file_lseek,
  .fstat_h     = rtems_rfs_rtems_fstat,
  .ftruncate_h = rtems_rfs_rtems_file_ftruncate,
  .fsync_h     = rtems_rfs_rtems_fdatasync,
  .fdatasync_h = rtems_rfs_rtems_fdatasync,
  .fcntl_h     = rtems_filesystem_default_fcntl
};
