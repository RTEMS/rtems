/*
 *  COPYRIGHT (c) 2004.
 *  Ralf Corsepius, Ulm/Germany.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __sys_stdint_h
#define __sys_stdint_h

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3 ) \
  && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ > 2 ) 
#define __EXP(x) __##x##__
#else
#define __EXP(x) x
#include <limits.h>
#endif

#if __EXP(SCHAR_MAX) == 0x7f
typedef signed char int8_t ;
typedef unsigned char uint8_t ;
#define int8_t_defined 1

typedef signed char int_least8_t;
typedef unsigned char uint_least8_t;
#define int_least8_t_defined 1
#endif

#if __EXP(INT_MAX) == 0x7fff
typedef signed int int16_t;
typedef unsigned int uint16_t;
#define int16_t_defined 1
#elif __EXP(SHRT_MAX) == 0x7fff
typedef signed short int16_t;
typedef unsigned short uint16_t;
#define int16_t_defined 1
#elif __EXP(SCHAR_MAX) == 0x7fff
typedef signed char int32_t;
typedef unsigned char uint32_t;
#define int16_t_defined 1
#endif

#if __EXP(INT_MAX) == 0x7fffffffL
typedef signed int int32_t;
typedef unsigned int uint32_t;
#define int32_t_defined 1
#elif __EXP(LONG_MAX) == 0x7fffffffL
typedef signed long int32_t;
typedef unsigned long uint32_t;
#define int32_t_defined 1
#define __rtems_long32 1
#elif __EXP(SHRT_MAX) == 0x7fffffffL
typedef signed short int32_t;
typedef unsigned short uint32_t;
#define int32_t_defined 1
#elif __EXP(SCHAR_MAX) == 0x7fffffffL
typedef signed char int32_t;
typedef unsigned char uint32_t;
#define int32_t_defined 1
#endif

#if __EXP(LONG_MAX) > 0x7fffffff
typedef signed long int64_t;
typedef unsigned long uint64_t;
#define int64_t_defined 1
#define __rtems_long64 1
#elif  __EXP(LONG_LONG_MAX) > 0x7fffffff
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
#define int64_t_defined 1
#define __rtems_longlong64 1
#elif  __EXP(INT_MAX) > 0x7fffffff
typedef signed int int64_t;
typedef unsigned int uint64_t;
#define int64_t_defined 1
#endif

#if __rtems_longlong64
typedef signed long long intmax_t;
typedef unsigned long long uintmax_t;
#else
typedef signed long intmax_t;
typedef unsigned long uintmax_t;
#endif

typedef signed long intptr_t;
typedef unsigned long uintptr_t;

#undef __EXP

#ifdef __cplusplus
}
#endif

#endif /*__sys_stdint_h */
