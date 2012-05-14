/*
 *  Initialization Manager
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
#include <rtems/score/scheduler.h>
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

#if defined(RTEMS_SMP)
  #include <rtems/score/smp.h>
  #include <rtems/score/percpu.h>
#endif

Objects_Information *_Internal_Objects[ OBJECTS_INTERNAL_CLASSES_LAST + 1 ];

void rtems_initialize_data_structures(void)
{
  /*
   *  Dispatching and interrupts are disabled until the end of the
   *  initialization sequence.  This prevents an inadvertent context
   *  switch before the executive is initialized.
   *
   *  WARNING: Interrupts should have been disabled by the BSP and
   *           are disabled by boot_card().
   */

  #if defined(RTEMS_MULTIPROCESSING)
    /*
     *  Initialize the system state based on whether this is an MP system.
     *  In an MP configuration, internally we view single processor
     *  systems as a very restricted multiprocessor system.
     */
    _Configuration_MP_table = Configuration.User_multiprocessing_table;

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
   * Initialize any target architecture specific support as early as possible
   */
  _CPU_Initialize();

  #if defined(RTEMS_MULTIPROCESSING)
    _Objects_MP_Handler_early_initialization();
  #endif

  /*
   *  Do this as early as possible to ensure no debugging output
   *  is even attempted to be printed.
   */
  _Debug_Manager_initialization();

  _API_extensions_Initialization();

  _Thread_Dispatch_initialization();

  /*
   *  Before this is called, we are not allowed to allocate memory
   *  from the Workspace because it is not initialized.
   */
  _Workspace_Handler_initialization();

  #if defined(RTEMS_SMP)
    _SMP_Handler_initialize();
  #endif

  _User_extensions_Handler_initialization();
  _ISR_Handler_initialization();

  /*
   * Initialize the internal support API and allocator Mutex
   */
  _Objects_Information_table[OBJECTS_INTERNAL_API] = _Internal_Objects;

  _API_Mutex_Initialization( 1 );
  _API_Mutex_Allocate( &_RTEMS_Allocator_Mutex );

  _Priority_bit_map_Handler_initialization();
  _Watchdog_Handler_initialization();
  _TOD_Handler_initialization();

  _Thread_Handler_initialization();

  _Scheduler_Handler_initialization();

  #if defined(RTEMS_MULTIPROCESSING)
    _Objects_MP_Handler_initialization();
    _MPCI_Handler_initialization( RTEMS_TIMEOUT );
  #endif

/* MANAGERS */

  _RTEMS_API_Initialize();

  _Extension_Manager_initialization();

  _IO_Manager_initialization();

  #ifdef RTEMS_POSIX_API
    _POSIX_API_Initialize();
  #endif

  /*
   * Discover and initialize the secondary cores in an SMP system.
   */
  #if defined(RTEMS_SMP)
    _SMP_Processor_count =
        bsp_smp_initialize( rtems_configuration_smp_maximum_processors );
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
}

void rtems_initialize_before_drivers(void)
{

  #if defined(RTEMS_MULTIPROCESSING)
    _MPCI_Create_server();
  #endif

  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    /*
     *  Run the API and BSPs predriver hook.
     */
    _API_extensions_Run_predriver();
  #endif
}

void rtems_initialize_device_drivers(void)
{
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
}

uint32_t rtems_initialize_start_multitasking(void)
{
  _System_state_Set( SYSTEM_STATE_BEGIN_MULTITASKING );

  _Thread_Start_multitasking();

  /*******************************************************************
   *******************************************************************
   *******************************************************************
   ******                 APPLICATION RUNS HERE                 ******
   ******            RETURNS WHEN SYSTEM IS SHUT DOWN           ******
   *******************************************************************
   *******************************************************************
   *******************************************************************/
  
  return _Per_CPU_Information[0].idle->Wait.return_code;
}
