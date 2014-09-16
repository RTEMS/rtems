/*
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_OR1K_SHARED_CACHE_H
#define LIBBSP_OR1K_SHARED_CACHE_H

#include <assert.h>
#include <bsp.h>
#include <rtems/rtems/intr.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* These two defines also ensure that the rtems_cache_* functions have bodies */
#define CPU_DATA_CACHE_ALIGNMENT        32
#define CPU_INSTRUCTION_CACHE_ALIGNMENT 32

#define CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS 1

static inline size_t
_CPU_cache_get_data_cache_size( const uint32_t level )
{
  return (level == 0 || level == 1)? 8192 : 0;
}

static inline size_t
_CPU_cache_get_instruction_cache_size( const uint32_t level )
{
  return (level == 0 || level == 1)? 8192 : 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_OR1K_SHARED_CACHE_H */
