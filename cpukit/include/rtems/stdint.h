/* 
 * rtems/stdint.h
 *
 * ISO C99 integer types
 *
 * $Id$
 */

#ifndef __rtems_stdint_h
#define __rtems_stdint_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/cpuopts.h>

#if RTEMS_USES_STDINT_H
#include <stdint.h>

#elif RTEMS_USES_INTTYPES_H
#include <inttypes.h>

#else
/*
 * Wild guesses on systems not providing stdint.h nor inttypes.h
 * Known to work on Cywgin-1.3
 */
 
#include <sys/types.h>

#if !defined(uint8_t) && defined(__uint8_t)
typedef __uint8_t uint8_t;
#endif

#if !defined(uint16_t) && defined(__uint16_t)
typedef __uint16_t uint16_t;
#endif

#if !defined(uint32_t) && defined(__uint32_t)
typedef __uint32_t uint32_t;
#endif

#if !defined(uint64_t) && defined(__uint64_t)
typedef __uint64_t uint64_t;
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
