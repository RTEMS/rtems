/*
 * pipe.c: anonymous pipe
 *
 * Author: Wei Shen <cquark@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <fcntl.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include <rtems/pipe.h>

/* Incremental number added to names of anonymous pipe files */
/* FIXME: This approach is questionable */
static uint16_t rtems_pipe_no = 0;

/*
 * Called by pipe() to create an anonymous pipe.
 */
int pipe_create(
  int filsdes[2]
)
{
  rtems_libio_t *iop;
  int err = 0;

  if (rtems_mkdir("/tmp", S_IRWXU | S_IRWXG | S_IRWXO) != 0)
    return -1;

  /* /tmp/.fifoXXXX */
  char fifopath[15];
  memcpy(fifopath, "/tmp/.fifo", 10);
  sprintf(fifopath + 10, "%04x", rtems_pipe_no ++);

  /* Try creating FIFO file until find an available file name */
  while (mkfifo(fifopath, S_IRUSR|S_IWUSR) != 0) {
    if (errno != EEXIST){
      return -1;
    }
    /* Just try once... */
    return -1;
    /* sprintf(fifopath + 10, "%04x", rtems_pipe_no ++); */
  }

  /* Non-blocking open to avoid waiting for writers */
  filsdes[0] = open(fifopath, O_RDONLY | O_NONBLOCK);
  if (filsdes[0] < 0) {
    err = errno;
    /* Delete file at errors, or else if pipe is successfully created
     the file node will be deleted after it is closed by all. */
    unlink(fifopath);
  }
  else {
  /* Reset open file to blocking mode */
    iop = rtems_libio_iop(filsdes[0]);
    iop->flags &= ~LIBIO_FLAGS_NO_DELAY;

    filsdes[1] = open(fifopath, O_WRONLY);

    if (filsdes[1] < 0) {
    err = errno;
    close(filsdes[0]);
    }
  unlink(fifopath);
  }
  if(err != 0)
    rtems_set_errno_and_return_minus_one(err);
  return 0;
}

