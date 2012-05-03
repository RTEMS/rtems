/*
 *  Cache Management Support Routines for the i386
 */

#include <rtems.h>
#include <rtems/score/registers.h>
#include "cache_.h"

void _CPU_disable_cache(void) {
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

void _CPU_enable_cache(void) {
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
void _CPU_cache_flush_1_data_line(const void *d_addr) {}
void _CPU_cache_invalidate_1_data_line(const void *d_addr) {}
void _CPU_cache_freeze_data(void) {}
void _CPU_cache_unfreeze_data(void) {}
void _CPU_cache_invalidate_1_instruction_line ( const void *d_addr ) {}
void _CPU_cache_freeze_instruction(void) {}
void _CPU_cache_unfreeze_instruction(void) {}

void _CPU_cache_flush_entire_data(void)
{
  __asm__ volatile ("wbinvd");
}
void _CPU_cache_invalidate_entire_data(void)
{
  __asm__ volatile ("invd");
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
  __asm__ volatile ("invd");
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
