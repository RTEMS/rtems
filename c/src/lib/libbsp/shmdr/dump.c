/*
 *  This routine is invoked following a reset to report the statistics
 *  gathered during the previous execution.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
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

#include <rtems.h>
#include <stdio.h>

#include "shm.h"

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
