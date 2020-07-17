/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test-info.h>
#include <rtems/score/threadimpl.h>

void rtems_test_busy_cpu_usage( time_t seconds, long nanoseconds )
{
  Thread_Control    *executing;
  Timestamp_Control  busy;
  Timestamp_Control  start;
  Timestamp_Control  now;

  executing = _Thread_Get_executing();
  _Thread_Get_CPU_time_used( executing, &start );
  _Timestamp_Set( &busy, seconds, nanoseconds );

  do {
    _Thread_Get_CPU_time_used( executing, &now );
  } while ( now - start < busy );
}
