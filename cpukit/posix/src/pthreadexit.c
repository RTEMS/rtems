/**
 * @file
 *
 * @ingroup POSIX_THREAD Thread API Extension
 *
 * @brief POSIX Thread Exit Shared Helper
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>

#include <rtems/score/threadimpl.h>

void pthread_exit( void *value_ptr )
{
  Thread_Control  *executing;
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  executing = _Per_CPU_Get_executing( cpu_self );

  _Thread_Exit( executing, THREAD_LIFE_TERMINATING, value_ptr );

  _Thread_Dispatch_direct( cpu_self );
  RTEMS_UNREACHABLE();
}
