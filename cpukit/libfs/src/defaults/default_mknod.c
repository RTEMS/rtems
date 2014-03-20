/**
 * @file
 *
 * @brief RTEMS Default File System creates a special or ordinary file
 * @ingroup LibIOFSOps File System Operations  
 */

/*
 *  COPYRIGHT (c) 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int rtems_filesystem_default_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
)
{
  rtems_set_errno_and_return_minus_one( ENOTSUP );
}
