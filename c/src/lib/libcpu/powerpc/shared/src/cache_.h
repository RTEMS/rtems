/*
 *  PowerPC Cache Manager Support
 */

#ifndef __POWERPC_CACHE_h
#define __POWERPC_CACHE_h

/*
 * CACHE MANAGER: The following functions are CPU-specific.
 * They provide the basic implementation for the rtems_* cache
 * management routines. If a given function has no meaning for the CPU,
 * it does nothing by default.
 *
 * FIXME: Some functions simply have not been implemented.
 */

#if defined(ppc603)                     /* And possibly others */
#define CPU_DATA_CACHE_ALIGNMENT PPC_CACHE_ALIGNMENT
#define CPU_INSTRUCTION_CACHE_ALIGNMENT PPC_CACHE_ALIGNMENT

#elif ( defined(mpc860) || defined(mpc821) )

#define CPU_DATA_CACHE_ALIGNMENT PPC_CACHE_ALIGNMENT
#define CPU_INSTRUCTION_CACHE_ALIGNMENT PPC_CACHE_ALIGNMENT

#endif

#include <libcpu/cache.h>

#endif
/* end of include file */
