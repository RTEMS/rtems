/**
 *  @file
 *
 *  Cache Management Support Routines for the MCF5282
 */

#include <rtems.h>
#include <mcf5282/mcf5282.h>   /* internal MCF5282 modules */
#include "cache.h"

/*
 * CPU-space access
 */
#define m68k_set_acr0(_acr0) \
  __asm__ volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) \
  __asm__ volatile ("movec %0,%%acr1" : : "d" (_acr1))

#define NOP __asm__ volatile ("nop");

/*
 * DEFAULT WHEN mcf5xxx_initialize_cacr not called
 *   Read/write copy of common cache
 *   Split I/D cache
 *   Allow CPUSHL to invalidate a cache line
 *   Enable buffered writes
 *   No burst transfers on non-cacheable accesses
 *   Default cache mode is *disabled* (cache only ACRx areas)
 */
static uint32_t cacr_mode = MCF5XXX_CACR_CENB |
                            MCF5XXX_CACR_DBWE |
                            MCF5XXX_CACR_DCM;

void mcf5xxx_initialize_cacr(uint32_t cacr)
{
  cacr_mode = cacr;
  m68k_set_cacr( cacr_mode );
}

/*
 * Cannot be frozen
 */
static void _CPU_cache_freeze_data(void) {}
static void _CPU_cache_unfreeze_data(void) {}
static void _CPU_cache_freeze_instruction(void) {}
static void _CPU_cache_unfreeze_instruction(void) {}

/*
 * Write-through data cache -- flushes are unnecessary
 */
static void _CPU_cache_flush_1_data_line(const void *d_addr) {}
static void _CPU_cache_flush_entire_data(void) {}

static void _CPU_cache_enable_instruction(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
    cacr_mode &= ~MCF5XXX_CACR_DIDI;
    m68k_set_cacr(cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVI );
    NOP;
  rtems_interrupt_enable(level);
}

static void _CPU_cache_disable_instruction(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
    cacr_mode |= MCF5XXX_CACR_DIDI;
    m68k_set_cacr(cacr_mode);
  rtems_interrupt_enable(level);
}

static void _CPU_cache_invalidate_entire_instruction(void)
{
  m68k_set_cacr(cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVI);
  NOP;
}

static void _CPU_cache_invalidate_1_instruction_line(const void *addr)
{
  /*
   * Top half of cache is I-space
   */
  addr = (void *)((int)addr | 0x400);
  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (addr));
}

static void _CPU_cache_enable_data(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
    cacr_mode &= ~MCF5XXX_CACR_DISD;
    m68k_set_cacr(cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVD);
  rtems_interrupt_enable(level);
}

static void _CPU_cache_disable_data(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
    cacr_mode |= MCF5XXX_CACR_DISD;
    m68k_set_cacr(cacr_mode);
  rtems_interrupt_enable(level);
}

static void _CPU_cache_invalidate_entire_data(void)
{
  m68k_set_cacr(cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVD);
}

static void _CPU_cache_invalidate_1_data_line(const void *addr)
{
  /*
   * Bottom half of cache is D-space
   */
  addr = (void *)((int)addr & ~0x400);
  __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (addr));
}

#include "../../../shared/cache/cacheimpl.h"
