/*
 *  Internal Error Handler
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
#include <rtems/core/interr.h>
#include <rtems/core/sysstate.h>
#include <rtems/core/userext.h>

/*PAGE
 *
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

void volatile _Internal_error_Occurred(
  Internal_errors_Source  the_source,
  boolean                 is_internal,
  unsigned32              the_error
)
{

  Internal_errors_What_happened.the_source  = the_source;
  Internal_errors_What_happened.is_internal = is_internal;
  Internal_errors_What_happened.the_error   = the_error;

  _User_extensions_Fatal( the_source, is_internal, the_error );

  _System_state_Set( SYSTEM_STATE_FAILED );

  _CPU_Fatal_halt( the_error );

  /* will not return from this routine */
}
