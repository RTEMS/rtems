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

#if defined(HAS_INSTRUCTION_CACHE)
#define CPU_INSTRUCTION_CACHE_ALIGNMENT 0
#endif

#include <libcpu/cache.h>

#endif
/* end of include file */
