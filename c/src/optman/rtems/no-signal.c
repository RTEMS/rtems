/*
 *  Signal Manager
 *
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
#include <rtems/rtems/status.h>
#include <rtems/core/isr.h>
#include <rtems/core/thread.h>

#include <rtems/rtems/asr.h>
#include <rtems/rtems/modes.h>
#include <rtems/rtems/signal.h>

void _Signal_Manager_initialization( void )
{
}
 
rtems_status_code rtems_signal_catch(
  rtems_asr_entry   handler,
  rtems_mode        mode_set
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_signal_send(
  rtems_id         id,
  rtems_signal_set signal_set
)
{
  return RTEMS_NOT_CONFIGURED;
}

/* end of file */
