/**
 * @file
 *
 * @ingroup ClassicTasksImpl
 */

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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/scheduler.h>

rtems_task_priority _RTEMS_Maximum_priority( void )
{
  return PRIORITY_MAXIMUM;
}
