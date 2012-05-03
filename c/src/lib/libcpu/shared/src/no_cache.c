/*
 *  STUB Cache Management Support Routines
 */

#include <rtems.h>
#include "cache_.h"

/*
 * CACHE MANAGER: The following functions should be implemented
 * properly for each CPU if it has a cache.  These provide the basic
 * implementation for the rtems_* cache management routines. If a given
 * function has no meaning for the CPU, then just let your implementation
 * do nothing.
 * 
 * This is just a series of stubs.
 */

void _CPU_cache_freeze_data ( void ) {}
void _CPU_cache_unfreeze_data ( void ) {}
void _CPU_cache_freeze_instruction ( void ) {}
void _CPU_cache_unfreeze_instruction ( void ) {}

void _CPU_cache_flush_1_data_line (
  const void * d_addr )
{
}

void _CPU_cache_invalidate_1_data_line (
  const void * d_addr )
{
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
