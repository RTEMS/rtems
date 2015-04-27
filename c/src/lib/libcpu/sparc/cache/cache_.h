/*
 *  SPARC Cache Manager Support
 */

#ifndef __SPARC_CACHE_h
#define __SPARC_CACHE_h

/*
 * CACHE MANAGER: The following functions are CPU-specific.
 * They provide the basic implementation for the rtems_* cache
 * management routines. If a given function has no meaning for the CPU,
 * it does nothing by default.
 *
 * FIXME: Some functions simply have not been implemented.
 */

/* This define is set in a Makefile */
#if defined(HAS_INSTRUCTION_CACHE)

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 0

static inline void _CPU_cache_invalidate_entire_instruction ( void )
{
  __asm__ volatile ("flush");
}

/* XXX these need to be addressed */

static inline void _CPU_cache_invalidate_1_instruction_line (
  const void * i_addr )
{
}

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

#endif /* defined(HAS_INSTRUCTION_CACHE) */

#endif
/* end of include file */
