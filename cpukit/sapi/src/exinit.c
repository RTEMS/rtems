/**
 * @file
 *
 * @brief Initialization Manager
 *
 * @ingroup ClassicRTEMS
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/config.h>
#include <rtems/extensionimpl.h>
#include <rtems/init.h>
#include <rtems/ioimpl.h>
#include <rtems/sysinit.h>
#include <rtems/score/sysstate.h>

#include <rtems/score/copyrt.h>
#include <rtems/score/heap.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/priority.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/timecounter.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/wkspace.h>

const char _Copyright_Notice[] =
"COPYRIGHT (c) 1989-2008.\n\
On-Line Applications Research Corporation (OAR).\n";

static Objects_Information *
_Internal_Objects[ OBJECTS_INTERNAL_CLASSES_LAST + 1 ];

static Objects_Information *_RTEMS_Objects[ OBJECTS_RTEMS_CLASSES_LAST + 1 ];

static Objects_Information *_POSIX_Objects[ OBJECTS_POSIX_CLASSES_LAST + 1 ];

Objects_Information ** const
_Objects_Information_table[ OBJECTS_APIS_LAST + 1 ] = {
  NULL,
  &_Internal_Objects[ 0 ],
  &_RTEMS_Objects[ 0 ],
  &_POSIX_Objects[ 0 ]
};

static void rtems_initialize_data_structures(void)
{
  /*
   *  Dispatching and interrupts are disabled until the end of the
   *  initialization sequence.  This prevents an inadvertent context
   *  switch before the executive is initialized.
   *
   *  WARNING: Interrupts should have been disabled by the BSP and
   *           are disabled by boot_card().
   */

  /*
   * Initialize any target architecture specific support as early as possible
   */
  _CPU_Initialize();

  _Thread_Dispatch_initialization();

  _ISR_Handler_initialization();

  _Thread_Handler_initialization();

  _Scheduler_Handler_initialization();

  _SMP_Handler_initialize();
}

RTEMS_LINKER_ROSET( _Sysinit, rtems_sysinit_item );

RTEMS_SYSINIT_ITEM(
  rtems_initialize_data_structures,
  RTEMS_SYSINIT_DATA_STRUCTURES,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

/*
 *  No threads should be created before this point!!!
 *  _Thread_Executing and _Thread_Heir are not set.
 *
 *  At this point all API extensions are in place.  After the call to
 *  _Thread_Create_idle() _Thread_Executing and _Thread_Heir will be set.
 *
 *  Scheduling can properly occur afterwards as long as we avoid dispatching.
 */
RTEMS_SYSINIT_ITEM(
  _Thread_Create_idle,
  RTEMS_SYSINIT_IDLE_THREADS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

/* Initialize I/O drivers.
 *
 * Driver Manager note:
 * All drivers may not be registered yet. Drivers will dynamically
 * be initialized when registered in level 2,3 and 4.
 */
RTEMS_SYSINIT_ITEM(
  _IO_Initialize_all_drivers,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

void rtems_initialize_executive(void)
{
  const rtems_sysinit_item *item;

  /* Invoke the registered system initialization handlers */
  RTEMS_LINKER_SET_FOREACH( _Sysinit, item ) {
    ( *item->handler )();
  }

  _System_state_Set( SYSTEM_STATE_UP );

  _SMP_Request_start_multitasking();

  _Thread_Start_multitasking();

  /*******************************************************************
   *******************************************************************
   *******************************************************************
   ******                 APPLICATION RUNS HERE                 ******
   ******              THE FUNCTION NEVER RETURNS               ******
   *******************************************************************
   *******************************************************************
   *******************************************************************/
}
