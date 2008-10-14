/*
 * imfs_fifo.c: FIFO support for IMFS
 *
 * Author: Wei Shen <cquark@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>

#include "imfs.h"

#define JNODE2PIPE(_jnode)  ( (_jnode)->info.fifo.pipe )

#define LIBIO2PIPE(_iop)  ( JNODE2PIPE((IMFS_jnode_t *)(_iop)->file_info) )

/* Set errno and return -1 if error, else return _err */
#define IMFS_FIFO_RETURN(_err) \
do {  \
  if (_err < 0)	\
    rtems_set_errno_and_return_minus_one(-_err); \
  return _err; \
} while (0)

int IMFS_fifo_open(
  rtems_libio_t *iop,
  const char    *pathname,
  uint32_t       flag,
  uint32_t       mode
)
{
  IMFS_jnode_t *jnode = iop->file_info;

  int err = fifo_open(&JNODE2PIPE(jnode), iop);
  IMFS_FIFO_RETURN(err);
}

int IMFS_fifo_close(
  rtems_libio_t *iop
)
{
  IMFS_jnode_t *jnode = iop->file_info;

  int err = pipe_release(&JNODE2PIPE(jnode), iop);

  if (! err) {
    iop->flags &= ~LIBIO_FLAGS_OPEN;
    /* Free jnode if file is already unlinked and no one opens it */
    if (! rtems_libio_is_file_open(jnode) && jnode->st_nlink < 1)
      free(jnode);
  }

  IMFS_FIFO_RETURN(err);
}

ssize_t IMFS_fifo_read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
)
{
  IMFS_jnode_t *jnode = iop->file_info;

  int err = pipe_read(JNODE2PIPE(jnode), buffer, count, iop);
  if (err > 0)
    IMFS_update_atime(jnode);

  IMFS_FIFO_RETURN(err);
}

ssize_t IMFS_fifo_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  IMFS_jnode_t *jnode = iop->file_info;

  int err = pipe_write(JNODE2PIPE(jnode), buffer, count, iop);
  if (err > 0) {
    IMFS_mtime_ctime_update(jnode);
  }

  IMFS_FIFO_RETURN(err);
}

int IMFS_fifo_ioctl(
  rtems_libio_t *iop,
  uint32_t       command,
  void          *buffer
)
{
  int err;

  if (command == FIONBIO) {
    if (buffer == NULL)
      err = -EFAULT;
    else {
      if (*(int *)buffer)
        iop->flags |= LIBIO_FLAGS_NO_DELAY;
      else
        iop->flags &= ~LIBIO_FLAGS_NO_DELAY;
      return 0;
    }
  }
  else
    err = pipe_ioctl(LIBIO2PIPE(iop), command, buffer, iop);

  IMFS_FIFO_RETURN(err);
}

int IMFS_fifo_lseek(
  rtems_libio_t *iop,
  off_t         offset,
  int           whence
)
{
  int err = pipe_lseek(LIBIO2PIPE(iop), offset, whence, iop);
  IMFS_FIFO_RETURN(err);
}

/*
 *  Handler table for IMFS FIFO nodes
 */

const rtems_filesystem_file_handlers_r IMFS_fifo_handlers = {
  IMFS_fifo_open,
  IMFS_fifo_close,
  IMFS_fifo_read,
  IMFS_fifo_write,
  IMFS_fifo_ioctl,
  IMFS_fifo_lseek,
  IMFS_stat,
  IMFS_fchmod,
  NULL,    /* ftruncate */
  NULL,    /* fpathconf */
  NULL,    /* fsync */
  NULL,    /* fdatasync */
  IMFS_fcntl,
  IMFS_rmnod,
};
