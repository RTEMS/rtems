/**
 * @file
 *
 * @ingroup LibIOFSOps File System Operations  
 *
 * @brief RTEMS Default File System Truncates a file to indicated length
 */

/*
 *  COPYRIGHT (c) 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int rtems_filesystem_default_ftruncate(
  rtems_libio_t *iop,
  off_t          length
)
{
  rtems_set_errno_and_return_minus_one( EINVAL );
}
