/*
 *  Fatal Error Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/fatal.h>
#include <rtems/score/interr.h>

/*PAGE
 *
 *  rtems_fatal_error_occurred
 *
 *  This directive will invoke the internal fatal error handler.
 *
 *  Input parameters:
 *    the_error - fatal error status code
 *
 *  Output parameters: NONE
 */

void volatile rtems_fatal_error_occurred(
  unsigned32 the_error
)
{
  _Internal_error_Occurred( INTERNAL_ERROR_RTEMS_API, FALSE, the_error );

/* will not return from this routine */
}
