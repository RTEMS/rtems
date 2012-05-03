/*
 *  Internal Error Handler
 *
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/interr.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/userext.h>

/*
 *  _Internal_error_Occurred
 *
 *  This routine will invoke the fatal error handler supplied by the user
 *  followed by the the default one provided by the executive.  The default
 *  error handler assumes no hardware is present to help inform the user
 *  of the problem.  Halt stores the error code in a known register,
 *  disables interrupts, and halts the CPU.  If the CPU does not have a
 *  halt instruction, it will loop to itself.
 *
 *  Input parameters:
 *    the_source  - what subsystem the error originated in
 *    is_internal - if the error was internally generated
 *    the_error   - fatal error status code
 *
 *  Output parameters:
 *    As much information as possible is stored in a CPU dependent fashion.
 *    See the CPU dependent code for more information.
 *
 *  NOTE: The the_error is not necessarily a directive status code.
 */

void _Internal_error_Occurred(
  Internal_errors_Source  the_source,
  bool                    is_internal,
  Internal_errors_t       the_error
)
{

  _Internal_errors_What_happened.the_source  = the_source;
  _Internal_errors_What_happened.is_internal = is_internal;
  _Internal_errors_What_happened.the_error   = the_error;

  _User_extensions_Fatal( the_source, is_internal, the_error );

  _System_state_Set( SYSTEM_STATE_FAILED );

  _CPU_Fatal_halt( the_error );

  /* will not return from this routine */
  while (true);
}
