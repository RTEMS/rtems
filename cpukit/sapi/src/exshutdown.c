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

#include <rtems/system.h>
#include <rtems/init.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>

#if defined(RTEMS_SMP)
  #include <rtems/score/smp.h>
#endif

/*
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
   uint32_t   result
)
{
  if ( _System_state_Is_up( _System_state_Get() ) ) {
    #if defined(RTEMS_SMP)
      _SMP_Request_other_cores_to_shutdown();
    #endif

    _Per_CPU_Information[0].idle->Wait.return_code = result;

    _System_state_Set( SYSTEM_STATE_SHUTDOWN );
    _Thread_Stop_multitasking();

    /*******************************************************************
     *******************************************************************
     *******************************************************************
     ******     RETURN TO RTEMS_INITIALIZE_START_MULTITASKING()   ******
     ******                 AND THEN TO BOOT_CARD()               ******
     *******************************************************************
     *******************************************************************
     *******************************************************************/
  }
  _Internal_error_Occurred(
    INTERNAL_ERROR_CORE,
    true,
    INTERNAL_ERROR_SHUTDOWN_WHEN_NOT_UP
  );
}
