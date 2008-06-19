/*
 *  BSP startup
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  Author:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 * 
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

/*
 * Cannot be frozen
 */
void _CPU_cache_freeze_data(void) {}
void _CPU_cache_unfreeze_data(void) {}
void _CPU_cache_freeze_instruction(void) {}
void _CPU_cache_unfreeze_instruction(void) {}

/*
 * Write-through data cache -- flushes are unnecessary
 */
void _CPU_cache_flush_1_data_line(const void *d_addr) {}
void _CPU_cache_flush_entire_data(void) {}

void _CPU_cache_enable_instruction(void) {}
void _CPU_cache_disable_instruction(void) {}
void _CPU_cache_invalidate_entire_instruction(void) {}
void _CPU_cache_invalidate_1_instruction_line(const void *addr) {}

void _CPU_cache_enable_data(void) {}
void _CPU_cache_disable_data(void) {}
void _CPU_cache_invalidate_entire_data(void) {}
void _CPU_cache_invalidate_1_data_line(const void *addr) {}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialisation.
 */
void bsp_start(void)
{
}

uint32_t bsp_get_CPU_clock_speed(void)
{
  return 60000000;
}
