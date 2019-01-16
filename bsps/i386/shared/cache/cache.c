/*
 *  Cache Management Support Routines for the i386
 */

#include <rtems.h>
#include <rtems/score/cpu.h>
#include <libcpu/page.h>

#define I386_CACHE_ALIGNMENT 16
#define CPU_DATA_CACHE_ALIGNMENT I386_CACHE_ALIGNMENT
#define CPU_INSTRUCTION_CACHE_ALIGNMENT I386_CACHE_ALIGNMENT

void _CPU_disable_cache(void)
{
  unsigned int regCr0;

  regCr0 = i386_get_cr0();
  regCr0 |= CR0_PAGE_LEVEL_CACHE_DISABLE;
  regCr0 |= CR0_NO_WRITE_THROUGH;
  i386_set_cr0( regCr0 );
  rtems_cache_flush_entire_data();
}

/*
 * Enable the entire cache
 */

void _CPU_enable_cache(void)
{
  unsigned int regCr0;

  regCr0 = i386_get_cr0();
  regCr0 &= ~(CR0_PAGE_LEVEL_CACHE_DISABLE);
  regCr0 &= ~(CR0_NO_WRITE_THROUGH);
  i386_set_cr0( regCr0 );
  /*rtems_cache_flush_entire_data();*/
}

/*
 * CACHE MANAGER: The following functions are CPU-specific.
 * They provide the basic implementation for the rtems_* cache
 * management routines. If a given function has no meaning for the CPU,
 * it does nothing by default.
 *
 * FIXME: The routines below should be implemented per CPU,
 *        to accomodate the capabilities of each.
 */

#if defined(I386_CACHE_ALIGNMENT)
static void _CPU_cache_flush_1_data_line(const void *d_addr) {}
static void _CPU_cache_invalidate_1_data_line(const void *d_addr) {}
static void _CPU_cache_freeze_data(void) {}
static void _CPU_cache_unfreeze_data(void) {}
static void _CPU_cache_flush_entire_data(void)
{
  __asm__ volatile ("wbinvd");
}

static void _CPU_cache_invalidate_entire_data(void)
{
  __asm__ volatile ("invd");
}

static void _CPU_cache_invalidate_entire_instruction(void)
{
  __asm__ volatile ("invd");
}

static void _CPU_cache_invalidate_1_instruction_line(const void *i_addr)
{
  _CPU_cache_invalidate_entire_instruction();
}

static void _CPU_cache_enable_data(void)
{
  _CPU_enable_cache();
}

static void _CPU_cache_disable_data(void)
{
  _CPU_disable_cache();
}

static void _CPU_cache_enable_instruction(void)
{
  _CPU_enable_cache();
}

static void _CPU_cache_disable_instruction(void)
{
  _CPU_disable_cache();
}

static void _CPU_cache_freeze_instruction(void)
{
}

static void _CPU_cache_unfreeze_instruction(void)
{
}

#endif

#include "../../../shared/cache/cacheimpl.h"
