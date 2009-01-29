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
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>               /* libio_.h pulls in rtems */
#include <rtems.h>
#include <rtems/assoc.h>                /* assoc.h not included by rtems.h */

#include <stdio.h>                      /* O_RDONLY, et.al. */
#include <fcntl.h>                      /* O_RDONLY, et.al. */
#include <assert.h>
#include <errno.h>

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

#include <errno.h>
#include <string.h>                     /* strcmp */
#include <unistd.h>
#include <stdlib.h>                     /* calloc() */

#include <rtems/libio.h>                /* libio.h not pulled in by rtems */

/*
 *  File descriptor Table Information
 */

extern uint32_t       rtems_libio_number_iops;
extern rtems_id       rtems_libio_semaphore;
extern rtems_libio_t *rtems_libio_iops;
extern rtems_libio_t *rtems_libio_iop_freelist;

/*
 *  rtems_libio_fcntl_flags
 *
 *  Convert UNIX fnctl(2) flags to ones that RTEMS drivers understand
 */

const rtems_assoc_t access_modes_assoc[] = {
  { "READ",       LIBIO_FLAGS_READ,  O_RDONLY },
  { "WRITE",      LIBIO_FLAGS_WRITE, O_WRONLY },
  { "READ/WRITE", LIBIO_FLAGS_READ_WRITE, O_RDWR },
  { 0, 0, 0 },
};

const rtems_assoc_t status_flags_assoc[] = {
#ifdef ACCEPT_O_NDELAY_ALIAS
  { "NO DELAY",  LIBIO_FLAGS_NO_DELAY,  O_NDELAY },
#endif
  { "NONBLOCK",  LIBIO_FLAGS_NO_DELAY,  O_NONBLOCK },
  { "APPEND",    LIBIO_FLAGS_APPEND,    O_APPEND },
  { "CREATE",    LIBIO_FLAGS_CREATE,    O_CREAT },
  { 0, 0, 0 },
};

uint32_t   rtems_libio_fcntl_flags(
  uint32_t   fcntl_flags
)
{
  uint32_t   flags = 0;
  uint32_t   access_modes;

  /*
   * Access mode is a small integer
   */

  access_modes = fcntl_flags & O_ACCMODE;
  fcntl_flags &= ~O_ACCMODE;
  flags = rtems_assoc_local_by_remote( access_modes_assoc, access_modes );

  /*
   * Everything else is single bits
   */

  flags |=
     rtems_assoc_local_by_remote_bitfield(status_flags_assoc, fcntl_flags);
  return flags;
}

/*
 *  rtems_libio_to_fcntl_flags
 *
 *  Convert RTEMS internal flags to UNIX fnctl(2) flags
 */

uint32_t   rtems_libio_to_fcntl_flags(
  uint32_t   flags
)
{
  uint32_t   fcntl_flags = 0;

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
  rtems_libio_t *iop, *next;
  rtems_status_code rc;
  rtems_id sema;

  rtems_semaphore_obtain( rtems_libio_semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT );

  if (rtems_libio_iop_freelist) {
    rc = rtems_semaphore_create(
      RTEMS_LIBIO_IOP_SEM(rtems_libio_iop_freelist - rtems_libio_iops),
      1,
      RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
      RTEMS_NO_PRIORITY,
      &sema
    );
    if (rc != RTEMS_SUCCESSFUL)
      goto failed;
    iop = rtems_libio_iop_freelist;
    next = iop->data1;
    (void) memset( iop, 0, sizeof(rtems_libio_t) ); 
    iop->flags = LIBIO_FLAGS_OPEN;
    iop->sem = sema;
    rtems_libio_iop_freelist = next;
    goto done;
  }

failed:
  iop = 0;

done:
  rtems_semaphore_release( rtems_libio_semaphore );
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
  rtems_semaphore_obtain( rtems_libio_semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT );

    if (iop->sem)
      rtems_semaphore_delete(iop->sem);

    iop->flags &= ~LIBIO_FLAGS_OPEN;
    iop->data1 = rtems_libio_iop_freelist;
    rtems_libio_iop_freelist = iop;

  rtems_semaphore_release(rtems_libio_semaphore);
}

/*
 *  rtems_libio_is_open_files_in_fs
 *
 *  This routine scans the entire file descriptor table to determine if the
 *  are any active file descriptors that refer to the at least one node in the
 *  file system that we are trying to dismount.
 *
 *  If there is at least one node in the file system referenced by the mount
 *  table entry a 1 is returned, otherwise a 0 is returned.
 */

int rtems_libio_is_open_files_in_fs(
  rtems_filesystem_mount_table_entry_t * fs_mt_entry
)
{
  rtems_libio_t     *iop;
  int                result = 0;
  int                i;

  rtems_semaphore_obtain( rtems_libio_semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT );

  /*
   *  Look for any active file descriptor entry.
   */

  for (iop=rtems_libio_iops,i=0; i < rtems_libio_number_iops; iop++, i++){

    if ((iop->flags & LIBIO_FLAGS_OPEN) != 0) {

       /*
        *  Check if this node is under the file system that we
        *  are trying to dismount.
        */

       if ( iop->pathinfo.mt_entry == fs_mt_entry ) {
          result = 1;
          break;
       }
    }
  }

  rtems_semaphore_release( rtems_libio_semaphore );

  return result;
}

/*
 *  rtems_libio_is_file_open
 *
 *  This routine scans the entire file descriptor table to determine if the
 *  given file refers to an active file descriptor.
 *
 *  If the given file is open a 1 is returned, otherwise a 0 is returned.
 */

int rtems_libio_is_file_open(
  void         *node_access
)
{
  rtems_libio_t     *iop;
  int                result=0;
  int                i;

  rtems_semaphore_obtain( rtems_libio_semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT );

  /*
   *  Look for any active file descriptor entry.
   */

 for (iop=rtems_libio_iops,i=0; i < rtems_libio_number_iops; iop++, i++){
    if ((iop->flags & LIBIO_FLAGS_OPEN) != 0) {

       /*
        *  Check if this node is under the file system that we
        *  are trying to dismount.
        */

       if ( iop->pathinfo.node_access == node_access ) {
          result = 1;
          break;
       }
    }
  }

  rtems_semaphore_release( rtems_libio_semaphore );

  return result;
}
