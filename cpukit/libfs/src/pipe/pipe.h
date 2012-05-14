/**
 * @file rtems/pipe.h
 *
 * This include file defines the interface to the POSIX FIFO/pipe file system
 * support.
 */

/*
 * Author: Wei Shen <cquark@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
  unsigned int Size;
  unsigned int Start;
  unsigned int Length;
  unsigned int Readers;
  unsigned int Writers;
  unsigned int waitingReaders;
  unsigned int waitingWriters;
  unsigned int readerCounter;     /* incremental counters */
  unsigned int writerCounter;     /* for differentiation of successive opens */
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
extern void pipe_release(
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
  pipe_control_t  *pipe,
  ioctl_command_t  cmd,
  void            *buffer,
  rtems_libio_t   *iop
);

#ifdef __cplusplus
}
#endif

#endif
