/*
 *  This routine is invoked following a reset to report the statistics
 *  gathered during the previous execution.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <stdio.h>
#include <inttypes.h>
#include <rtems/bspIo.h>

#include "shm_driver.h"

void
Shm_Print_statistics(void)
{
  uint32_t    ticks;
  uint32_t    ticks_per_second;
  uint32_t    seconds;
  int         packets_per_second;

  ticks            = rtems_clock_get_ticks_since_boot();
  ticks_per_second = rtems_clock_get_ticks_per_second();

  seconds = ticks / ticks_per_second;
  if ( seconds == 0 )
    seconds = 1;

  packets_per_second = Shm_Receive_message_count / seconds;
  if ( (Shm_Receive_message_count % seconds) >= (seconds / 2) )
    packets_per_second++;

  printk( "\n\nSHMDR STATISTICS (NODE %" PRId32 ")\n",
    Multiprocessing_configuration.node );
  printk( "TICKS SINCE BOOT = %" PRId32 "\n", ticks );
  printk( "TICKS PER SECOND = %" PRId32 "\n", ticks_per_second );
  printk( "ISRs=%" PRId32 "\n",     Shm_Interrupt_count );
  printk( "RECV=%" PRId32 "\n",     Shm_Receive_message_count );
  printk( "NULL=%" PRId32 "\n",     Shm_Null_message_count );
  printk( "PKTS/SEC=%" PRId32 "\n", packets_per_second );
}
