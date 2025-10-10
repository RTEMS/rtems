/**
 * @file
 *
 * @ingroup RTEMSImplClassicCache
 *
 * @brief This header file contains the implementation of the
 *   @ref RTEMSAPIClassicCache.
 */

/*
 * Copyright (C) 2014, 2018 embedded brains GmbH & Co. KG
 * Copyright (C) 1989, 1999 On-Line Applications Research Corporation (OAR)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *
 * The functions in this file implement the API to the
 * @ref RTEMSAPIClassicCache.  This file is intended to be included in a cache
 * implemention source file provided by the architecture or BSP, e.g.
 *
 *  - bsps/${RTEMS_CPU}/shared/cache/cache.c
 *  - bsps/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}/start/cache.c
 *
 * In this file a couple of defines and inline functions may be provided and
 * afterwards this file is included, e.g.
 *
 *  @code
 *  #define CPU_DATA_CACHE_ALIGNMENT XYZ
 *  ...
 *  #include "../../../bsps/shared/cache/cacheimpl.h"
 *  @endcode
 *
 * The cache implementation source file shall define
 *
 *  @code
 *  #define CPU_DATA_CACHE_ALIGNMENT <POSITIVE INTEGER>
 *  @endcode
 *
 * to enable the data cache support.
 *
 * The cache implementation source file shall define
 *
 *  @code
 *  #define CPU_INSTRUCTION_CACHE_ALIGNMENT <POSITIVE INTEGER>
 *  @endcode
 *
 * to enable the instruction cache support.
 *
 * The cache implementation source file shall define
 *
 *  @code
 *  #define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS
 *  @endcode
 *
 * if it provides cache maintenance functions which operate on multiple lines.
 * Otherwise a generic loop with single line operations will be used.  It is
 * strongly recommended to provide the implementation in terms of static inline
 * functions for performance reasons.
 *
 * The cache implementation source file shall define
 *
 *  @code
 *  #define CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS
 *  @endcode
 *
 * if it provides functions to get the data and instruction cache sizes by
 * level.
 *
 * The cache implementation source file shall define
 *
 *  @code
 *  #define CPU_CACHE_SUPPORT_PROVIDES_INSTRUCTION_SYNC_FUNCTION
 *  @endcode
 *
 * if special instructions must be used to synchronize the instruction caches
 * after a code change.
 *
 * The cache implementation source file shall define
 *
 *  @code
 *  #define CPU_CACHE_SUPPORT_PROVIDES_DISABLE_DATA
 *  @endcode
 *
 * if an external implementation of rtems_cache_disable_data() is provided,
 * e.g. as an implementation in assembly code.
 *
 * The cache implementation source file shall define
 *
 *  @code
 *  #define CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING
 *  @endcode
 *
 * if the hardware provides no instruction cache snooping and the instruction
 * cache invalidation needs software support.
 *
 * The functions below are implemented with inline routines found in the cache
 * implementation source file for each architecture or BSP.  In the event that
 * not support for a specific function for a cache is provided, the API routine
 * does nothing (but does exist).
 */

#include <rtems.h>

#include <sys/param.h>

#if defined(RTEMS_SMP) && defined(CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING)
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>
#endif

#if CPU_DATA_CACHE_ALIGNMENT > CPU_CACHE_LINE_BYTES
#error "CPU_DATA_CACHE_ALIGNMENT is greater than CPU_CACHE_LINE_BYTES"
#endif

#if CPU_INSTRUCTION_CACHE_ALIGNMENT > CPU_CACHE_LINE_BYTES
#error "CPU_INSTRUCTION_CACHE_ALIGNMENT is greater than CPU_CACHE_LINE_BYTES"
#endif

/**
 * @defgroup RTEMSImplClassicCache Cache Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Cache Manager implementation.
 */

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
#if defined(CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS)
  _CPU_cache_flush_data_range( d_addr, n_bytes );
#else
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
#else
  (void) d_addr;
  (void) n_bytes;
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
#if defined(CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS)
  _CPU_cache_invalidate_data_range( d_addr, n_bytes );
#else
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
#else
  (void) d_addr;
  (void) n_bytes;
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
size_t
rtems_cache_get_data_line_size( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  return CPU_DATA_CACHE_ALIGNMENT;
#else
  return 0;
#endif
}

size_t
rtems_cache_get_data_cache_size( uint32_t level )
{
#if defined(CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS)
  return _CPU_cache_get_data_cache_size( level );
#else
  (void) level;

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

void rtems_cache_unfreeze_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_unfreeze_data();
#endif
}

void
rtems_cache_enable_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_enable_data();
#endif
}

#if !defined(CPU_CACHE_SUPPORT_PROVIDES_DISABLE_DATA)
void
rtems_cache_disable_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_disable_data();
#endif
}
#endif

/*
 * THESE FUNCTIONS ONLY HAVE BODIES IF WE HAVE AN INSTRUCTION CACHE
 */

#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT) \
  && defined(RTEMS_SMP) \
  && defined(CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING)

typedef struct {
  const void *addr;
  size_t size;
} smp_cache_area;

static void smp_cache_inst_inv(void *arg)
{
  smp_cache_area *area = arg;

  _CPU_cache_invalidate_instruction_range(area->addr, area->size);
}

static void smp_cache_inst_inv_all(void *arg)
{
  (void) arg;
  _CPU_cache_invalidate_entire_instruction();
}

static void smp_cache_broadcast( SMP_Action_handler handler, void *arg )
{
  uint32_t         isr_level;
  Per_CPU_Control *cpu_self;

  isr_level = _ISR_Get_level();

  if ( isr_level == 0 ) {
    cpu_self = _Thread_Dispatch_disable();
  } else {
    cpu_self = _Per_CPU_Get();
  }

  ( *handler )( arg );
  _SMP_Othercast_action( handler, arg );

  if ( isr_level == 0 ) {
    _Thread_Dispatch_enable( cpu_self );
  }
}

#endif

/*
 * This function is responsible for performing an instruction cache
 * invalidate. It must determine how many cache lines need to be invalidated
 * and then perform the invalidations.
 */
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT) \
  && !defined(CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS)
static void
_CPU_cache_invalidate_instruction_range(
  const void * i_addr,
  size_t n_bytes
)
{
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
  while( final_address >= i_addr ) {
    _CPU_cache_invalidate_1_instruction_line( i_addr );
    i_addr = (void *)((size_t)i_addr + CPU_INSTRUCTION_CACHE_ALIGNMENT);
  }
}
#endif

void
rtems_cache_invalidate_multiple_instruction_lines(
  const void * i_addr,
  size_t n_bytes
)
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
#if defined(RTEMS_SMP) && defined(CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING)
  smp_cache_area area = { i_addr, n_bytes };

  smp_cache_broadcast( smp_cache_inst_inv, &area );
#else
  _CPU_cache_invalidate_instruction_range( i_addr, n_bytes );
#endif
#else
  (void) i_addr;
  (void) n_bytes;
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
#if defined(RTEMS_SMP) && defined(CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING)
  smp_cache_broadcast( smp_cache_inst_inv_all, NULL );
#else
 _CPU_cache_invalidate_entire_instruction();
#endif
#endif
}

/*
 * This function returns the instruction cache granularity.
 */
size_t
rtems_cache_get_instruction_line_size( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  return CPU_INSTRUCTION_CACHE_ALIGNMENT;
#else
  return 0;
#endif
}

size_t
rtems_cache_get_instruction_cache_size( uint32_t level )
{
#if defined(CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS)
  return _CPU_cache_get_instruction_cache_size( level );
#else
  (void) level;

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

void rtems_cache_unfreeze_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_unfreeze_instruction();
#endif
}

void
rtems_cache_enable_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_enable_instruction();
#endif
}

void
rtems_cache_disable_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_disable_instruction();
#endif
}

/* Returns the maximal cache line size of all cache kinds in bytes. */
size_t rtems_cache_get_maximal_line_size( void )
{
#if defined(CPU_MAXIMAL_CACHE_ALIGNMENT)
  return CPU_MAXIMAL_CACHE_ALIGNMENT;
#endif
  size_t data_line_size =
#if defined(CPU_DATA_CACHE_ALIGNMENT)
    CPU_DATA_CACHE_ALIGNMENT;
#else
    0;
#endif
  size_t instruction_line_size =
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
    CPU_INSTRUCTION_CACHE_ALIGNMENT;
#else
    0;
#endif
  return MAX( data_line_size, instruction_line_size );
}

/*
 * Purpose is to synchronize caches after code has been loaded
 * or self modified. Actual implementation is simple only
 * but it can and should be repaced by optimized version
 * which does not need flush and invalidate all cache levels
 * when code is changed.
 */
void rtems_cache_instruction_sync_after_code_change(
  const void *code_addr,
  size_t      n_bytes
)
{
#if defined(CPU_CACHE_SUPPORT_PROVIDES_INSTRUCTION_SYNC_FUNCTION)
  _CPU_cache_instruction_sync_after_code_change( code_addr, n_bytes );
#else
  rtems_cache_flush_multiple_data_lines( code_addr, n_bytes );
  rtems_cache_invalidate_multiple_instruction_lines( code_addr, n_bytes );
#endif
}
