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
#include <string.h>

/*
 * Cacheable areas
 */
#define SDRAM_BASE      0
#define SDRAM_SIZE      (16*1024*1024)
#define FLASH_BASE      0xFF800000
#define FLASH_SIZE      (8*1024*1024)

/*
 * CPU-space access
 */
#define m68k_set_acr0(_acr0) __asm__ volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) __asm__ volatile ("movec %0,%%acr1" : : "d" (_acr1))

/*
 * Read/write copy of common cache
 *   Split I/D cache
 *   Allow CPUSHL to invalidate a cache line
 *   Enable buffered writes
 *   No burst transfers on non-cacheable accesses
 *   Default cache mode is *disabled* (cache only ACRx areas)
 */
static uint32_t cacr_mode = MCF5XXX_CACR_CENB |
                              MCF5XXX_CACR_DBWE |
                              MCF5XXX_CACR_DCM;
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

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialisation.
 */
void bsp_start( void )
{
  /*
   * Invalidate the cache and disable it
   */
  m68k_set_acr0(0);
  m68k_set_acr1(0);
  m68k_set_cacr(MCF5XXX_CACR_CINV);

  /*
   * Cache SDRAM and FLASH
   */
  m68k_set_acr0(MCF5XXX_ACR_AB(SDRAM_BASE)    |
                MCF5XXX_ACR_AM(SDRAM_SIZE-1)  |
                MCF5XXX_ACR_EN                |
                MCF5XXX_ACR_BWE               |
                MCF5XXX_ACR_SM_IGNORE);

  /*
   * Enable the cache
   */
  m68k_set_cacr(cacr_mode);
}

extern char _CPUClockSpeed[];

uint32_t get_CPU_clock_speed(void)
{
  return( (uint32_t)_CPUClockSpeed);
}
