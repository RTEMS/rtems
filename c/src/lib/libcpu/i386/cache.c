/*
 *  Cache Management Support Routines for the i386
 *
 *  $Id$
 */

#include <rtems.h>
#include <libcpu/registers.h>
#include "cache_.h"

void _CPU_disable_cache() {
  cr0 regCr0;

  regCr0.i = i386_get_cr0();
  regCr0.cr0.page_level_cache_disable = 1;
  regCr0.cr0.no_write_through = 1;
  i386_set_cr0( regCr0.i );
  rtems_flush_entire_data_cache();
}

/*
 * Enable the entire cache
 */

void _CPU_enable_cache() {
  cr0 regCr0;

  regCr0.i = i386_get_cr0();
  regCr0.cr0.page_level_cache_disable = 0;
  regCr0.cr0.no_write_through = 0;
  i386_set_cr0( regCr0.i );
  /*rtems_flush_entire_data_cache();*/
}

/*
 * CACHE MANAGER: The following functions are CPU-specific.
 * They provide the basic implementation for the rtems_* cache
 * management routines. If a given function has no meaning for the CPU,
 * it does nothing by default.
 *
 * FIXME: Definitions for I386_CACHE_ALIGNMENT are missing above for
 *        each CPU. The routines below should be implemented per CPU,
 *        to accomodate the capabilities of each.
 */

/* FIXME: I don't belong here. */
#define I386_CACHE_ALIGNMENT 16

#if defined(I386_CACHE_ALIGNMENT)
#define _CPU_DATA_CACHE_ALIGNMENT I386_CACHE_ALIGNMENT
#define _CPU_INST_CACHE_ALIGNEMNT I386_CACHE_ALIGNMENT

void _CPU_flush_1_data_cache_line(const void *d_addr) {}
void _CPU_invalidate_1_data_cache_line(const void *d_addr) {}
void _CPU_freeze_data_cache(void) {}
void _CPU_unfreeze_data_cache(void) {}
void _CPU_invalidate_1_inst_cache_line ( const void *d_addr ) {}
void _CPU_freeze_inst_cache(void) {}
void _CPU_unfreeze_inst_cache(void) {}

void _CPU_flush_entire_data_cache(
  const void * d_addr
)
{
  asm volatile ("wbinvd");
}
void _CPU_invalidate_entire_data_cache(
  const void * d_addr
)
{
  asm volatile ("invd");
}

void _CPU_enable_data_cache(void)
{
        _CPU_enable_cache();
}

void _CPU_disable_data_cache(void)
{
        _CPU_disable_cache();
}

void _CPU_invalidate_entire_inst_cache(void)
{
  asm volatile ("invd");
}

void _CPU_enable_inst_cache(void)
{
  _CPU_enable_cache();
}

void _CPU_disable_inst_cache( void )
{
  _CPU_disable_cache();
}
#endif

