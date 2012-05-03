/*
 *  COPYRIGHT (c) 2010.
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

ssize_t rtems_filesystem_default_readlink(
  const rtems_filesystem_location_info_t *loc,
  char *buf,
  size_t bufsize
)
{
  rtems_set_errno_and_return_minus_one( ENOTSUP );
}
