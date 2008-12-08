/*
 * pipe.h: header of POSIX FIFO/pipe
 *
 * Author: Wei Shen <cquark@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#ifndef _RTEMS_PIPE_H
#define _RTEMS_PIPE_H

#include <rtems/libio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Control block to manage each pipe */
typedef struct pipe_control {
  char *Buffer;
  uint Size;
  uint Start;
  uint Length;
  uint Readers;
  uint Writers;
  uint waitingReaders;
  uint waitingWriters;
  uint readerCounter;     /* incremental counters */
  uint writerCounter;     /* for differentiation of successive opens */
  rtems_id Semaphore;
  rtems_id readBarrier;   /* wait queues */
  rtems_id writeBarrier;
#if 0
  boolean Anonymous;      /* anonymous pipe or FIFO */
#endif
} pipe_control_t;

/*
 * Called by pipe() to create an anonymous pipe.
 */
extern int pipe_create(
  int filsdes[2]
);

/*
 * Interface to file system close.
 *
 * *pipep points to pipe control structure. When the last user releases pipe,
 * it will be set to NULL.
 */
extern int pipe_release(
  pipe_control_t **pipep,
  rtems_libio_t *iop
);

/*
 * Interface to file system open.
 *
 * *pipep points to pipe control structure. If called with *pipep = NULL,
 * fifo_open will try allocating and initializing a control structure. If the
 * call succeeds, *pipep will be set to address of new control structure.
 */
extern int fifo_open(
  pipe_control_t **pipep,
  rtems_libio_t *iop
);

/*
 * Interface to file system read.
 */
extern ssize_t pipe_read(
  pipe_control_t *pipe,
  void           *buffer,
  size_t          count,
  rtems_libio_t  *iop
);

/*
 * Interface to file system write.
 */
extern ssize_t pipe_write(
  pipe_control_t *pipe,
  const void     *buffer,
  size_t          count,
  rtems_libio_t  *iop
);

/*
 * Interface to file system ioctl.
 */
extern int pipe_ioctl(
  pipe_control_t *pipe,
  uint32_t        cmd,
  void           *buffer,
  rtems_libio_t  *iop
);

/*
 * Interface to file system lseek.
 */
extern int pipe_lseek(
  pipe_control_t *pipe,
  off_t           offset,
  int             whence,
  rtems_libio_t  *iop
);

/*
 * Initialization of FIFO/pipe module.
 */
extern void rtems_pipe_initialize (void);

#ifdef __cplusplus
}  
#endif

#endif
