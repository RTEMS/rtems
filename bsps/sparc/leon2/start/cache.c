/*
 *  SPARC Cache Manager Support
 */

/*
 * CACHE MANAGER: The following functions are CPU-specific.
 * They provide the basic implementation for the rtems_* cache
 * management routines. If a given function has no meaning for the CPU,
 * it does nothing by default.
 *
 * FIXME: Some functions simply have not been implemented.
 */

#include <stddef.h>

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 0

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

static inline void _CPU_cache_invalidate_entire_instruction ( void )
{
  __asm__ volatile ("flush");
}

static inline void _CPU_cache_invalidate_instruction_range(
  const void *i_addr,
  size_t      n_bytes
)
{
  __asm__ volatile ("flush");
}

/* XXX these need to be addressed */

static inline void _CPU_cache_freeze_instruction ( void )
{
}

static inline void _CPU_cache_unfreeze_instruction ( void )
{
}

static inline void _CPU_cache_enable_instruction ( void )
{
}

static inline void _CPU_cache_disable_instruction (   void )
{
}

#include "../../../shared/cache/cacheimpl.h"
