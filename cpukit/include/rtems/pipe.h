/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief POSIX FIFO/pipe File System Support
 *
 * This include file defines the interface to the POSIX FIFO/pipe file system
 * support.
 */

/*
 * Copyright (C) 2008 Wei Shen <jshen.yn@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_PIPE_H
#define _RTEMS_PIPE_H

#include <rtems/libio.h>
#include <rtems/thread.h>

/**
 * @defgroup FIFO_PIPE FIFO/Pipe File System Support
 *
 * @ingroup FileSystemTypesAndMount
 *
 * @brief Interface to the POSIX FIFO/Pipe File System
 */
/**@{*/

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
  rtems_mutex Mutex;
  rtems_condition_variable readBarrier;   /* wait queues */
  rtems_condition_variable writeBarrier;
#if 0
  boolean Anonymous;      /* anonymous pipe or FIFO */
#endif
} pipe_control_t;

/**
 * @brief Release a pipe.
 *
 * Interface to file system close.
 *
 * *pipep points to pipe control structure. When the last user releases pipe,
 * it will be set to NULL.
 */
extern void pipe_release(
  pipe_control_t **pipep,
  rtems_libio_t *iop
);

/**
 * @brief File system open.
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

/**
 * @brief File system read.
 *
 * Interface to file system read.
 */
extern ssize_t pipe_read(
  pipe_control_t *pipe,
  void           *buffer,
  size_t          count,
  rtems_libio_t  *iop
);

/**
 * @brief File system write.
 *
 * Interface to file system write.
 */
extern ssize_t pipe_write(
  pipe_control_t *pipe,
  const void     *buffer,
  size_t          count,
  rtems_libio_t  *iop
);

/**
 * @brief File system Input/Output control.
 *
 * Interface to file system ioctl.
 */
extern int pipe_ioctl(
  pipe_control_t  *pipe,
  ioctl_command_t  cmd,
  void            *buffer,
  rtems_libio_t   *iop
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
