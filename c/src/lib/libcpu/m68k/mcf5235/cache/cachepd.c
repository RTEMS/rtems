/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <mcf5235/mcf5235.h>

/*
 *  Default value for the cacr is set by the BSP
 */
extern uint32_t cacr_mode;

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

void _CPU_cache_enable_instruction(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    cacr_mode &= ~MCF5XXX_CACR_DIDI;
    m68k_set_cacr(cacr_mode);
    rtems_interrupt_enable(level);
}

void _CPU_cache_disable_instruction(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    cacr_mode |= MCF5XXX_CACR_DIDI;
    m68k_set_cacr(cacr_mode);
    rtems_interrupt_enable(level);
}

void _CPU_cache_invalidate_entire_instruction(void)
{
    m68k_set_cacr(cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVI);
}

void _CPU_cache_invalidate_1_instruction_line(const void *addr)
{
    /*
     * Top half of cache is I-space
     */
    addr = (void *)((int)addr | 0x400);
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (addr));
}

void _CPU_cache_enable_data(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    cacr_mode &= ~MCF5XXX_CACR_DISD;
    m68k_set_cacr(cacr_mode);
    rtems_interrupt_enable(level);
}

void _CPU_cache_disable_data(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    cacr_mode |= MCF5XXX_CACR_DISD;
    m68k_set_cacr(cacr_mode);
    rtems_interrupt_enable(level);
}

void _CPU_cache_invalidate_entire_data(void)
{
    m68k_set_cacr(cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVD);
}

void _CPU_cache_invalidate_1_data_line(const void *addr)
{
    /*
     * Bottom half of cache is D-space
     */
    addr = (void *)((int)addr & ~0x400);
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (addr));
}
