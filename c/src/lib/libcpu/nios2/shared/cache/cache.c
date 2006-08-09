/*
 *  Cache Management Support Routines for the MC68040
 *
 *  $Id$
 */

#include <rtems.h>
#include "cache_.h"

/*
 * CACHE MANAGER: The following functions are CPU-specific.
 * They provide the basic implementation for the rtems_* cache
 * management routines. If a given function has no meaning for the CPU,
 * it does nothing by default.
 */

void _CPU_cache_freeze_data ( void ) {}
void _CPU_cache_unfreeze_data ( void ) {}
void _CPU_cache_freeze_instruction ( void ) {}
void _CPU_cache_unfreeze_instruction ( void ) {}

void _CPU_cache_flush_1_data_line (
  const void * d_addr )
{
  void * p_address = (void *) _CPU_virtual_to_physical( d_addr );
}

void _CPU_cache_invalidate_1_data_line (
  const void * d_addr )
{
  void * p_address = (void *) _CPU_virtual_to_physical( d_addr );
}

void _CPU_cache_flush_entire_data ( void )
{
}

void _CPU_cache_invalidate_entire_data ( void )
{
}

void _CPU_cache_enable_data ( void )
{
}

void _CPU_cache_disable_data ( void )
{
}

void _CPU_cache_invalidate_1_instruction_line (
  const void * i_addr )
{
  void * p_address = (void *)  _CPU_virtual_to_physical( i_addr );
}

void _CPU_cache_invalidate_entire_instruction ( void )
{
}

void _CPU_cache_enable_instruction ( void )
{
}

void _CPU_cache_disable_instruction ( void )
{
}
/* end of file */
