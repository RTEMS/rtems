/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief File Descriptor Routines
 *  @ingroup LibIOInternal
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/libio_.h>
#include <rtems/assoc.h>

/* define this to alias O_NDELAY to  O_NONBLOCK, i.e.,
 * O_NDELAY is accepted on input but fcntl(F_GETFL) returns
 * O_NONBLOCK. This is because rtems has no distinction
 * between the two (but some systems have).
 * Note that accepting this alias creates a problem:
 * an application trying to clear the non-blocking flag
 * using a
 *
 *    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NDELAY);
 *
 * does (silently) ignore the operation.
 */
#undef ACCEPT_O_NDELAY_ALIAS

static const rtems_assoc_t access_modes_assoc[] = {
  { "READ",       LIBIO_FLAGS_READ,  O_RDONLY },
  { "WRITE",      LIBIO_FLAGS_WRITE, O_WRONLY },
  { "READ/WRITE", LIBIO_FLAGS_READ_WRITE, O_RDWR },
  { 0, 0, 0 },
};

static const rtems_assoc_t status_flags_assoc[] = {
#ifdef ACCEPT_O_NDELAY_ALIAS
  { "NO DELAY",  LIBIO_FLAGS_NO_DELAY,  O_NDELAY },
#endif
  { "NONBLOCK",  LIBIO_FLAGS_NO_DELAY,  O_NONBLOCK },
  { "APPEND",    LIBIO_FLAGS_APPEND,    O_APPEND },
  { 0, 0, 0 },
};

unsigned int rtems_libio_from_fcntl_flags( int fcntl_flags )
{
  unsigned int   flags = 0;
  uint32_t   access_modes;

  /*
   * Access mode is a small integer
   */

  access_modes = (unsigned int) (fcntl_flags & O_ACCMODE);
  fcntl_flags &= ~O_ACCMODE;
  flags = rtems_assoc_local_by_remote( access_modes_assoc, access_modes );

  /*
   * Everything else is single bits
   */

  flags |= (unsigned int ) rtems_assoc_local_by_remote_bitfield(
    status_flags_assoc,
    (uint32_t) fcntl_flags
  );

  return flags;
}

int rtems_libio_to_fcntl_flags( unsigned int flags )
{
  int fcntl_flags = 0;

  if ( (flags & LIBIO_FLAGS_READ_WRITE) == LIBIO_FLAGS_READ_WRITE ) {
    fcntl_flags |= O_RDWR;
  } else if ( (flags & LIBIO_FLAGS_READ) == LIBIO_FLAGS_READ) {
    fcntl_flags |= O_RDONLY;
  } else if ( (flags & LIBIO_FLAGS_WRITE) == LIBIO_FLAGS_WRITE) {
    fcntl_flags |= O_WRONLY;
  }

  if ( (flags & LIBIO_FLAGS_NO_DELAY) == LIBIO_FLAGS_NO_DELAY ) {
    fcntl_flags |= O_NONBLOCK;
  }

  if ( (flags & LIBIO_FLAGS_APPEND) == LIBIO_FLAGS_APPEND ) {
    fcntl_flags |= O_APPEND;
  }

  return fcntl_flags;
}

rtems_libio_t *rtems_libio_allocate( void )
{
  rtems_libio_t *iop;

  rtems_libio_lock();

  iop = rtems_libio_iop_free_head;

  if ( iop != NULL ) {
    void *next;

    rtems_libio_iop_flags_clear( iop, LIBIO_FLAGS_FREE );

    next = iop->data1;
    rtems_libio_iop_free_head = next;

    if ( next == NULL ) {
      rtems_libio_iop_free_tail = &rtems_libio_iop_free_head;
    }
  }

  rtems_libio_unlock();

  return iop;
}

void rtems_libio_free_iop(
  rtems_libio_t *iop
)
{
  size_t zero;

  rtems_libio_lock();

  if ( !rtems_libio_iop_is_free( iop ) ) {
    /*
     * Clear the flags. All references should have been dropped.
     */
    _Atomic_Store_uint( &iop->flags, LIBIO_FLAGS_FREE, ATOMIC_ORDER_RELAXED );

    rtems_filesystem_location_free( &iop->pathinfo );

    zero = offsetof( rtems_libio_t, offset );
    memset( (char *) iop + zero, 0, sizeof( *iop ) - zero );

    /*
     * Append it to the free list. This increases the likelihood that
     * a use after close is detected.
     */
    *rtems_libio_iop_free_tail = iop;
    rtems_libio_iop_free_tail = &iop->data1;
  }

  rtems_libio_unlock();
}

int rtems_libio_count_open_iops(
  void
)
{
  int open = 0;
  int i;
  /*
   * No locking needed to count the open iops
   */
  for (i = 0; i < rtems_libio_number_iops; ++i) {
    const rtems_libio_t *iop = &rtems_libio_iops[ i ];
    if (( rtems_libio_iop_flags( iop ) & LIBIO_FLAGS_OPEN ) != 0 ) {
      ++open;
    }
  }
  return open;
}
