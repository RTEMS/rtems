/*
 *  Stub Base file system initialization
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

rtems_user_env_t  rtems_global_user_env;
rtems_user_env_t *rtems_current_user_env;

/*
 *  rtems_filesystem_initialize
 *
 *  Initialize the foundation of the file system.  It is an empty function.
 */

void rtems_filesystem_initialize( void )
{
}
