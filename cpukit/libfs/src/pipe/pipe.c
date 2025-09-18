/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup FIFO_PIPE
 *
 * @brief Create an Anonymous Pipe
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include <rtems/pipe.h>

/* Incremental number added to names of anonymous pipe files */
/* FIXME: This approach is questionable */
static uint16_t rtems_pipe_no = 0;

int pipe(
  int filsdes[2]
)
{
  rtems_libio_t *iop;
  int err = 0;

  if (filsdes == NULL)
    rtems_set_errno_and_return_minus_one( EFAULT );

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
    rtems_libio_iop_flags_clear( iop, LIBIO_FLAGS_NO_DELAY );

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

