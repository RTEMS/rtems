/*
 * pipe.c: anonymous pipe
 *
 * Author: Wei Shen <cquark@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <stdio.h>
#include <fcntl.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>

/* Incremental number added to names of anonymous pipe files */
uint16_t rtems_pipe_no = 0;

/*
 * Called by pipe() to create an anonymous pipe.
 */
int pipe_create(
  int filsdes[2]
)
{
  rtems_filesystem_location_info_t loc;
  rtems_libio_t *iop;
  int err = 0;

  /* Create /tmp if not exists */
  if (rtems_filesystem_evaluate_path("/tmp", RTEMS_LIBIO_PERMS_RWX, &loc, TRUE)
      != 0) {
    if (errno != ENOENT)
      return -1;
    if (mkdir("/tmp", S_IRWXU|S_IRWXG|S_IRWXO|S_ISVTX) != 0)
      return -1;
  }
  else
    rtems_filesystem_freenode(&loc);

  /* /tmp/.fifoXXXX */
  char fifopath[15];
  memcpy(fifopath, "/tmp/.fifo", 10);
  sprintf(fifopath + 10, "%04x", rtems_pipe_no ++);

  /* Try creating FIFO file until find an available file name */
  while (mkfifo(fifopath, S_IRUSR|S_IWUSR) != 0) {
    if (errno != EEXIST)
      return -1;
    /* Just try once... */
    return -1;
    sprintf(fifopath + 10, "%04x", rtems_pipe_no ++);
  }

  /* Non-blocking open to avoid waiting for writers */
  filsdes[0] = open(fifopath, O_RDONLY | O_NONBLOCK);
  if (filsdes[0] < 0) {
    err = errno;
    goto out;
  }

  /* Reset open file to blocking mode */
  iop = rtems_libio_iop(filsdes[0]);
  iop->flags &= ~LIBIO_FLAGS_NO_DELAY;

  filsdes[1] = open(fifopath, O_WRONLY);

  if (filsdes[1] < 0) {
    err = errno;
    close(filsdes[0]);
  }

out:
  /* Delete file at errors, or else if pipe is successfully created
     the file node will be deleted after it is closed by all. */
  unlink(fifopath);

  if (! err)
    return 0;
  rtems_set_errno_and_return_minus_one(err);
}
