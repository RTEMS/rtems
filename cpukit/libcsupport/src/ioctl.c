/**
 * @file
 *
 * @brief ioctl() system call
 * @ingroup libcsupport
 */

/*
 *  This routine is not defined in the POSIX 1003.1b standard but is
 *  commonly supported on most UNIX and POSIX systems.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

#include <unistd.h>

int ioctl(
  int  fd,
  ioctl_command_t  command,
  ...
)
{
  va_list            ap;
  int                rc;
  rtems_libio_t     *iop;
  void              *buffer;

  LIBIO_GET_IOP( fd, iop );

  va_start(ap, command);

  buffer = va_arg(ap, void *);

  /*
   *  Now process the ioctl().
   */
  rc = (*iop->pathinfo.handlers->ioctl_h)( iop, command, buffer );

  va_end( ap );
  rtems_libio_iop_drop( iop );
  return rc;
}
