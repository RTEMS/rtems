/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains a definition of the
 *   ::_RTEMS_tasks_Information with zero objects.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksdata.h>

THREAD_INFORMATION_DEFINE_ZERO(
  _RTEMS_tasks,
  OBJECTS_CLASSIC_API,
  OBJECTS_RTEMS_TASKS
);
