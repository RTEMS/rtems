/*  
 *  chroot() -  Change Root Directory
 *  Author: fernando.ruiz@ctv.es
 *
 *  COPYRIGHT (c) 1989-1999.
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

#include <rtems.h>

#include <unistd.h>
#include <errno.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int chroot(
  const char *pathname
)
{
  int                               result;
  rtems_filesystem_location_info_t  loc;
  
  /* an automatic call to new private env the first time */
  if (rtems_current_user_env == &rtems_global_user_env) {
   rtems_libio_set_private_env(); /* try to set a new private env*/
   if (rtems_current_user_env == &rtems_global_user_env) /* not ok */
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }; 

  result = chdir(pathname);
  if (result) {
    rtems_set_errno_and_return_minus_one( errno );
  };
  /* clone the new root location */
  if (rtems_filesystem_evaluate_path(".", 0, &loc, 0)) {
	/* our cwd has changed, though - but there is no easy way of return :-( */
    rtems_set_errno_and_return_minus_one( errno );
  }
  rtems_filesystem_freenode(&rtems_filesystem_root);
  rtems_filesystem_root = loc;

  return 0;
}
