/*
 *  POSIX API Initialization Support
 *
 *  NOTE:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

/*
 *  RTEMS_API_INIT is defined so all of the RTEMS API
 *  data will be included in this object file.
 */

#define RTEMS_API_INIT

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/rtemsapi.h>

#include <rtems/rtems/intr.h>
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

void _RTEMS_API_Initialize(
  rtems_configuration_table *configuration_table
)
{
  rtems_api_configuration_table *api_configuration;

  api_configuration = configuration_table->RTEMS_api_configuration;

  _Objects_Information_table[OBJECTS_CLASSIC_API] = _RTEMS_Objects;

  _Attributes_Handler_initialization();
 
  _Interrupt_Manager_initialization();
 
#if defined(RTEMS_MULTIPROCESSING)
  _Multiprocessing_Manager_initialization();
#endif
 
  _RTEMS_tasks_Manager_initialization(
    api_configuration->maximum_tasks,
    api_configuration->number_of_initialization_tasks,
    api_configuration->User_initialization_tasks_table
  );
 
  _Timer_Manager_initialization( api_configuration->maximum_timers );
 
  _Signal_Manager_initialization();
 
  _Event_Manager_initialization();
 
  _Message_queue_Manager_initialization(
    api_configuration->maximum_message_queues
  );
 
  _Semaphore_Manager_initialization( api_configuration->maximum_semaphores );
 
  _Partition_Manager_initialization( api_configuration->maximum_partitions );
 
  _Region_Manager_initialization( api_configuration->maximum_regions );
 
  _Dual_ported_memory_Manager_initialization( api_configuration->maximum_ports);
 
  _Rate_monotonic_Manager_initialization( api_configuration->maximum_periods );
}

/* end of file */
