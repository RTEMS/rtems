/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
  uint32_t    packets_per_second;

  ticks            = rtems_clock_get_ticks_since_boot();
  ticks_per_second = rtems_clock_get_ticks_per_second();

  seconds = ticks / ticks_per_second;
  if ( seconds == 0 )
    seconds = 1;

  packets_per_second = Shm_Receive_message_count / seconds;
  if ( (Shm_Receive_message_count % seconds) >= (seconds / 2) )
    packets_per_second++;

  printk( "\n\nSHMDR STATISTICS (NODE %" PRId32 ")\n",
    rtems_object_get_local_node() );
  printk( "TICKS SINCE BOOT = %" PRId32 "\n", ticks );
  printk( "TICKS PER SECOND = %" PRId32 "\n", ticks_per_second );
  printk( "ISRs=%" PRId32 "\n",     Shm_Interrupt_count );
  printk( "RECV=%" PRId32 "\n",     Shm_Receive_message_count );
  printk( "NULL=%" PRId32 "\n",     Shm_Null_message_count );
  printk( "PKTS/SEC=%" PRId32 "\n", packets_per_second );
}
