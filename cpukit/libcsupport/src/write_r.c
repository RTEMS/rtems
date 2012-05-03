/*
 *  write_r() - POSIX 1003.1b 6.4.2 - Write to a File
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>
#include <rtems/seterr.h>

/*
 *  _write_r
 *
 *  This is the Newlib dependent reentrant version of write().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

_ssize_t _write_r(
  struct _reent *ptr __attribute__((unused)),
  int            fd,
  const void    *buf,
  size_t         nbytes
)
{
  return write( fd, buf, nbytes );
}
#endif
