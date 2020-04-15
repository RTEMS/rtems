/**
 *  @file
 *
 *  @brief RTEMS Get Self Task Id
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksimpl.h>

rtems_id rtems_task_self(void)
{
   return _Thread_Get_executing()->Object.id;
}
