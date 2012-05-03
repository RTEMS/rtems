/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <mcf5223x/mcf5223x.h>

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
