/*
 *   fcntl() - POSIX 1003.1b 6.5.2 - File Control
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <rtems.h>
#include "libio_.h"

int fcntl(
  int fd,
  int cmd,
  ...
)
{
  va_list        ap;
  rtems_libio_t *iop;
  rtems_libio_t *diop;
  int            fd2;
  int            flags;
  
  va_start( ap, cmd );

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  /*
   *  Now process the fcntl().
   */

  /*
   *  This switch should contain all the cases from POSIX.
   */

  switch ( cmd ) {
    case F_DUPFD:        /* dup */
      fd2 = va_arg( ap, int );
      if ( fd2 )
        diop = rtems_libio_iop( fd2 );
      else {
        /* allocate a file control block */
        diop = rtems_libio_allocate();
        if ( diop == 0 )
          return -1;
      }

      diop->handlers   = iop->handlers;
      diop->file_info  = iop->file_info;
      diop->flags      = iop->flags;
      diop->pathinfo   = iop->pathinfo;
      
      return 0;

    case F_GETFD:        /* get f_flags */
      if ( iop->flags & LIBIO_FLAGS_CLOSE_ON_EXEC )
        return 1;
      return 0;

    case F_SETFD:        /* set f_flags */
      /*
       *  Interpret the third argument as the "close on exec()" flag.
       *  If this argument is 1, then the file descriptor is to be closed
       *  if a new process is exec()'ed.  Since RTEMS does not support
       *  processes, then we can ignore this one except to make 
       *  F_GETFD work.
       */

      if ( va_arg( ap, int ) )
        iop->flags |= LIBIO_FLAGS_CLOSE_ON_EXEC;
      else
        iop->flags &= ~LIBIO_FLAGS_CLOSE_ON_EXEC;
      return 0;

    case F_GETFL:        /* more flags (cloexec) */
      return rtems_libio_to_fcntl_flags( iop->flags );

    case F_SETFL:
      flags = rtems_libio_fcntl_flags( va_arg( ap, int ) );

      /*
       *  XXX Double check this in the POSIX spec.  According to the Linux
       *  XXX man page, only these flags can be added.
       */

      flags = (iop->flags & ~(O_APPEND|O_NONBLOCK)) |
                   (flags & (O_APPEND|O_NONBLOCK));

      /*
       *  XXX If we are turning on append, should we seek to the end?
       */

      iop->flags = flags;
      return 0;

    case F_GETLK:
      return -1;

    case F_SETLK:
      return -1;

    case F_SETLKW:
      return -1;

    case F_SETOWN:       /*  for sockets. */
      return -1;

    case F_GETOWN:       /*  for sockets. */
      return -1;

    default:
      break;
  }
  return -1;
}
