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

#include <rtems/init.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/interr.h>

#if defined(RTEMS_SMP)
  #include <rtems/score/smp.h>
#endif

void rtems_shutdown_executive( uint32_t result )
{
  Internal_errors_Source  source;
  bool                    is_internal;
  Internal_errors_t       code;

  if ( _System_state_Is_up( _System_state_Get() ) ) {
    #if defined(RTEMS_SMP)
      _SMP_Request_other_cores_to_shutdown();
    #endif

    source      = RTEMS_FATAL_SOURCE_EXIT;
    is_internal = false;
    code        = result;
  } else {
    source      = INTERNAL_ERROR_CORE;
    is_internal = true;
    code        = INTERNAL_ERROR_SHUTDOWN_WHEN_NOT_UP;
  }

  _Internal_error_Occurred( source, is_internal, code );

  /***************************************************************
   ***************************************************************
   *   SYSTEM SHUTS DOWN!!!  WE DO NOT RETURN TO THIS POINT!!!   *
   ***************************************************************
   ***************************************************************/
}
