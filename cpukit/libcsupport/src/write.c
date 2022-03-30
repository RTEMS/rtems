/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Write to a File
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/libio_.h>
#include <rtems/seterr.h>

/**
 *  POSIX 1003.1b 6.4.2 - Write to a File
 * 
 *  This routine writes count bytes from from buffer pointed to by buffer
 *  to the file associated with the open file descriptor, fildes.
 */
ssize_t write(
  int         fd,
  const void *buffer,
  size_t      count
)
{
  rtems_libio_t *iop;
  ssize_t        n;

  rtems_libio_check_buffer( buffer );
  rtems_libio_check_count( count );

  LIBIO_GET_IOP_WITH_ACCESS( fd, iop, LIBIO_FLAGS_WRITE, EBADF );

  /*
   *  Now process the write() request.
   */
  n = (*iop->pathinfo.handlers->write_h)( iop, buffer, count );
  rtems_libio_iop_drop( iop );
  return n;
}
