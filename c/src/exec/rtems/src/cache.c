/*  cache.c
 *
 *  Cache Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  
 *  The functions in this file define the API to the RTEMS Cache Manager and
 *  are divided into data cache and instruction cache functions. Data cache
 *  functions are only declared if a data cache is supported. Instruction
 *  cache functions are only declared if an instruction cache is supported.
 *  Support for a particular cache exists only if _CPU_x_CACHE_ALIGNMENT is
 *  defined, where x E {DATA, INST}. These definitions are found in the CPU
 *  dependent source files in the supercore, often
 *  
 *  rtems/c/src/exec/score/cpu/CPU/rtems/score/CPU.h
 *  
 *  The functions below are implemented with CPU dependent inline routines
 *  also found in the above file. In the event that a CPU does not support a
 *  specific function, the CPU dependent routine does nothing (but does exist).
 *  
 *  At this point, the Cache Manager makes no considerations, and provides no
 *  support for BSP specific issues such as a secondary cache. In such a system,
 *  the CPU dependent routines would have to be modified, or a BSP layer added
 *  to this Manager.
 */

#include <rtems/system.h>
#include <sys/types.h>
#include <rtems/rtems/cache.h>


/*
 * THESE FUNCTIONS ONLY EXIST IF WE HAVE A DATA CACHE
 */
#if defined(_CPU_DATA_CACHE_ALIGNMENT)

/*
 * This function is called to flush the data cache by performing cache
 * copybacks. It must determine how many cache lines need to be copied
 * back and then perform the copybacks.
 */
void
rtems_flush_multiple_data_cache_lines( const void * d_addr, size_t n_bytes )
{
    const void * final_address;
   /*
    * Set d_addr to the beginning of the cache line; final_address indicates
    * the last address_t which needs to be pushed. Increment d_addr and push
    * the resulting line until final_address is passed.
    */
    final_address = (void *)((size_t)d_addr + n_bytes - 1);
    d_addr = (void *)((size_t)d_addr & ~(_CPU_DATA_CACHE_ALIGNMENT - 1));
    while( d_addr <= final_address )  {
        _CPU_flush_1_data_cache_line( d_addr );
        d_addr = (void *)((size_t)d_addr + _CPU_DATA_CACHE_ALIGNMENT);
    }
}


/*
 * This function is responsible for performing a data cache invalidate.
 * It must determine how many cache lines need to be invalidated and then
 * perform the invalidations.
 */
void
rtems_invalidate_multiple_data_cache_lines( const void * d_addr, size_t n_bytes )
{
    const void * final_address;
   /*
    * Set d_addr to the beginning of the cache line; final_address indicates
    * the last address_t which needs to be invalidated. Increment d_addr and
    * invalidate the resulting line until final_address is passed.
    */
    final_address = (void *)((size_t)d_addr + n_bytes - 1);
    d_addr = (void *)((size_t)d_addr & ~(_CPU_DATA_CACHE_ALIGNMENT - 1));
    while( final_address > d_addr ) {
        _CPU_invalidate_1_data_cache_line( d_addr );
        d_addr = (void *)((size_t)d_addr + _CPU_DATA_CACHE_ALIGNMENT);
    }
}


/*
 * This function is responsible for performing a data cache flush.
 * It flushes the entire cache.
 */
void
rtems_flush_entire_data_cache( void )
{
   /*
    * Call the CPU-specific routine
    */
   _CPU_flush_entire_data_cache();
      
}


/*
 * This function is responsible for performing a data cache
 * invalidate. It invalidates the entire cache.
 */
void
rtems_invalidate_entire_data_cache( void )
{
   /*
    * Call the CPU-specific routine
    */
   _CPU_invalidate_entire_data_cache();
}


/*
 * This function returns the data cache granularity.
 */
int
rtems_get_data_cache_line_size( void )
{
	return _CPU_DATA_CACHE_ALIGNMENT;
}


/*
 * This function freezes the data cache; cache lines
 * are not replaced.
 */
void
rtems_freeze_data_cache( void )
{
	_CPU_freeze_data_cache();
}


/*
 * This function unfreezes the instruction cache.
 */
void rtems_unfreeze_data_cache( void )
{
	_CPU_unfreeze_data_cache();
}


/* Turn on the data cache. */
void
rtems_enable_data_cache( void )
{
	_CPU_enable_data_cache();
}


/* Turn off the data cache. */
void
rtems_disable_data_cache( void )
{
	_CPU_disable_data_cache();
}
#endif



/*
 * THESE FUNCTIONS ONLY EXIST IF WE HAVE AN INSTRUCTION CACHE
 */
#if defined(_CPU_INST_CACHE_ALIGNMENT)

/*
 * This function is responsible for performing an instruction cache
 * invalidate. It must determine how many cache lines need to be invalidated
 * and then perform the invalidations.
 */
void
rtems_invalidate_multiple_inst_cache_lines( const void * i_addr, size_t n_bytes )
{
    const void * final_address;
   /*
    * Set i_addr to the beginning of the cache line; final_address indicates
    * the last address_t which needs to be invalidated. Increment i_addr and
    * invalidate the resulting line until final_address is passed.
    */
    final_address = (void *)((size_t)i_addr + n_bytes - 1);
    i_addr = (void *)((size_t)i_addr & ~(_CPU_INST_CACHE_ALIGNMENT - 1));
    while( final_address > i_addr ) {
        _CPU_invalidate_1_inst_cache_line( i_addr );
        i_addr = (void *)((size_t)i_addr + _CPU_INST_CACHE_ALIGNMENT);
    }
}


/*
 * This function is responsible for performing an instruction cache
 * invalidate. It invalidates the entire cache.
 */
void
rtems_invalidate_entire_inst_cache( void )
{
   /*
    * Call the CPU-specific routine
    */
   _CPU_invalidate_entire_inst_cache();
}


/*
 * This function returns the instruction cache granularity.
 */
int
rtems_get_inst_cache_line_size( void )
{
	return _CPU_INST_CACHE_ALIGNMENT;
}


/*
 * This function freezes the instruction cache; cache lines
 * are not replaced.
 */
void
rtems_freeze_inst_cache( void )
{
	_CPU_freeze_inst_cache();
}


/*
 * This function unfreezes the instruction cache.
 */
void rtems_unfreeze_inst_cache( void )
{
	_CPU_unfreeze_inst_cache();
}


/* Turn on the instruction cache. */
void
rtems_enable_inst_cache( void )
{
	_CPU_enable_inst_cache();
}


/* Turn off the instruction cache. */
void
rtems_disable_inst_cache( void )
{
	_CPU_disable_inst_cache();
}
#endif
