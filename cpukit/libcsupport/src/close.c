/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief POSIX 1003.1b 6.3.1 - Close a File
 */

/*
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

#include <rtems/libio_.h>

int close(
  int  fd
)
{
  rtems_libio_t *iop;
  int            rc;

  if ( (uint32_t) fd >= rtems_libio_number_iops ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  LIBIO_GET_IOP( fd, iop );

  while ( true ) {
    unsigned int   flags;
    unsigned int desired;
    bool         success;
    flags = rtems_libio_iop_flags( iop );

    if ( ( flags & LIBIO_FLAGS_OPEN ) == 0 ) {
      rtems_libio_iop_drop( iop );
      rtems_set_errno_and_return_minus_one( EBADF );
    }

    /**
     * When LIBIO_FLAGS_CLOSE_BUSY is not set check that only one reference
     * is held to prevent closing while busy. Otherwise atomically check that
     * the flags match to ensure the reference count is maintained.
     */
    if ( ( flags & LIBIO_FLAGS_CLOSE_BUSY ) == 0 ) {
      flags &= LIBIO_FLAGS_FLAGS_MASK;
      flags |= LIBIO_FLAGS_REFERENCE_INC;
    }

    desired = flags & ~LIBIO_FLAGS_OPEN;
    success = _Atomic_Compare_exchange_uint(
      &iop->flags,
      &flags,
      desired,
      ATOMIC_ORDER_ACQ_REL,
      ATOMIC_ORDER_RELAXED
    );

    if ( success ) {
      break;
    }

    if ( ( flags & LIBIO_FLAGS_REFERENCE_MASK ) != LIBIO_FLAGS_REFERENCE_INC ) {
      rtems_libio_iop_drop( iop );
      rtems_set_errno_and_return_minus_one( EBUSY );
    }
  }

  rc = (*iop->pathinfo.handlers->close_h)( iop );

  rtems_libio_iop_drop( iop );

  return rc;
}

/*
 *  _close_r
 *
 *  This is the Newlib dependent reentrant version of close().
 */

#if defined(RTEMS_NEWLIB) && !defined(HAVE__CLOSE_R)

#include <reent.h>

int _close_r(
  struct _reent *ptr RTEMS_UNUSED,
  int            fd
)
{
  return close( fd );
}
#endif
