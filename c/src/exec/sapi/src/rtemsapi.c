/*
 *  RTEMS API Support
 *
 *  NOTE:
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/rtemsapi.h>

#define INIT

#include <rtems/rtems/intr.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/mp.h>
#include <rtems/rtems/part.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/rtems/region.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/signal.h>
#include <rtems/rtems/timer.h>

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
  _Attributes_Handler_initialization();
 
  _Interrupt_Manager_initialization();
 
  _Multiprocessing_Manager_initialization();
 
  _RTEMS_tasks_Manager_initialization(
    configuration_table->maximum_tasks,
    configuration_table->number_of_initialization_tasks,
    configuration_table->User_initialization_tasks_table
  );
 
  _Timer_Manager_initialization( configuration_table->maximum_timers );
 
  _Signal_Manager_initialization();
 
  _Event_Manager_initialization();
 
  _Message_queue_Manager_initialization(
    configuration_table->maximum_message_queues
  );
 
  _Semaphore_Manager_initialization(
    configuration_table->maximum_semaphores
  );
 
  _Partition_Manager_initialization(
    configuration_table->maximum_partitions
  );
 
  _Region_Manager_initialization( configuration_table->maximum_regions );
 
  _Dual_ported_memory_Manager_initialization(
    configuration_table->maximum_ports
  );
 
  _Rate_monotonic_Manager_initialization(
    configuration_table->maximum_periods
  );
}

/* end of file */
