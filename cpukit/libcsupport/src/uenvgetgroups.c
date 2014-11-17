/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 * embedded brains GmbH
 * Dornierstr. 4
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/userenv.h>
#include <rtems.h>

#include <sys/types.h>
#include <unistd.h>

void rtems_current_user_env_getgroups(void)
{
  rtems_user_env_t *uenv = rtems_current_user_env_get();
  int ngroups = (int) RTEMS_ARRAY_SIZE( uenv->groups );

  ngroups = getgroups( ngroups, &uenv->groups[ 0 ] );
  if ( ngroups > 0 ) {
    uenv->ngroups = (size_t) ngroups;
  } else {
    uenv->ngroups = 0;
  }
}
