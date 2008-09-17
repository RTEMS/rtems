/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems/libio.h>
#include <rtems/libio_.h>

/*
 *  Global information for POSIX Process Environment Support
 */

rtems_user_env_t   rtems_global_user_env;
rtems_user_env_t * rtems_current_user_env = &rtems_global_user_env;


