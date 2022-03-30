/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  fstat() - POSIX 1003.1b 5.6.2 - Get File Status
 *
 *  COPYRIGHT (c) 1989-1999.
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

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int fstat(
  int          fd,
  struct stat *sbuf
)
{
  rtems_libio_t *iop;
  int            rv;

  /*
   *  Check to see if we were passed a valid pointer.
   */
  if ( !sbuf )
    rtems_set_errno_and_return_minus_one( EFAULT );

  /*
   *  Now process the stat() request.
   */
  LIBIO_GET_IOP( fd, iop );

  /*
   *  Zero out the stat structure so the various support
   *  versions of stat don't have to.
   */
  memset( sbuf, 0, sizeof(struct stat) );

  rv = (*iop->pathinfo.handlers->fstat_h)( &iop->pathinfo, sbuf );
  rtems_libio_iop_drop( iop );
  return rv;
}

/*
 *  _fstat_r
 *
 *  This is the Newlib dependent reentrant version of fstat().
 */

#if defined(RTEMS_NEWLIB) && !defined(HAVE_FSTAT_R)

#include <reent.h>

int _fstat_r(
  struct _reent *ptr RTEMS_UNUSED,
  int            fd,
  struct stat   *buf
)
{
  return fstat( fd, buf );
}
#endif
