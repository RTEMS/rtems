/*
 *  Initialization Manager
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
 *  SCORE_INIT and SAPI_INIT are defined so all of the super core and 
 *  super API data will be included in this object file.
 */

#define SAPI_INIT
#define SCORE_INIT

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/debug.h>
#include <rtems/extension.h>
#include <rtems/fatal.h>
#include <rtems/init.h>
#include <rtems/io.h>
#include <rtems/score/sysstate.h>

#include <rtems/score/apiext.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/copyrt.h>
#include <rtems/score/heap.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/score/priority.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/wkspace.h>

#include <rtems/sptables.h>


#include <rtems/rtems/rtemsapi.h>
#ifdef RTEMS_POSIX_API
#include <rtems/posix/posixapi.h>
#endif
#ifdef RTEMS_ITRON_API
#include <rtems/itron/itronapi.h>
#endif

Objects_Information *_Internal_Objects[ OBJECTS_INTERNAL_CLASSES_LAST + 1 ];

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

  if ( configuration_table == NULL )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_NO_CONFIGURATION_TABLE
    );

  /*
   *  Initialize the system state based on whether this is an MP system.
   */

#if defined(RTEMS_MULTIPROCESSING)
  multiprocessing_table = configuration_table->User_multiprocessing_table;

  _System_state_Handler_initialization(
    (multiprocessing_table) ? TRUE : FALSE
  );
#else
  multiprocessing_table = NULL;

  _System_state_Handler_initialization( FALSE );
#endif

  /*
   *  Provided just for user convenience.
   */

  _Configuration_Table    = configuration_table;
  _Configuration_MP_table = multiprocessing_table;

  /*
   *  Internally we view single processor systems as a very restricted
   *  multiprocessor system.
   */

  if ( multiprocessing_table == NULL )
    multiprocessing_table = 
      (void *)&_Initialization_Default_multiprocessing_table;

  if ( cpu_table == NULL )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_NO_CPU_TABLE
    );

  _CPU_Initialize( cpu_table, _Thread_Dispatch );

  /*
   *  Do this as early as possible to insure no debugging output
   *  is even attempted to be printed.
   */

  _Debug_Manager_initialization();

  _API_extensions_Initialization();

  _Thread_Dispatch_initialization();

  _Workspace_Handler_initialization(
     (void *)configuration_table->work_space_start,
     configuration_table->work_space_size
  );

  _User_extensions_Handler_initialization(
    configuration_table->number_of_initial_extensions,
    configuration_table->User_extension_table
  );

  _ISR_Handler_initialization();

  _Objects_Handler_initialization(
    multiprocessing_table->node,
    multiprocessing_table->maximum_nodes,
    multiprocessing_table->maximum_global_objects
  );

  _Objects_Information_table[OBJECTS_INTERNAL_API] = _Internal_Objects;

  _Priority_Handler_initialization();

  _Watchdog_Handler_initialization();

  _TOD_Handler_initialization( configuration_table->microseconds_per_tick );

  _Thread_Handler_initialization(
    configuration_table->ticks_per_timeslice,
    configuration_table->maximum_extensions,
    multiprocessing_table->maximum_proxies
  );

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Handler_initialization(
    multiprocessing_table->User_mpci_table,
    RTEMS_TIMEOUT
  );
#endif

/* MANAGERS */

  _RTEMS_API_Initialize( configuration_table );

  _Extension_Manager_initialization( configuration_table->maximum_extensions );

  _IO_Manager_initialization(
    configuration_table->Device_driver_table,
    configuration_table->number_of_device_drivers,
    configuration_table->maximum_drivers,
    configuration_table->maximum_devices
  );

#ifdef RTEMS_POSIX_API
  _POSIX_API_Initialize( configuration_table );
#endif

#ifdef RTEMS_ITRON_API
  _ITRON_API_Initialize( configuration_table );
#endif

  _System_state_Set( SYSTEM_STATE_BEFORE_MULTITASKING );

  /*
   *  No threads should be created before this point!!! 
   *  _Thread_Executing and _Thread_Heir are not set.
   *
   *  At this point all API extensions are in place.  After the call to
   *  _Thread_Create_idle() _Thread_Executing and _Thread_Heir will be set.
   */

  _Thread_Create_idle();


  /*
   *  Scheduling can properly occur now as long as we avoid dispatching.
   */

  if ( cpu_table->pretasking_hook )
    (*cpu_table->pretasking_hook)();

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Create_server();
#endif

  /*
   *  Run the API and BSPs predriver hook.
   */
 
  _API_extensions_Run_predriver();
 
  if ( _CPU_Table.predriver_hook )
    (*_CPU_Table.predriver_hook)();
 
  /*
   *  Initialize all the device drivers and initialize the MPCI layer.
   *
   *  NOTE:  The MPCI may be build upon a device driver.
   */
 
  _IO_Initialize_all_drivers();
 
#if defined(RTEMS_MULTIPROCESSING)
  if ( _System_state_Is_multiprocessing ) {
    _MPCI_Initialization();
    _MPCI_Internal_packets_Send_process_packet(
      MPCI_PACKETS_SYSTEM_VERIFY
    );
  }
#endif
 
  /*
   *  Run the APIs and BSPs postdriver hooks.
   *
   *  The API extensions are supposed to create user initialization tasks.
   */
 
  _API_extensions_Run_postdriver();
 
  if ( _CPU_Table.postdriver_hook )
    (*_CPU_Table.postdriver_hook)();

  return bsp_level;
}

void rtems_initialize_executive_late(
  rtems_interrupt_level bsp_level
)
{

  _System_state_Set( SYSTEM_STATE_BEGIN_MULTITASKING );

  _Thread_Start_multitasking();

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
  if ( _System_state_Current != SYSTEM_STATE_SHUTDOWN ) {
    _System_state_Set( SYSTEM_STATE_SHUTDOWN );
    _Thread_Stop_multitasking();
  }
}
