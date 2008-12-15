/*
 *  POSIX API Initialization Support
 *
 *  NOTE:
 *
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

/*
 *  RTEMS_API_INIT is defined so all of the RTEMS API
 *  data will be included in this object file.
 */

#define RTEMS_API_INIT

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/rtemsapi.h>

#include <rtems/rtems/intr.h>
#include <rtems/rtems/barrier.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/message.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/mp.h>
#endif
#include <rtems/rtems/part.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/rtems/region.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/signal.h>
#include <rtems/rtems/timer.h>

Objects_Information *_RTEMS_Objects[ OBJECTS_RTEMS_CLASSES_LAST + 1 ];

/*PAGE
 *
 *  _RTEMS_API_Initialize
 *
 *  XXX
 */

void _RTEMS_API_Initialize(void)
{
  rtems_api_configuration_table *api;

  /*
   * Install our API Object Management Table and initialize the
   * various managers.
   */
  api = &Configuration_RTEMS_API;

  _Objects_Information_table[OBJECTS_CLASSIC_API] = _RTEMS_Objects;

  _Attributes_Handler_initialization();

  _Interrupt_Manager_initialization();

#if defined(RTEMS_MULTIPROCESSING)
  _Multiprocessing_Manager_initialization();
#endif

  _RTEMS_tasks_Manager_initialization( api->maximum_tasks );

  _Timer_Manager_initialization( api->maximum_timers );

  _Signal_Manager_initialization();

  _Event_Manager_initialization();

  _Message_queue_Manager_initialization( api->maximum_message_queues );

  _Semaphore_Manager_initialization( api->maximum_semaphores );

  _Partition_Manager_initialization( api->maximum_partitions );

  _Region_Manager_initialization( api->maximum_regions );

  _Dual_ported_memory_Manager_initialization( api->maximum_ports);

  _Rate_monotonic_Manager_initialization( api->maximum_periods );

  _Barrier_Manager_initialization( api->maximum_barriers );
}

/* end of file */
