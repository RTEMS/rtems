/*
 *  lseek() - POSIX 1003.1b 6.5.3 - Reposition Read/Write File Offset
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

#include <unistd.h>

#include <rtems/libio_.h>

off_t lseek( int fd, off_t offset, int whence )
{
  off_t rv = 0;
  rtems_libio_t *iop;
  off_t reference_offset;
  off_t old_offset;
  off_t new_offset;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  old_offset = iop->offset;
  switch ( whence ) {
    case SEEK_SET:
      reference_offset = 0;
      break;
    case SEEK_CUR:
      reference_offset = old_offset;
      break;
    case SEEK_END:
      reference_offset = iop->size;
      break;
    default:
      errno = EINVAL;
      rv = (off_t) -1;
      break;
  }
  new_offset = reference_offset + offset;

  if ( rv == 0 ) {
    if (
      (reference_offset >= 0 && new_offset >= offset)
        || (reference_offset < 0 && new_offset <= offset)
    ) {
      switch ( rtems_filesystem_node_type( &iop->pathinfo ) ) {
        case RTEMS_FILESYSTEM_DIRECTORY:
        case RTEMS_FILESYSTEM_MEMORY_FILE:
          if ( new_offset < 0 ) {
            errno = EINVAL;
            rv = (off_t) -1;
          }
          break;
        default:
          break;
      }

      if ( rv == 0 ) {
        iop->offset = new_offset;
        rv = (*iop->pathinfo.handlers->lseek_h)( iop, offset, whence );
        if ( rv == (off_t) -1 ) {
          iop->offset = old_offset;
        }
      }
    } else {
      errno = EOVERFLOW;
      rv = (off_t) -1;
    }
  }

  return rv;
}

/*
 *  _lseek_r
 *
 *  This is the Newlib dependent reentrant version of lseek().
 */

#if defined(RTEMS_NEWLIB) && !defined(HAVE__LSEEK_R)

#include <reent.h>

off_t _lseek_r(
  struct _reent *ptr __attribute__((unused)),
  int            fd,
  off_t          offset,
  int            whence
)
{
  return lseek( fd, offset, whence );
}
#endif
