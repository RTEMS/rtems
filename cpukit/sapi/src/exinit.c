/*
 *  Initialization Manager
 *
 *  COPYRIGHT (c) 1989-2007.
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

rtems_interrupt_level rtems_initialize_executive_early(
  rtems_configuration_table *configuration_table
)
{
  rtems_interrupt_level        bsp_level;

  /*
   *  Dispatching and interrupts are disabled until the end of the
   *  initialization sequence.  This prevents an inadvertent context
   *  switch before the executive is initialized.
   */
  _ISR_Disable( bsp_level );

  /*
   *  Make sure the parameters were not NULL.
   */
  if ( configuration_table == NULL )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_NO_CONFIGURATION_TABLE
    );

  /*
   *  Provide pointers just for later convenience.
   */
  _Configuration_Table    = configuration_table;

  /*
   * Initialize any target architecture specific support as early as possible
   */
  _CPU_Initialize( _Thread_Dispatch );

#if defined(RTEMS_MULTIPROCESSING)
  /*
   *  Initialize the system state based on whether this is an MP system.
   *  In an MP configuration, internally we view single processor
   *  systems as a very restricted multiprocessor system.
   */
  _Configuration_MP_table = configuration_table->User_multiprocessing_table;

  if ( _Configuration_MP_table == NULL ) {
    _Configuration_MP_table =
      (void *)&_Initialization_Default_multiprocessing_table;
    _System_state_Handler_initialization( FALSE );
  } else {
    _System_state_Handler_initialization( TRUE );
  }
#else
  _System_state_Handler_initialization( FALSE );
#endif

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
#if defined(RTEMS_MULTIPROCESSING)
    _Configuration_MP_table->node,
    _Configuration_MP_table->maximum_nodes,
    _Configuration_MP_table->maximum_global_objects
#endif
  );

  _Objects_Information_table[OBJECTS_INTERNAL_API] = _Internal_Objects;

  /*
   * Initialize the internal allocator Mutex
   */
  _API_Mutex_Initialization( 1 );
  _API_Mutex_Allocate( &_RTEMS_Allocator_Mutex );

  _Priority_Handler_initialization();

  _Watchdog_Handler_initialization();

  _TOD_Handler_initialization( configuration_table->microseconds_per_tick );

  _Thread_Handler_initialization(
    configuration_table->ticks_per_timeslice,
    configuration_table->maximum_extensions
#if defined(RTEMS_MULTIPROCESSING)
    ,
    _Configuration_MP_table->maximum_proxies
#endif
  );

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Handler_initialization(
    _Configuration_MP_table->User_mpci_table,
    RTEMS_TIMEOUT
  );
#endif

/* MANAGERS */

  _RTEMS_API_Initialize( configuration_table );

  _Extension_Manager_initialization( configuration_table->maximum_extensions );

  _IO_Manager_initialization(
    configuration_table->Device_driver_table,
    configuration_table->number_of_device_drivers,
    configuration_table->maximum_drivers
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

  {
    extern void bsp_pretasking_hook(void);
    bsp_pretasking_hook();
  }

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Create_server();
#endif

  /*
   *  Run the API and BSPs predriver hook.
   */

  _API_extensions_Run_predriver();

  {
    extern void bsp_predriver_hook(void);
    bsp_predriver_hook();
  }

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

  {
    extern void bsp_postdriver_hook(void);
    bsp_postdriver_hook();
  }

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
