/*
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

rtems_timer_service_routine Timer_handler(
  rtems_id argument
)
{
}

rtems_asr Isr_handler(
  rtems_signal_set signals
)
{
}

rtems_asr Asr_handler(
  rtems_signal_set signals
)
{
}

rtems_task task_func() {}

void null_func() {}

rtems_status_code Empty_directive()
{
  return( RTEMS_SUCCESSFUL );
}
