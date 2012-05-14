/*
 *   fcntl() - POSIX 1003.1b 6.5.2 - File Control
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

#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

#include <rtems/libio_.h>

static int duplicate_iop( rtems_libio_t *iop, int fd2 )
{
  int rv = 0;

  /*
   * FIXME: We ignore the start value fd2 for the file descriptor search.  This
   * is not POSIX conform.
   */
  rtems_libio_t *diop = rtems_libio_allocate();

  if (diop != NULL) {
    int oflag = rtems_libio_to_fcntl_flags( iop->flags );

    oflag &= ~O_CREAT;
    diop->flags |= rtems_libio_fcntl_flags( oflag );

    rtems_filesystem_instance_lock( &iop->pathinfo );
    rtems_filesystem_location_clone( &diop->pathinfo, &iop->pathinfo );
    rtems_filesystem_instance_unlock( &iop->pathinfo );

    /*
     * XXX: We call the open handler here to have a proper open and close pair.
     *
     * FIXME: What to do with the path?
     */
    rv = (*diop->pathinfo.handlers->open_h)( diop, NULL, oflag, 0 );
    if ( rv == 0 ) {
      rv = diop - rtems_libio_iops;
    } else {
      rtems_libio_free( diop );
    }
  } else {
    rv = -1;
  }

  return rv;
}

static int vfcntl(
  int fd,
  int cmd,
  va_list ap
)
{
  rtems_libio_t *iop;
  int            fd2;
  int            flags;
  int            mask;
  int            ret = 0;

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
      ret = duplicate_iop( iop, fd2 );
      break;

    case F_GETFD:        /* get f_flags */
      ret = ((iop->flags & LIBIO_FLAGS_CLOSE_ON_EXEC) != 0);
      break;

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
      break;

    case F_GETFL:        /* more flags (cloexec) */
      ret = rtems_libio_to_fcntl_flags( iop->flags );
      break;

    case F_SETFL:
      flags = rtems_libio_fcntl_flags( va_arg( ap, int ) );
      mask = LIBIO_FLAGS_NO_DELAY | LIBIO_FLAGS_APPEND;

      /*
       *  XXX If we are turning on append, should we seek to the end?
       */

      iop->flags = (iop->flags & ~mask) | (flags & mask);
      break;

    case F_GETLK:
      errno = ENOTSUP;
      ret = -1;
      break;

    case F_SETLK:
      errno = ENOTSUP;
      ret = -1;
      break;

    case F_SETLKW:
      errno = ENOTSUP;
      ret = -1;
      break;

    case F_SETOWN:       /*  for sockets. */
      errno = ENOTSUP;
      ret = -1;
      break;

    case F_GETOWN:       /*  for sockets. */
      errno = ENOTSUP;
      ret = -1;
      break;

    default:
      errno = EINVAL;
      ret = -1;
      break;
  }

  /*
   *  If we got this far successfully, then we give the optional
   *  filesystem specific handler a chance to process this.
   */

  if (ret >= 0) {
    int err = (*iop->pathinfo.handlers->fcntl_h)( iop, cmd );
    if (err) {
      errno = err;
      ret = -1;
    }
  }
  return ret;
}

int fcntl(
  int fd,
  int cmd,
  ...
)
{
  int            ret;
  va_list        ap;
  va_start( ap, cmd );
  ret = vfcntl(fd,cmd,ap);
  va_end(ap);
  return ret;
}


/*
 *  _fcntl_r
 *
 *  This is the Newlib dependent reentrant version of fcntl().
 */

#if defined(RTEMS_NEWLIB) && !defined(HAVE_FCNTL_R)

#include <reent.h>

int _fcntl_r(
  struct _reent *ptr __attribute__((unused)),
  int fd,
  int cmd,
  int arg
)
{
  return fcntl( fd, cmd, arg );
}
#endif
