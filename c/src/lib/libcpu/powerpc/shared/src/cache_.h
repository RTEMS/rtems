/**
 * @file
 *
 * #ingroup powerpc_shared
 *
 * @brief Header file for the Cache Manager PowerPC support.
 */

#ifndef LIBCPU_POWERPC_CACHE_H
#define LIBCPU_POWERPC_CACHE_H

#include <rtems/powerpc/powerpc.h>

/* Provide the CPU defines only if we have a cache */
#if PPC_CACHE_ALIGNMENT != PPC_NO_CACHE_ALIGNMENT
  #define CPU_DATA_CACHE_ALIGNMENT PPC_CACHE_ALIGNMENT
  #define CPU_INSTRUCTION_CACHE_ALIGNMENT PPC_CACHE_ALIGNMENT
#endif

#ifdef _OLD_EXCEPTIONS
# warning OLD EXCEPTIONS
# include <libcpu/cache.h>
#endif

#endif /* LIBCPU_POWERPC_CACHE_H */
