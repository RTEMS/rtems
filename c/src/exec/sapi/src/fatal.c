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
