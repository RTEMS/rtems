/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief POSIX 1003.1b 6.5.2 - File Control
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

#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

#include <rtems/libio_.h>

static int duplicate_iop( rtems_libio_t *iop )
{
  int            rv;
  int            oflag;
  rtems_libio_t *diop;

  oflag = rtems_libio_to_fcntl_flags( rtems_libio_iop_flags( iop ) );
  diop = rtems_libio_allocate();

  if (diop != NULL) {
    rtems_filesystem_instance_lock( &iop->pathinfo );
    rtems_filesystem_location_clone( &diop->pathinfo, &iop->pathinfo );
    rtems_filesystem_instance_unlock( &iop->pathinfo );

    rtems_libio_iop_flags_set( diop, rtems_libio_from_fcntl_flags( oflag ) );
    /*
     * XXX: We call the open handler here to have a proper open and close pair.
     *
     * FIXME: What to do with the path?
     */
    rv = (*diop->pathinfo.handlers->open_h)( diop, NULL, oflag, 0 );
    if ( rv == 0 ) {
      rtems_libio_iop_flags_set( diop, LIBIO_FLAGS_OPEN );
      rv = rtems_libio_iop_to_descriptor( diop );
    } else {
      rtems_libio_free( diop );
    }
  } else {
    rv = -1;
  }

  return rv;
}

static int duplicate2_iop( rtems_libio_t *iop, int fd2 )
{
  rtems_libio_t *iop2;
  int            rv = 0;

  if ( (uint32_t) fd2 >= rtems_libio_number_iops ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  iop2 = rtems_libio_iop( fd2 );

  if (iop != iop2)
  {
    int oflag;

    if ((rtems_libio_iop_flags( iop2 ) & LIBIO_FLAGS_OPEN) != 0) {
      rv = (*iop2->pathinfo.handlers->close_h)( iop2 );
    }

    if (rv == 0) {
      oflag = rtems_libio_to_fcntl_flags( rtems_libio_iop_flags( iop ) );
      rtems_libio_iop_flags_set( iop2, rtems_libio_from_fcntl_flags( oflag ) );

      rtems_filesystem_instance_lock( &iop->pathinfo );
      rtems_filesystem_location_clone( &iop2->pathinfo, &iop->pathinfo );
      rtems_filesystem_instance_unlock( &iop->pathinfo );

      /*
       * XXX: We call the open handler here to have a proper open and close
       *      pair.
       *
       * FIXME: What to do with the path?
       */
      rv = (*iop2->pathinfo.handlers->open_h)( iop2, NULL, oflag, 0 );
      if ( rv == 0 ) {
        rv = fd2;
      }
    }
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

  LIBIO_GET_IOP( fd, iop );

  /*
   *  Now process the fcntl().
   */

  /*
   *  This switch should contain all the cases from POSIX.
   */

  switch ( cmd ) {
    case F_DUPFD:        /* dup */
      ret = duplicate_iop( iop );
      break;

    case F_DUP2FD:       /* dup2 */
      fd2 = va_arg( ap, int );
      ret = duplicate2_iop( iop, fd2 );
      break;

    case F_GETFD:        /* get f_flags */
      ret = ((rtems_libio_iop_flags(iop) & LIBIO_FLAGS_CLOSE_ON_EXEC) != 0);
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
        rtems_libio_iop_flags_set( iop, LIBIO_FLAGS_CLOSE_ON_EXEC );
      else
        rtems_libio_iop_flags_clear( iop, LIBIO_FLAGS_CLOSE_ON_EXEC );
      break;

    case F_GETFL:        /* more flags (cloexec) */
      ret = rtems_libio_to_fcntl_flags( rtems_libio_iop_flags( iop ) );
      break;

    case F_SETFL:
      flags = rtems_libio_from_fcntl_flags( va_arg( ap, int ) );
      mask = LIBIO_FLAGS_NO_DELAY | LIBIO_FLAGS_APPEND;

      /*
       *  XXX If we are turning on append, should we seek to the end?
       */

      rtems_libio_iop_flags_clear( iop, mask );
      rtems_libio_iop_flags_set( iop, flags & mask );
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
    if (err == 0 && !rtems_libio_iop_is_open( iop ) ) {
      err = EBADF;
    }
    if (err != 0) {
      errno = err;
      ret = -1;
    }
  }

  rtems_libio_iop_drop( iop );
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
  struct _reent *ptr RTEMS_UNUSED,
  int fd,
  int cmd,
  int arg
)
{
  return fcntl( fd, cmd, arg );
}
#endif
