/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_scheduler_ident(
  rtems_name  name,
  rtems_id   *id
)
{
  rtems_status_code sc;

  if ( id != NULL ) {
    size_t n = _Scheduler_Count;
    size_t i;

    sc = RTEMS_INVALID_NAME;

    for ( i = 0 ; i < n && sc == RTEMS_INVALID_NAME ; ++i ) {
      const Scheduler_Control *scheduler = &_Scheduler_Table[ i ];

      if ( scheduler->name == name ) {
        *id = _Scheduler_Build_id( i );
        sc = RTEMS_SUCCESSFUL;
      }
    }
  } else {
    sc = RTEMS_INVALID_ADDRESS;
  }

  return sc;
}
