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
  
  va_start( ap, cmd );

  /*
   *  If this is not a file system based entity, it is an error.
   */

  if ( rtems_file_descriptor_type( fd ) )
    set_errno_and_return_minus_one( EBADF );

  /*
   *  Now process the fcntl().
   */

  iop = rtems_libio_iop( fd );
  rtems_libio_check_fd( fd );

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
      return -1;

    case F_SETFL:
      return -1;

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
