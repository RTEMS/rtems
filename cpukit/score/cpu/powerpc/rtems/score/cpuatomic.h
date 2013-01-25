/**
 * @file  rtems/score/cpuatomic.h
 * 
 * This include file implements the atomic operations for PowerPC and defines 
 * atomic data types which are used by the atomic operations API file. This
 * file should use fixed name cpuatomic.h and should be included in atomic
 * operations API file atomic.h. Most of the parts of implementations are 
 * imported from FreeBSD kernel.
 */

/*
 * Copyright (c) 2008 Marcel Moolenaar
 * Copyright (c) 2001 Benno Rice
 * Copyright (c) 2001 David E. O'Brien
 * Copyright (c) 1998 Doug Rabson
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _RTEMS_SCORE_ATOMIC_CPU_H
#define _RTEMS_SCORE_ATOMIC_CPU_H

#include <rtems/score/genericcpuatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS atomic implementation
 *
 */

/**@{*/

#define	__ATOMIC_BARRIER                   \
    __asm __volatile("sync" : : : "memory")

#define mb()	__ATOMIC_BARRIER
#define	wmb()	mb()
#define	rmb()	mb()

/*
 * Atomic_Fetch_add(p, v)
 * { *p += v; }
 */
#define __CPU_Atomic_Fetch_add_int(p, v, t)         \
  __asm __volatile(                                 \
     "1:lwarx	%0, 0, %2\n"                        \
     "	add	%0, %3, %0\n"                       \
     "	stwcx.	%0, 0, %2\n"                        \
     "	bne-	1b\n"                               \
     : "=&r" (t), "=m" (*p)                         \
     : "r" (p), "r" (v), "m" (*p)                   \
     : "cc", "memory")                              \
     /* __CPU_Atomic_Fetch_add_int */

#define	__CPU_Atomic_Fetch_add_long(p, v, t)        \
  __asm __volatile(                                 \
     "1:lwarx	%0, 0, %2\n"                        \
     "	add	%0, %3, %0\n"                       \
     "	stwcx.	%0, 0, %2\n"                        \
     "	bne-	1b\n"                               \
     : "=&r" (t), "=m" (*p)                         \
     : "r" (p), "r" (v), "m" (*p)                   \
     : "cc", "memory")                              \
     /* __CPU_Atomic_Fetch_add_long */

#define	_ATOMIC_ADD(typename, type)                 \
  static __inline void                              \
  _CPU_Atomic_Fetch_add_##typename(volatile Atomic_##type *p, Atomic_##type v) {  \
    Atomic_##type t;                                \
    __CPU_Atomic_Fetch_add_##typename(p, v, t);     \
  }                                                 \
                                                    \
  static __inline void                              \
  _CPU_Atomic_Fetch_add_acq_##typename(volatile Atomic_##type *p, Atomic_##type v) { \
    Atomic_##type t;                                \
   __CPU_Atomic_Fetch_add_##typename(p, v, t);      \
   __ATOMIC_BARRIER;                                \
  }                                                 \
                                                    \
  static __inline void                              \
  _CPU_Atomic_Fetch_add_rel_##typename(volatile Atomic_##type *p, Atomic_##type v) { \
    Atomic_##type t;                                \
    __ATOMIC_BARRIER;                               \
    __CPU_Atomic_Fetch_add_##typename(p, v, t);     \
  }                                                 \
  /* _ATOMIC_ADD */

_ATOMIC_ADD(int, Int)
_ATOMIC_ADD(long, Long)

#define _CPU_Atomic_Fetch_add_32(p, v)      \
    _CPU_Atomic_Fetch_add_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_add_acq_32(p, v)  \
    _CPU_Atomic_Fetch_add_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_add_rel_32(p, v)  \
    _CPU_Atomic_Fetch_add_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))

#define _CPU_Atomic_Fetch_add_ptr(p, v)     \
    _CPU_Atomic_Fetch_add_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_add_acq_ptr(p, v) \
    _CPU_Atomic_Fetch_add_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_add_rel_ptr(p, v) \
    _CPU_Atomic_Fetch_add_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))

#undef _ATOMIC_ADD
#undef __CPU_Atomic_Fetch_add_long
#undef __CPU_Atomic_Fetch_add_int

/*
 * Atomic_Fetch_and(p, v)
 * { *p &= v; }
 */

#define __CPU_Atomic_Fetch_and_int(p, v, t)                     \
  __asm __volatile(                                             \
  "1:	lwarx	%0, 0, %2\n"                                    \
  "	and	%0, %0, %3\n"                                   \
  "	stwcx.	%0, 0, %2\n"                                    \
  "	bne-	1b\n"                                           \
  : "=&r" (t), "=m" (*p)                                        \
  : "r" (p), "r" (v), "m" (*p)                                  \
  : "cc", "memory")                                             \
  /* _CPU_Atomic_Fetch_and_int */

#define	__CPU_Atomic_Fetch_and_long(p, v, t)                    \
  __asm __volatile(                                             \
  "1:	lwarx	%0, 0, %2\n"                                    \
  "	and	%0, %0, %3\n"                                   \
  "	stwcx.	%0, 0, %2\n"                                    \
  "	bne-	1b\n"                                           \
  : "=&r" (t), "=m" (*p)                                        \
  : "r" (p), "r" (v), "m" (*p)                                  \
  : "cc", "memory")                                             \
  /* _CPU_Atomic_Fetch_and_long */

#define	_ATOMIC_AND(typename, type)                             \
  static __inline void                                          \
  _CPU_Atomic_Fetch_and_##typename(volatile Atomic_##type *p, Atomic_##type v) {  \
    Atomic_##type t;                                            \
    __CPU_Atomic_Fetch_and_##typename(p, v, t);                 \
  }                                                             \
                                                                \
  static __inline void                                          \
  _CPU_Atomic_Fetch_and_acq_##typename(volatile Atomic_##type *p, Atomic_##type v) {  \
    Atomic_##type t;                                            \
    __CPU_Atomic_Fetch_and_##typename(p, v, t);                 \
    __ATOMIC_BARRIER;                                           \
  }                                                             \
                                                                \
  static __inline void                                          \
  _CPU_Atomic_Fetch_and_rel_##typename(volatile Atomic_##type *p, Atomic_##type v) {  \
    Atomic_##type t;                                            \
    __ATOMIC_BARRIER;                                           \
    __CPU_Atomic_Fetch_and_##typename(p, v, t);                 \
  }                                                             \
  /* _ATOMIC_AND */


_ATOMIC_AND(int, Int)
_ATOMIC_AND(long, Long)

#define _CPU_Atomic_Fetch_and_32(p, v)      \
    _CPU_Atomic_Fetch_and_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_and_acq_32(p, v)  \
    _CPU_Atomic_Fetch_and_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_and_rel_32(p, v)  \
    _CPU_Atomic_Fetch_and_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))

#define _CPU_Atomic_Fetch_and_ptr(p, v)     \
    _CPU_Atomic_Fetch_and_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_and_acq_ptr(p, v) \
    _CPU_Atomic_Fetch_and_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_and_rel_ptr(p, v) \
    _CPU_Atomic_Fetch_and_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))


#undef _ATOMIC_AND
#undef __CPU_Atomic_Fetch_and_long
#undef __CPU_Atomic_Fetch_and_int

/*
 * Atomic_Fetch_or(p, v)
 * { *p |= v; }
 */

#define __CPU_Atomic_Fetch_or_int(p, v, t)              \
  __asm __volatile(                                     \
  "1:	lwarx	%0, 0, %2\n"                            \
  "	or	%0, %3, %0\n"                           \
  "	stwcx.	%0, 0, %2\n"                            \
  "	bne-	1b\n"                                   \
  : "=&r" (t), "=m" (*p)                                \
  : "r" (p), "r" (v), "m" (*p)                          \
  : "cc", "memory")                                     \
  /* __CPU_Atomic_Fetch_or_int */

#define	__CPU_Atomic_Fetch_or_long(p, v, t)             \
  __asm __volatile(                                     \
  "1:	lwarx	%0, 0, %2\n"                            \
  "	or	%0, %3, %0\n"                           \
  "	stwcx.	%0, 0, %2\n"                            \
  "	bne-	1b\n"                                   \
  : "=&r" (t), "=m" (*p)                                \
  : "r" (p), "r" (v), "m" (*p)                          \
  : "cc", "memory")                                     \
  /* __CPU_Atomic_Fetch_or_long */

#define	_ATOMIC_OR(typename, type)                      \
  static __inline void                                  \
  _CPU_Atomic_Fetch_or_##typename(volatile Atomic_##type *p, Atomic_##type v) {  \
    Atomic_##type t;                                    \
    __CPU_Atomic_Fetch_or_##typename(p, v, t);          \
  }                                                     \
                                                        \
  static __inline void                                  \
  _CPU_Atomic_Fetch_or_acq_##typename(volatile Atomic_##type *p, Atomic_##type v) { \
    Atomic_##type t;                                    \
    __CPU_Atomic_Fetch_or_##typename(p, v, t);          \
    __ATOMIC_BARRIER;                                   \
  }                                                     \
                                                        \
  static __inline void                                  \
  _CPU_Atomic_Fetch_or_rel_##typename(volatile Atomic_##type *p, Atomic_##type v) {	\
    Atomic_##type t;                                    \
    __ATOMIC_BARRIER;                                   \
    __CPU_Atomic_Fetch_or_##typename(p, v, t);          \
  }                                                     \
  /* _ATOMIC_OR */

_ATOMIC_OR(int, Int)
_ATOMIC_OR(long, Long)

#define _CPU_Atomic_Fetch_or_32(p, v)      \
    _CPU_Atomic_Fetch_or_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_or_acq_32(p, v)  \
    _CPU_Atomic_Fetch_or_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_or_rel_32(p, v)  \
    _CPU_Atomic_Fetch_or_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))

#define _CPU_Atomic_Fetch_or_ptr(p, v)     \
    _CPU_Atomic_Fetch_or_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_or_acq_ptr(p, v) \
    _CPU_Atomic_Fetch_or_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_or_rel_ptr(p, v) \
    _CPU_Atomic_Fetch_or_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))

#undef _ATOMIC_OR
#undef __CPU_Atomic_Fetch_or_long
#undef __CPU_Atomic_Fetch_or_int

/*
 * Atomic_Fetch_sub(p, v)
 * { *p -= v; }
 */

#define __CPU_Atomic_Fetch_sub_int(p, v, t)             \
  __asm __volatile(                                     \
  "1:	lwarx	%0, 0, %2\n"                            \
  "	subf	%0, %3, %0\n"                           \
  "	stwcx.	%0, 0, %2\n"                            \
  "	bne-	1b\n"                                   \
  : "=&r" (t), "=m" (*p)                                \
  : "r" (p), "r" (v), "m" (*p)                          \
  : "cc", "memory")                                     \
  /* __CPU_Atomic_Fetch_sub_int */

#define	__CPU_Atomic_Fetch_sub_long(p, v, t)            \
  __asm __volatile(                                     \
  "1:	lwarx	%0, 0, %2\n"                            \
  "	subf	%0, %3, %0\n"                           \
  "	stwcx.	%0, 0, %2\n"                            \
  "	bne-	1b\n"                                   \
  : "=&r" (t), "=m" (*p)                                \
  : "r" (p), "r" (v), "m" (*p)                          \
  : "cc", "memory")                                     \
  /* __CPU_Atomic_Fetch_sub_long */

#define	_ATOMIC_SUB(typename, type)                     \
  static __inline void                                  \
  _CPU_Atomic_Fetch_sub_##typename(volatile Atomic_##type *p, Atomic_##type v) {     \
    Atomic_##type t;                                    \
    __CPU_Atomic_Fetch_sub_##typename(p, v, t);         \
  }                                                     \
                                                        \
  static __inline void                                  \
  _CPU_Atomic_Fetch_sub_acq_##typename(volatile Atomic_##type *p, Atomic_##type v) { \
    Atomic_##type t;                                    \
    __CPU_Atomic_Fetch_sub_##typename(p, v, t);         \
    __ATOMIC_BARRIER;                                   \
  }                                                     \
                                                        \
  static __inline void                                  \
  _CPU_Atomic_Fetch_sub_rel_##typename(volatile Atomic_##type *p, Atomic_##type v) { \
    Atomic_##type t;                                    \
    __ATOMIC_BARRIER;                                   \
    __CPU_Atomic_Fetch_sub_##typename(p, v, t);         \
  }                                                     \
  /* _ATOMIC_SUB */


_ATOMIC_SUB(int, Int)
_ATOMIC_SUB(long, Long)

#define _CPU_Atomic_Fetch_sub_32(p, v)      \
    _CPU_Atomic_Fetch_sub_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_sub_acq_32(p, v)  \
    _CPU_Atomic_Fetch_sub_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_sub_rel_32(p, v)  \
    _CPU_Atomic_Fetch_sub_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))

#define _CPU_Atomic_Fetch_sub_ptr(p, v)     \
    _CPU_Atomic_Fetch_sub_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_sub_acq_ptr(p, v) \
    _CPU_Atomic_Fetch_sub_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Fetch_sub_rel_ptr(p, v) \
    _CPU_Atomic_Fetch_sub_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))

#undef _ATOMIC_SUB
#undef __CPU_Atomic_Fetch_sub_long
#undef __CPU_Atomic_Fetch_sub_int

/*
 * We assume that a = b will do atomic loads and stores.
 */
#define	ATOMIC_STORE_LOAD(TYPENAME, TYPE)                       \
static __inline Atomic_##TYPE                                   \
_CPU_Atomic_Load_##TYPENAME(volatile Atomic_##TYPE *p)          \
{                                                               \
  Atomic_##TYPE v;                                              \
                                                                \
  v = *p;                                                       \
  return (v);                                                   \
}                                                               \
static __inline Atomic_##TYPE                                   \
_CPU_Atomic_Load_acq_##TYPENAME(volatile Atomic_##TYPE *p)      \
{                                                               \
  Atomic_##TYPE v;                                              \
                                                                \
  v = *p;                                                       \
  __ATOMIC_BARRIER;                                             \
  return (v);                                                   \
}                                                               \
                                                                \
static __inline void                                            \
_CPU_Atomic_Store_##TYPENAME(volatile Atomic_##TYPE *p, Atomic_##TYPE v)      \
{                                                               \
  *p = v;                                                       \
}                                                               \
static __inline void                                            \
_CPU_Atomic_Store_rel_##TYPENAME(volatile Atomic_##TYPE *p, Atomic_##TYPE v)  \
{                                                               \
  __ATOMIC_BARRIER;                                             \
  *p = v;                                                       \
}

ATOMIC_STORE_LOAD(int, Int)

#define _CPU_Atomic_Load_32(p)      \
    _CPU_Atomic_Load_int((volatile Atomic_Int *)(p))
#define _CPU_Atomic_Load_acq_32(p)  \
    _CPU_Atomic_Load_acq_int((volatile Atomic_Int *)(p))
#define _CPU_Atomic_Store_32(p, v)  \
    _CPU_Atomic_Store_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Store_rel_32(p, v)  \
    _CPU_Atomic_Store_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))

static __inline Atomic_Long
_CPU_Atomic_Load_long(volatile Atomic_Long *addr)
{
  return ((Atomic_Long)_CPU_Atomic_Load_int((volatile Atomic_Int *)addr));
}

static __inline Atomic_Long
_CPU_Atomic_Load_acq_long(volatile Atomic_Long *addr)
{
  return ((Atomic_Long)_CPU_Atomic_Load_acq_int((volatile Atomic_Int *)addr));
}

static __inline void
_CPU_Atomic_Store_long(volatile Atomic_Long *addr, Atomic_Long val)
{
  _CPU_Atomic_Store_int((volatile Atomic_Int *)addr, (Atomic_Int)val);
}

static __inline void
_CPU_Atomic_Store_rel_long(volatile Atomic_Long *addr, Atomic_Long val)
{
  _CPU_Atomic_Store_rel_int((volatile Atomic_Int *)addr, (Atomic_Int)val);
}

#define _CPU_Atomic_Load_ptr(p)     \
    _CPU_Atomic_Load_int((volatile Atomic_Int *)(p))
#define _CPU_Atomic_Load_acq_ptr(p) \
    _CPU_Atomic_Load_acq_int((volatile Atomic_Int *)(p))
#define _CPU_Atomic_Store_ptr(p, v) \
    _CPU_Atomic_Store_int((volatile Atomic_Int *)(p), (v))
#define _CPU_Atomic_Store_rel_ptr(p, v) \
    _CPU_Atomic_Store_rel_int((volatile Atomic_Int *)(p), (v))


#undef ATOMIC_STORE_LOAD

/*
 *  * Atomically compare the value stored at *p with cmpval and if the
 *   * two values are equal, update the value of *p with newval. Returns
 *    * zero if the compare failed, nonzero otherwise.
 *     */
static __inline int
_CPU_Atomic_Compare_exchange_int(volatile Atomic_Int* p, Atomic_Int cmpval, Atomic_Int newval)
{
  int ret;

  __asm __volatile (
    "1:\tlwarx %0, 0, %2\n\t"    /* load old value */
    "cmplw %3, %0\n\t"           /* compare */
    "bne 2f\n\t"                 /* exit if not equal */
    "stwcx. %4, 0, %2\n\t"       /* attempt to store */
    "bne- 1b\n\t"                /* spin if failed */
    "li %0, 1\n\t"               /* success - retval = 1 */
    "b 3f\n\t"                   /* we've succeeded */
    "2:\n\t"
    "stwcx. %0, 0, %2\n\t"       /* clear reservation (74xx) */
    "li %0, 0\n\t"               /* failure - retval = 0 */
    "3:\n\t"
    : "=&r" (ret), "=m" (*p)
    : "r" (p), "r" (cmpval), "r" (newval), "m" (*p)
    : "cc", "memory");

  return (ret);
}
static __inline int
_CPU_Atomic_Compare_exchange_long(volatile Atomic_Long* p, Atomic_Long cmpval, Atomic_Long newval)
{
  int ret;

  __asm __volatile (
    "1:\tlwarx %0, 0, %2\n\t"    /* load old value */
    "cmplw %3, %0\n\t"           /* compare */
    "bne 2f\n\t"                 /* exit if not equal */
    "stwcx. %4, 0, %2\n\t"       /* attempt to store */
    "bne- 1b\n\t"                /* spin if failed */
    "li %0, 1\n\t"               /* success - retval = 1 */
    "b 3f\n\t"                   /* we've succeeded */
    "2:\n\t"
    "stwcx. %0, 0, %2\n\t"       /* clear reservation (74xx) */
    "li %0, 0\n\t"               /* failure - retval = 0 */
    "3:\n\t"
    : "=&r" (ret), "=m" (*p)
    : "r" (p), "r" (cmpval), "r" (newval), "m" (*p)
    : "cc", "memory");

  return (ret);
}

static __inline int
_CPU_Atomic_Compare_exchange_acq_int(volatile Atomic_Int *p, Atomic_Int cmpval, Atomic_Int newval)
{
  int retval;

  retval = _CPU_Atomic_Compare_exchange_int(p, cmpval, newval);
  __ATOMIC_BARRIER;
  return (retval);
}

static __inline int
_CPU_Atomic_Compare_exchange_rel_int(volatile Atomic_Int *p, Atomic_Int cmpval, Atomic_Int newval)
{
  __ATOMIC_BARRIER;
  return (_CPU_Atomic_Compare_exchange_int(p, cmpval, newval));
}

static __inline int
_CPU_Atomic_Compare_exchange_acq_long(volatile Atomic_Long *p, Atomic_Long cmpval, Atomic_Long newval)
{
  Atomic_Long retval;

  retval = _CPU_Atomic_Compare_exchange_long(p, cmpval, newval);
  __ATOMIC_BARRIER;
  return (retval);
}

static __inline int
_CPU_Atomic_Compare_exchange_rel_long(volatile Atomic_Long *p, Atomic_Long cmpval, Atomic_Long newval)
{
  __ATOMIC_BARRIER;
  return (_CPU_Atomic_Compare_exchange_long(p, cmpval, newval));
}

#define _CPU_Atomic_Compare_exchange_32(dst, old, new)      \
    _CPU_Atomic_Compare_exchange_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), (Atomic_Int)(new))
#define _CPU_Atomic_Compare_exchange_acq_32(dst, old, new)  \
    _CPU_Atomic_Compare_exchange_acq_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), (Atomic_Int)(new))
#define _CPU_Atomic_Compare_exchange_rel_32(dst, old, new)  \
    _CPU_Atomic_Compare_exchange_rel_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), (Atomic_Int)(new))

#define _CPU_Atomic_Compare_exchange_ptr(dst, old, new)     \
		_CPU_Atomic_Compare_exchange_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), (Atomic_Int)(new))
#define _CPU_Atomic_Compare_exchange_acq_ptr(dst, old, new) \
		_CPU_Atomic_Compare_exchange_acq_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), \
				(Atomic_Int)(new))
#define _CPU_Atomic_Compare_exchange_rel_ptr(dst, old, new) \
		_CPU_Atomic_Compare_exchange_rel_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), \
				(Atomic_Int)(new))


#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
