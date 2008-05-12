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
#include <string.h>
 
/*
 * Cacheable areas
 */
#define SDRAM_BASE      0
#define SDRAM_SIZE      (16*1024*1024)

/*
 * CPU-space access
 */
#define m68k_set_cacr(_cacr) asm volatile ("movec %0,%%cacr" : : "d" (_cacr))
#define m68k_set_acr0(_acr0) asm volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) asm volatile ("movec %0,%%acr1" : : "d" (_acr1))

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
    asm volatile ("cpushl %%bc,(%0)" :: "a" (addr));
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
    asm volatile ("cpushl %%bc,(%0)" :: "a" (addr));
}

/*
 *  Use the shared implementations of the following routines
 */
void bsp_libc_init( void *, uint32_t, int );
void bsp_pretasking_hook(void);			/* m68k version */

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialisation.
 */
void bsp_start( void )
{
  extern char _WorkspaceBase[];
  extern char _RamSize[];
  extern unsigned long  _M68k_Ramsize;

  _M68k_Ramsize = (unsigned long)_RamSize;		/* RAM size set in linker script */

  /*
   *  Allocate the memory for the RTEMS Work Space.  This can come from
   *  a variety of places: hard coded address, malloc'ed from outside
   *  RTEMS world (e.g. simulator or primitive memory manager), or (as
   *  typically done by stock BSPs) by subtracting the required amount
   *  of work space from the last physical address on the CPU board.
   */

  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */

  Configuration.work_space_start = (void *)_WorkspaceBase;

  /*
   * Invalidate the cache and disable it
   */
  m68k_set_acr0(0);
  m68k_set_acr1(0);
  m68k_set_cacr(MCF5XXX_CACR_CINV);

  /*
   * Cache SDRAM
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

uint32_t get_CPU_clock_speed(void)
{
  extern char _CPUClockSpeed[];
  return( (uint32_t)_CPUClockSpeed);
}
