/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <devctl.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/seterr.h>

/*PAGE
 *
 *  21.2.1 Control a Device, P1003.4b/D8, p. 65
 */

int devctl(
  int     filedes __attribute__((unused)),
  void   *dev_data_ptr  __attribute__((unused)),
  size_t  nbyte  __attribute__((unused)),
  int    *dev_info_ptr  __attribute__((unused))
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
