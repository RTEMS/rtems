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

#ifndef __rtems__stdint_h
#define __rtems__stdint_h

#ifdef __GNUC__
#define __EXP(x) __##x##__
#else
#define __EXP(x) x
#include <limits.h>
#endif

#if __EXP(SCHAR_MAX) == 0x7f
typedef signed char __rtems_int8_t ;
typedef unsigned char __rtems_uint8_t ;
#endif

#if __EXP(INT_MAX) == 0x7fff
typedef signed int __rtems_int16_t;
typedef unsigned int __rtems_uint16_t;
#elif __EXP(SHRT_MAX) == 0x7fff
typedef signed short __rtems_int16_t;
typedef unsigned short __rtems_uint16_t;
#elif __EXP(SCHAR_MAX) == 0x7fff
typedef signed char __rtems_int32_t;
typedef unsigned char __rtems_uint32_t;
#endif

#if __EXP(INT_MAX) == 0x7fffffff
typedef signed int __rtems_int32_t;
typedef unsigned int __rtems_uint32_t;
#elif __EXP(LONG_MAX) == 0x7fffffff
typedef signed long __rtems_int32_t;
typedef unsigned long __rtems_uint32_t;
#define __rtems_long32 1
#elif __EXP(SHRT_MAX) == 0x7fffffff
typedef signed short __rtems_int32_t;
typedef unsigned short __rtems_uint32_t;
#elif __EXP(SCHAR_MAX) == 0x7fffffff
typedef signed char __rtems_int32_t;
typedef unsigned char __rtems_uint32_t;
#endif

#if __EXP(LONG_MAX) == 0x7fffffffffffffff
typedef signed long int __rtems_int64_t;
typedef unsigned long int __rtems_uint64_t;
#define __rtems_long64 1
#elif  __EXP(LONG_LONG_MAX) == 0x7fffffffffffffff
typedef signed long int __rtems_int64_t;
typedef unsigned long int __rtems_uint64_t;
#define __rtems_longlong64 1
#elif  __EXP(INT_MAX) == 0x7fffffffffffffff
typedef signed int __rtems_int64_t;
typedef unsigned int __rtems_uint64_t;
#endif

#undef __EXP

typedef signed long     intptr_t;
typedef unsigned long   uintptr_t;

#endif /* __rtems__stdint_h */
