/*  
 *  chroot() -  Change Root Directory
 *  Author: fernando.ruiz@ctv.es
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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

int chroot(
  const char *pathname
)
{
  int                               result;
  rtems_filesystem_location_info_t  loc;
  
  if (rtems_current_user_env == &rtems_global_user_env)
    set_errno_and_return_minus_one( ENOTSUP );

  loc = rtems_filesystem_root;     /* save the value */

  /* if has been already changed */
  rtems_filesystem_root = rtems_global_user_env.root_directory;

  result = chdir(pathname);
  if (result) {
    rtems_filesystem_root = loc; /* restore the value */
    set_errno_and_return_minus_one( errno );
  };
  rtems_filesystem_root = rtems_filesystem_current;

  result = chdir("/");
  if (result) {
    rtems_filesystem_root = loc; /* restore the value */
    set_errno_and_return_minus_one( errno );
  };

  /*XXX : Call this? Sorry but I don't known if it is necesary */
  /* The old root.
   rtems_filesystem_freenode( &loc );
   */
  return 0;
}
