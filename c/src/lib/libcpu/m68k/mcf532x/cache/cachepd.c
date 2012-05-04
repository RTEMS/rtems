/**
 *  @file
 *
 *  Cache Management Support Routines for the MCF532x
 */

#include <rtems.h>
#include <mcf532x/mcf532x.h>

#define m68k_set_cacr(_cacr) __asm__ volatile ("movec %0,%%cacr" : : "d" (_cacr))

/*
 * Read/write copy of common cache
 *  Default cache mode is *disabled* (cache only ACRx areas)
 *  Allow CPUSHL to invalidate a cache line
 *  Enable store buffer
 */
static uint32_t cacr_mode = MCF_CACR_ESB |
                              MCF_CACR_DCM(3);

/*
 * Cannot be frozen
 */
void _CPU_cache_freeze_data(void)
{
}

void _CPU_cache_unfreeze_data(void)
{
}

void _CPU_cache_freeze_instruction(void)
{
}

void _CPU_cache_unfreeze_instruction(void)
{
}

void _CPU_cache_flush_1_data_line(const void *d_addr)
{
  register unsigned long adr = (((unsigned long) d_addr >> 4) & 0xff) << 4;

  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
  adr += 1;
  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
  adr += 1;
  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
  adr += 1;
  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
}

void _CPU_cache_flush_entire_data(void)
{
  register unsigned long set, adr;

  for(set = 0; set < 256; ++set) {
    adr = (set << 4);
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
    adr += 1;
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
    adr += 1;
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
    adr += 1;
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
  }
}

void _CPU_cache_enable_instruction(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  if(!(cacr_mode & MCF_CACR_CENB))
  {
    cacr_mode |= MCF_CACR_CENB;
    m68k_set_cacr(cacr_mode);
  }
  rtems_interrupt_enable(level);
}

void _CPU_cache_disable_instruction(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  if((cacr_mode & MCF_CACR_CENB))
  {
    cacr_mode &= ~MCF_CACR_CENB;
    m68k_set_cacr(cacr_mode);
  }
  rtems_interrupt_enable(level);
}

void _CPU_cache_invalidate_entire_instruction(void)
{
  m68k_set_cacr(cacr_mode | MCF_CACR_CINVA);
}

void _CPU_cache_invalidate_1_instruction_line(const void *addr)
{
  register unsigned long adr = (((unsigned long) addr >> 4) & 0xff) << 4;

  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
  adr += 1;
  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
  adr += 1;
  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
  adr += 1;
  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (adr));
}

void _CPU_cache_enable_data(void)
{
  /*
   * The 532x has a unified data and instruction cache, so we call through
   * to enable instruction.
   */
  _CPU_cache_enable_instruction();
}

void _CPU_cache_disable_data(void)
{
  /*
   * The 532x has a unified data and instruction cache, so we call through
   * to disable instruction.
   */
  _CPU_cache_disable_instruction();
}

void _CPU_cache_invalidate_entire_data(void)
{
  _CPU_cache_invalidate_entire_instruction();
}

void _CPU_cache_invalidate_1_data_line(const void *addr)
{
  _CPU_cache_invalidate_1_instruction_line(addr);
}
