/*
 *  Fatal Error Manager
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
#include <rtems/config.h>
#include <rtems/fatal.h>
#include <rtems/sysstate.h>
#include <rtems/userext.h>

/*PAGE
 *
 *  rtems_fatal_error_occurred
 *
 *  This directive will invoke the fatal error handler supplied by the user
 *  followed by the the default one provided by the executive.  The default
 *  error handler assumes no hardware is present to help inform the user
 *  of the problem.  Halt stores the error code in a known register,
 *  disables interrupts, and halts the CPU.  If the CPU does not have a
 *  halt instruction, it will loop to itself.
 *
 *  Input parameters:
 *    the_error - fatal error status code
 *
 *  Output parameters:
 *    the_error       - on stack
 *    status register - on stack
 *
 *  NOTE: The the_error is not necessarily a directive status code.
 */

void volatile rtems_fatal_error_occurred(
  unsigned32 the_error
)
{

  _User_extensions_Fatal( the_error );

  _System_state_Set( SYSTEM_STATE_FAILED );

  _CPU_Fatal_halt( the_error );

/* will not return from this routine */
}
