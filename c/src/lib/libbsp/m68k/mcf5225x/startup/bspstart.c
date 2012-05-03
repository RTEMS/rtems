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
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

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
void __attribute__((weak)) bsp_start(void)
{
}

uint32_t  __attribute__((weak)) bsp_get_CPU_clock_speed(void)
{
  #define DEF_CLOCK_SPEED 8000000.0F //8.0 MHz
  #define MCF_MFD0_2_MASK 0x7000U
  #define MCF_RFD0_2_MASK 0x0700U
  #define MCF_MFD0_2_OFFSET 4U

  #define SPEED_BIAS  ((((MCF_CLOCK_SYNCR & MCF_MFD0_2_MASK) >> 11) + MCF_MFD0_2_OFFSET) / (float)(((MCF_CLOCK_SYNCR & MCF_RFD0_2_MASK)>>7) ? : 1.0F))

  return MCF_CLOCK_SYNCR & MCF_CLOCK_SYNCR_PLLEN ? SPEED_BIAS * DEF_CLOCK_SPEED : DEF_CLOCK_SPEED;
}
