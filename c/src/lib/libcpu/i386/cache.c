/*
 *  Cache Management Support Routines for the i386
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/score/registers.h>
#include "cache_.h"

void _CPU_disable_cache() {
  cr0 regCr0;

  regCr0.i = i386_get_cr0();
  regCr0.cr0.page_level_cache_disable = 1;
  regCr0.cr0.no_write_through = 1;
  i386_set_cr0( regCr0.i );
  rtems_cache_flush_entire_data();
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
void _CPU_cache_flush_1_data_line(const void *d_addr) {}
void _CPU_cache_invalidate_1_data_line(const void *d_addr) {}
void _CPU_cache_freeze_data(void) {}
void _CPU_cache_unfreeze_data(void) {}
void _CPU_cache_invalidate_1_instruction_line ( const void *d_addr ) {}
void _CPU_cache_freeze_instruction(void) {}
void _CPU_cache_unfreeze_instruction(void) {}

void _CPU_cache_flush_entire_data(void)
{
  asm volatile ("wbinvd");
}
void _CPU_cache_invalidate_entire_data(void)
{
  asm volatile ("invd");
}

void _CPU_cache_enable_data(void)
{
        _CPU_enable_cache();
}

void _CPU_cache_disable_data(void)
{
        _CPU_disable_cache();
}

void _CPU_cache_invalidate_entire_instruction(void)
{
  asm volatile ("invd");
}

void _CPU_cache_enable_instruction(void)
{
  _CPU_enable_cache();
}

void _CPU_cache_disable_instruction( void )
{
  _CPU_disable_cache();
}
#endif

