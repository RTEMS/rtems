/*  Delayed_routine
 *
 *  This routine is used as the timer routine for Timer Manager tests.
 *
 *  Input parameters:
 *    ignored - this parameter is ignored
 *
 *  Output parameters:  NONE
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

#include "system.h"

rtems_timer_service_routine Delayed_routine(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
}
