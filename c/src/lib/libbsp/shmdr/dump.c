/*
 *  This routine is invoked following a reset to report the statistics
 *  gathered during the previous execution.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
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

#include <rtems.h>
#include <stdio.h>

#include "shm_driver.h"

void
Shm_Print_statistics(void)
{
  rtems_unsigned32  ticks;
  rtems_unsigned32  ticks_per_second;
  rtems_unsigned32  seconds;
  int               packets_per_second;

  (void) rtems_clock_get( RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &ticks );
  (void) rtems_clock_get( RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second );

  seconds = ticks / ticks_per_second;
  if ( seconds == 0 )
    seconds = 1;

  packets_per_second = Shm_Receive_message_count / seconds;
  if ( (Shm_Receive_message_count % seconds) >= (seconds / 2) )
    packets_per_second++;

  printf( "\n\nSHMDR STATISTICS (NODE %d)\n", Shm_Local_node );
  printf( "TICKS SINCE BOOT = %d\n", ticks );
  printf( "TICKS PER SECOND = %d\n", ticks_per_second );
  printf( "ISRs=%d\n",     Shm_Interrupt_count );
  printf( "RECV=%d\n",     Shm_Receive_message_count );
  printf( "NULL=%d\n",     Shm_Null_message_count );
  printf( "PKTS/SEC=%d\n", packets_per_second );
}
