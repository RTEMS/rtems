/*
 *  Cache Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  The functions in this file implement the API to the RTEMS Cache Manager and
 *  are divided into data cache and instruction cache functions. Data cache
 *  functions only have bodies if a data cache is supported. Instruction
 *  cache functions only have bodies if an instruction cache is supported.
 *  Support for a particular cache exists only if CPU_x_CACHE_ALIGNMENT is
 *  defined, where x E {DATA, INSTRUCTION}. These definitions are found in
 *  the Cache Manager Wrapper header files, often
 *
 *  rtems/c/src/lib/libcpu/CPU/cache_.h
 *
 *  The functions below are implemented with CPU dependent inline routines
 *  found in the cache.c files for each CPU. In the event that a CPU does
 *  not support a specific function for a cache it has, the CPU dependent
 *  routine does nothing (but does exist).
 *
 *  At this point, the Cache Manager makes no considerations, and provides no
 *  support for BSP specific issues such as a secondary cache. In such a system,
 *  the CPU dependent routines would have to be modified, or a BSP layer added
 *  to this Manager.
 */

#include <rtems.h>
#include "cache_.h"

/*
 * THESE FUNCTIONS ONLY HAVE BODIES IF WE HAVE A DATA CACHE
 */

/*
 * This function is called to flush the data cache by performing cache
 * copybacks. It must determine how many cache lines need to be copied
 * back and then perform the copybacks.
 */
void
rtems_cache_flush_multiple_data_lines( const void * d_addr, size_t n_bytes )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  const void * final_address;

 /*
  * Set d_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be pushed. Increment d_addr and push
  * the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to flush is zero */
    return;

  final_address = (void *)((size_t)d_addr + n_bytes - 1);
  d_addr = (void *)((size_t)d_addr & ~(CPU_DATA_CACHE_ALIGNMENT - 1));
  while( d_addr <= final_address )  {
    _CPU_cache_flush_1_data_line( d_addr );
    d_addr = (void *)((size_t)d_addr + CPU_DATA_CACHE_ALIGNMENT);
  }
#endif
}


/*
 * This function is responsible for performing a data cache invalidate.
 * It must determine how many cache lines need to be invalidated and then
 * perform the invalidations.
 */

void
rtems_cache_invalidate_multiple_data_lines( const void * d_addr, size_t n_bytes )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  const void * final_address;

 /*
  * Set d_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be invalidated. Increment d_addr and
  * invalidate the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to invalidate is zero */
    return;

  final_address = (void *)((size_t)d_addr + n_bytes - 1);
  d_addr = (void *)((size_t)d_addr & ~(CPU_DATA_CACHE_ALIGNMENT - 1));
  while( final_address >= d_addr ) {
    _CPU_cache_invalidate_1_data_line( d_addr );
    d_addr = (void *)((size_t)d_addr + CPU_DATA_CACHE_ALIGNMENT);
  }
#endif
}


/*
 * This function is responsible for performing a data cache flush.
 * It flushes the entire cache.
 */
void
rtems_cache_flush_entire_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
   /*
    * Call the CPU-specific routine
    */
   _CPU_cache_flush_entire_data();
#endif
}


/*
 * This function is responsible for performing a data cache
 * invalidate. It invalidates the entire cache.
 */
void
rtems_cache_invalidate_entire_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
 /*
  * Call the CPU-specific routine
  */

 _CPU_cache_invalidate_entire_data();
#endif
}


/*
 * This function returns the data cache granularity.
 */
int
rtems_cache_get_data_line_size( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  return CPU_DATA_CACHE_ALIGNMENT;
#else
  return 0;
#endif
}


/*
 * This function freezes the data cache; cache lines
 * are not replaced.
 */
void
rtems_cache_freeze_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_freeze_data();
#endif
}


/*
 * This function unfreezes the instruction cache.
 */
void rtems_cache_unfreeze_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_unfreeze_data();
#endif
}


/* Turn on the data cache. */
void
rtems_cache_enable_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_enable_data();
#endif
}


/* Turn off the data cache. */
void
rtems_cache_disable_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_disable_data();
#endif
}



/*
 * THESE FUNCTIONS ONLY HAVE BODIES IF WE HAVE AN INSTRUCTION CACHE
 */

/*
 * This function is responsible for performing an instruction cache
 * invalidate. It must determine how many cache lines need to be invalidated
 * and then perform the invalidations.
 */
void
rtems_cache_invalidate_multiple_instruction_lines( const void * i_addr, size_t n_bytes )
{
#if CPU_INSTRUCTION_CACHE_ALIGNMENT
  const void * final_address;

 /*
  * Set i_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be invalidated. Increment i_addr and
  * invalidate the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to invalidate is zero */
    return;

  final_address = (void *)((size_t)i_addr + n_bytes - 1);
  i_addr = (void *)((size_t)i_addr & ~(CPU_INSTRUCTION_CACHE_ALIGNMENT - 1));
  while( final_address > i_addr ) {
    _CPU_cache_invalidate_1_instruction_line( i_addr );
    i_addr = (void *)((size_t)i_addr + CPU_INSTRUCTION_CACHE_ALIGNMENT);
  }
#endif
}


/*
 * This function is responsible for performing an instruction cache
 * invalidate. It invalidates the entire cache.
 */
void
rtems_cache_invalidate_entire_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
 /*
  * Call the CPU-specific routine
  */

 _CPU_cache_invalidate_entire_instruction();
#endif
}


/*
 * This function returns the instruction cache granularity.
 */
int
rtems_cache_get_instruction_line_size( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  return CPU_INSTRUCTION_CACHE_ALIGNMENT;
#else
  return 0;
#endif
}


/*
 * This function freezes the instruction cache; cache lines
 * are not replaced.
 */
void
rtems_cache_freeze_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_freeze_instruction();
#endif
}


/*
 * This function unfreezes the instruction cache.
 */
void rtems_cache_unfreeze_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_unfreeze_instruction();
#endif
}


/* Turn on the instruction cache. */
void
rtems_cache_enable_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_enable_instruction();
#endif
}


/* Turn off the instruction cache. */
void
rtems_cache_disable_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_disable_instruction();
#endif
}
