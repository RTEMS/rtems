/*
 *  Initialization Manager
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

/*
 *  INIT is defined so all of the data will be included in this
 *  file.
 */

#define INIT

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/copyrt.h>
#include <rtems/clock.h>
#include <rtems/tasks.h>
#include <rtems/debug.h>
#include <rtems/dpmem.h>
#include <rtems/event.h>
#include <rtems/extension.h>
#include <rtems/fatal.h>
#include <rtems/heap.h>
#include <rtems/init.h>
#include <rtems/intthrd.h>
#include <rtems/isr.h>
#include <rtems/intr.h>
#include <rtems/io.h>
#include <rtems/message.h>
#include <rtems/mp.h>
#include <rtems/mpci.h>
#include <rtems/part.h>
#include <rtems/priority.h>
#include <rtems/ratemon.h>
#include <rtems/region.h>
#include <rtems/sem.h>
#include <rtems/signal.h>
#include <rtems/sysstate.h>
#include <rtems/thread.h>
#include <rtems/timer.h>
#include <rtems/tod.h>
#include <rtems/userext.h>
#include <rtems/watchdog.h>
#include <rtems/wkspace.h>

#include <rtems/sptables.h>

/*PAGE
 *
 *  rtems_initialize_executive
 *
 *  This directive initializes all the kernels data structures
 *  to the states necessary for the kernel to begin execution.  All
 *  include files that contain global variable definitions should be
 *  included in this file.  The system threads and initialization threads
 *  are created and started by this routine.  This routine then
 *  initiates multithreading.
 *
 *  Input parameters:
 *    configuration_table - pointer to the user's configuration table
 *    cpu_table           - pointer to the user's CPU configuration table
 *
 *  Output parameters:  NONE
 */

struct months {
  unsigned32 months[2][13];
};

void rtems_initialize_executive(
  rtems_configuration_table *configuration_table,
  rtems_cpu_table           *cpu_table
)
{
  rtems_interrupt_level bsp_level;

  bsp_level = rtems_initialize_executive_early(configuration_table, cpu_table);
  rtems_initialize_executive_late( bsp_level );
}

rtems_interrupt_level rtems_initialize_executive_early(
  rtems_configuration_table *configuration_table,
  rtems_cpu_table           *cpu_table
)
{
  rtems_interrupt_level        bsp_level;
  rtems_multiprocessing_table *multiprocessing_table;

  /*
   *  Dispatching and interrupts are disabled until the end of the
   *  initialization sequence.  This prevents an inadvertent context
   *  switch before the executive is initialized.
   */

  _ISR_Disable( bsp_level );

  _System_state_Set( SYSTEM_STATE_BEFORE_INITIALIZATION );

  _CPU_Initialize( cpu_table, _Thread_Dispatch );

  /*
   *  Do this as early as possible to insure no debugging output
   *  is even attempted to be printed.
   */

  _Debug_Manager_initialization();

  multiprocessing_table = configuration_table->User_multiprocessing_table;
  if ( multiprocessing_table == NULL )
    multiprocessing_table =
      (void *) &_Configuration_Default_multiprocessing_table;

  _Configuration_Handler_initialization(
    configuration_table,
    multiprocessing_table,
    multiprocessing_table->User_mpci_table
  );

  _Attributes_Handler_initialization();

  _Thread_Dispatch_initialization();

  _User_extensions_Handler_initialization(
    configuration_table->User_extension_table
  );

  _Workspace_Handler_initialization(
     (void *)configuration_table->work_space_start,
     configuration_table->work_space_size
  );

  _ISR_Handler_initialization();

  _Objects_Handler_initialization(
    multiprocessing_table->node,
    multiprocessing_table->maximum_global_objects
  );

  _Priority_Handler_initialization();

  _Watchdog_Handler_initialization();

  _TOD_Handler_initialization( configuration_table->microseconds_per_tick );

  _Thread_Handler_initialization(
    configuration_table->ticks_per_timeslice,
    multiprocessing_table->maximum_proxies
  );

  _MPCI_Handler_initialization();

/* MANAGERS */

  _Interrupt_Manager_initialization();

  _Multiprocessing_Manager_initialization();

  _RTEMS_tasks_Manager_initialization( configuration_table->maximum_tasks );

  _Timer_Manager_initialization( configuration_table->maximum_timers );

  _Extension_Manager_initialization( configuration_table->maximum_extensions );

  _IO_Manager_initialization(
    configuration_table->Device_driver_table,
    configuration_table->number_of_device_drivers
  );

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

  _Internal_threads_Initialization();

  if ( cpu_table->pretasking_hook )
    (*cpu_table->pretasking_hook)();

  _System_state_Set( SYSTEM_STATE_BEFORE_MULTITASKING );

  return bsp_level;
}

void rtems_initialize_executive_late(
  rtems_interrupt_level bsp_level
)
{

  _System_state_Set( SYSTEM_STATE_BEGIN_MULTITASKING );

  _Thread_Start_multitasking(
    _Internal_threads_System_initialization_thread,
    _Internal_threads_Idle_thread
  );

  /*
   *  Restore the interrupt level to what the BSP had.  Technically,
   *  this is unnecessary since the BSP should have all interrupts
   *  disabled when rtems_initialize_executive is invoked.  But this keeps
   *  the ISR Disable/Enable calls paired.
   */

  _ISR_Enable( bsp_level );
}

/*PAGE
 *
 *  rtems_shutdown_executive
 *
 *  This kernel routine shutdowns the executive.  It halts multitasking
 *  and returns control to the application execution "thread" which
 *  initialially invoked the rtems_initialize_executive directive.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void rtems_shutdown_executive(
   unsigned32 result
)
{
  _Thread_Stop_multitasking();
}
