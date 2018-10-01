/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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
#include <rtems/score/threadimpl.h>

void rtems_task_exit( void )
{
  Thread_Control  *executing;
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  executing = _Per_CPU_Get_executing( cpu_self );

  _Thread_Exit(
    executing,
    THREAD_LIFE_TERMINATING | THREAD_LIFE_DETACHED,
    NULL
  );

  _Thread_Dispatch_direct( cpu_self );
  RTEMS_UNREACHABLE();
}
