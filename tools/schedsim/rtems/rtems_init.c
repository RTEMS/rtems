/*
 *  BASED UPON SOURCE IN RTEMS, MODIFIED FOR SIMULATOR
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#define SAPI_INIT
#define SCORE_INIT
#define RTEMS_API_INIT

#include <rtems.h> 
#include <rtems/score/apiext.h>
#include <rtems/score/bitfield.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/wkspace.h>

Objects_Information *_Internal_Objects[ OBJECTS_INTERNAL_CLASSES_LAST + 1 ];

Objects_Information *_RTEMS_Objects[ OBJECTS_RTEMS_CLASSES_LAST + 1 ];

extern void check_heir_and_executing(void);

void rtems_initialize_data_structures(void)
{
  _System_state_Handler_initialization( FALSE );

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

  _User_extensions_Handler_initialization();

  // _ISR_Handler_initialization();

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


/* MANAGERS */
  /*
   * Install our API Object Management Table and initialize the
   * various managers.
   */
  _Objects_Information_table[OBJECTS_CLASSIC_API] = _RTEMS_Objects;

  _RTEMS_tasks_Manager_initialization();
  _Semaphore_Manager_initialization();

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

  _System_state_Set( SYSTEM_STATE_BEGIN_MULTITASKING );

  _Thread_Start_multitasking();

  /*
   *  Now we are back in a non-dispatching critical section
   */
  _Thread_Dispatch_set_disable_level(0);

  /*
   * Print an initial message
   */
  check_heir_and_executing();
}
