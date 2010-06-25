/*
 *  COPYRIGHT (c) 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/libio.h>
#include <rtems/libio_.h>

int rtems_filesystem_default_readlink(
 rtems_filesystem_location_info_t  *loc,     /* IN  */
 char                              *buf,     /* OUT */
 size_t                            bufsize
)
{
  rtems_set_errno_and_return_minus_one( ENOTSUP );
}
