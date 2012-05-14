/*
 *  This file contains the support infrastructure used to manage the
 *  table of integer style file descriptors used by the low level
 *  POSIX system calls like open(), read, fstat(), etc.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
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

/*
 *  rtems_libio_fcntl_flags
 *
 *  Convert UNIX fnctl(2) flags to ones that RTEMS drivers understand
 */

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
  { "CREATE",    LIBIO_FLAGS_CREATE,    O_CREAT },
  { 0, 0, 0 },
};

uint32_t rtems_libio_fcntl_flags( int fcntl_flags )
{
  uint32_t   flags = 0;
  uint32_t   access_modes;

  /*
   * Access mode is a small integer
   */

  access_modes = (uint32_t) (fcntl_flags & O_ACCMODE);
  fcntl_flags &= ~O_ACCMODE;
  flags = rtems_assoc_local_by_remote( access_modes_assoc, access_modes );

  /*
   * Everything else is single bits
   */

  flags |= rtems_assoc_local_by_remote_bitfield(
    status_flags_assoc,
    (uint32_t) fcntl_flags
  );

  return flags;
}

/*
 *  rtems_libio_to_fcntl_flags
 *
 *  Convert RTEMS internal flags to UNIX fnctl(2) flags
 */

int rtems_libio_to_fcntl_flags( uint32_t flags )
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

  if ( (flags & LIBIO_FLAGS_CREATE) == LIBIO_FLAGS_CREATE ) {
    fcntl_flags |= O_CREAT;
  }

  return fcntl_flags;
}

/*
 *  rtems_libio_allocate
 *
 *  This routine searches the IOP Table for an unused entry.  If it
 *  finds one, it returns it.  Otherwise, it returns NULL.
 */

rtems_libio_t *rtems_libio_allocate( void )
{
  rtems_libio_t *iop = NULL;

  rtems_libio_lock();

  if (rtems_libio_iop_freelist) {
    iop = rtems_libio_iop_freelist;
    rtems_libio_iop_freelist = iop->data1;
    memset( iop, 0, sizeof(*iop) );
    iop->flags = LIBIO_FLAGS_OPEN;
  }

  rtems_libio_unlock();

  return iop;
}

/*
 *  rtems_libio_free
 *
 *  This routine frees the resources associated with an IOP (file descriptor)
 *  and clears the slot in the IOP Table.
 */

void rtems_libio_free(
  rtems_libio_t *iop
)
{
  rtems_filesystem_location_free( &iop->pathinfo );

  rtems_libio_lock();

    iop->flags &= ~LIBIO_FLAGS_OPEN;
    iop->data1 = rtems_libio_iop_freelist;
    rtems_libio_iop_freelist = iop;

  rtems_libio_unlock();
}
