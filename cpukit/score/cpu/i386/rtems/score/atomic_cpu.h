/**
 * @file  rtems/score/atomic_cpu.h
 * 
 * This include file implements the atomic operations for i386 and defines 
 * atomic date types which are used by the atomic operations API file. This
 * file should use fixed name atomic_cpu.h and should be included in atomic
 * operations API file atomic.h. Most of the parts of implementations are 
 * imported from FreeBSD kernel.
 */

/*
 * COPYRIGHT (c) 2012 Deng Hengyi.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *-
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS atomic implementation
 *
 */

/**@{*/

/**
 * @brief atomic operation unsigned integer type
 */
typedef unsigned int Atomic_Int;

/**
 * @brief atomic operation unsigned long integer type
 */
typedef unsigned long Atomic_Long;

/**
 * @brief atomic operation unsigned 32-bit integer type
 */
typedef uint32_t Atomic_Int32;

/**
 * @brief atomic operation unsigned 64-bit integer type
 */
typedef uint64_t Atomic_Int64;

/**
 * @brief atomic operation unsigned integer the size of a pointer type
 */
typedef uintptr_t Atomic_Pointer;

#if defined(RTEMS_SMP)
#define	MPLOCKED	"lock ; "
#else
#define	MPLOCKED
#endif

#if !defined(RTEMS_SMP)
/*
 * We assume that a = b will do atomic loads and stores.  However, on a
 * PentiumPro or higher, reads may pass writes, so for that case we have
 * to use a serializing instruction (i.e. with LOCK) to do the load in
 * SMP kernels.  For UP kernels, however, the cache of the single processor
 * is always consistent, so we only need to take care of compiler.
 */
#define	ATOMIC_STORE_LOAD(TYPE, LOP, SOP)               \
static inline Atomic_##TYPE                           \
_CPU_Atomic_Load_##TYPE(volatile Atomic_##TYPE *p)      \
{                                                       \
  Atomic_##TYPE tmp;                                    \
                                                        \
  tmp = *p;                                             \
  __asm __volatile("" : : : "memory");                  \
  return (tmp);                                         \
}                                                       \
                                                        \
static inline _CPU_Atomic_Load_acq_##TYPE(volatile Atomic_##TYPE *p)  \
{                                                       \
  Atomic_##TYPE tmp;                                    \
                                                        \
  tmp = *p;                                             \
  __asm __volatile("" : : : "memory");                  \
  return (tmp);                                         \
}                                                       \
                                                        \
static inline void                                    \
_CPU_Atomic_Store_##TYPE(volatile Atomic_##TYPE *p, Atomic_##TYPE v) \
{                                                                    \
  __asm __volatile("" : : : "memory");                               \
  *p = v;                                                            \
}                                                                    \
                                                        \
static inline void                                    \
_CPU_Atomic_Store_rel_##TYPE(volatile Atomic_##TYPE *p, Atomic_##TYPE v) \
{                                                                        \
  __asm __volatile("" : : : "memory");                                   \
  *p = v;                                                                \
}                                                                        \

#else /* !(!SMP) */

#define	ATOMIC_STORE_LOAD(TYPE, LOP, SOP)               \
static inline Atomic_##TYPE                           \
_CPU_Atomic_Load_##TYPE(volatile Atomic_##TYPE *p)      \
{                                                       \
  Atomic_##TYPE res;                                    \
                                                        \
  __asm __volatile(MPLOCKED LOP                         \
  : "=a" (res),                 /* 0 */                 \
  "=m" (*p)                     /* 1 */                 \
  : "m" (*p)                    /* 2 */                 \
  : "memory", "cc");                                    \
                                                        \
  return (res);                                         \
}                                                       \
                                                        \
static inline Atomic_##TYPE                           \
_CPU_Atomic_Load_acq_##TYPE(volatile Atomic_##TYPE *p)  \
{                                                       \
  Atomic_##TYPE res;                                    \
                                                        \
  __asm __volatile(MPLOCKED LOP                         \
  : "=a" (res),			/* 0 */                 \
  "=m" (*p)			/* 1 */                 \
  : "m" (*p)			/* 2 */                 \
  : "memory", "cc");                                    \
                                                        \
  return (res);                                         \
}							\
							\
/*							\
 * The XCHG instruction asserts LOCK automagically.	\
 */							\
static inline void                                    \
_CPU_Atomic_Store_##TYPE(volatile Atomic_##TYPE *p, Atomic_##TYPE v) \
{                                                                    \
  __asm __volatile(SOP                                               \
  : "=m" (*p),                  /* 0 */                              \
  "+r" (v)                      /* 1 */                              \
  : "m" (*p)                    /* 2 */                              \
  : "memory");                                                       \
}                                                                    \
static inline void					             \
_CPU_Atomic_Store_rel_##TYPE(volatile Atomic_##TYPE *p, Atomic_##TYPE v) \
{                                                                        \
  __asm __volatile(SOP                                                   \
  : "=m" (*p),			/* 0 */                                  \
  "+r" (v)			/* 1 */		                         \
  : "m" (*p)			/* 2 */	                                 \
  : "memory");                                                           \
}                                                                        \

#endif /* !SMP */

/*
 * The assembly is volatilized to avoid code chunk removal by the compiler.
 * GCC aggressively reorders operations and memory clobbering is necessary
 * in order to avoid that for memory barriers.
 */
#define	ATOMIC_FETCH_GENERIC(NAME, TYPE, OP, CONS, V)                         \
static inline void                                                          \
_CPU_Atomic_Fetch_##NAME##_##TYPE(volatile Atomic_##TYPE *p, Atomic_##TYPE v) \
{                                                                             \
  __asm __volatile(MPLOCKED OP                                                \
  : "=m" (*p)                                                                 \
  : CONS (V), "m" (*p)                                                        \
  : "cc");                                                                    \
}                                                                             \
                                                                              \
static inline void                                                          \
_CPU_Atomic_Fetch_##NAME##_barr_##TYPE(volatile Atomic_##TYPE *p, Atomic_##TYPE v)\
{                                                                             \
  __asm __volatile(MPLOCKED OP                                                \
  : "=m" (*p)                                                                 \
  : CONS (V), "m" (*p)                                                        \
  : "memory", "cc");                                                          \
}                                                                             \

/*
 * Atomic compare and set, used by the mutex functions
 *
 * if (*dst == expect) *dst = src (all 32 bit words)
 *
 * Returns 0 on failure, non-zero on success
 */
static inline int
_CPU_Atomic_Compare_exchange_int(volatile Atomic_Int *dst, Atomic_Int expect, Atomic_Int src)
{
  unsigned char res;

  __asm __volatile(
  "    " MPLOCKED "    "
  "    cmpxchgl %2,%1 ;    "
  "    sete	%0 ;       "
  "1:                      "
  "# atomic_cmpset_int"
  : "=a" (res),              /* 0 */
    "=m" (*dst)              /* 1 */
  : "r" (src),               /* 2 */
    "a" (expect),            /* 3 */
    "m" (*dst)               /* 4 */
  : "memory", "cc");

  return (res);
}

static inline int
_CPU_Atomic_Compare_exchange_long(volatile Atomic_Long *dst, Atomic_Long expect, Atomic_Long src)
{

  return (_CPU_Atomic_Compare_exchange_int((volatile Atomic_Int *)dst, (Atomic_Int)expect,
         (Atomic_Int)src));
}

ATOMIC_STORE_LOAD(Int,	"cmpxchgl %0,%1",  "xchgl %1,%0");
ATOMIC_STORE_LOAD(Long,	"cmpxchgl %0,%1",  "xchgl %1,%0");

ATOMIC_FETCH_GENERIC(add, Int, "addl %1,%0", "ir", v);
ATOMIC_FETCH_GENERIC(sub, Int, "subl %1,%0", "ir", v);
ATOMIC_FETCH_GENERIC(or,  Int, "orl %1,%0",  "ir", v);
ATOMIC_FETCH_GENERIC(and, Int, "andl %1,%0", "ir", v);

ATOMIC_FETCH_GENERIC(add, Long, "addl %1,%0", "ir", v);
ATOMIC_FETCH_GENERIC(sub, Long, "subl %1,%0", "ir", v);
ATOMIC_FETCH_GENERIC(or,  Long, "orl %1,%0",  "ir", v);
ATOMIC_FETCH_GENERIC(and, Long, "andl %1,%0", "ir", v);

#define	_CPU_Atomic_Fetch_or_acq_int		_CPU_Atomic_Fetch_or_barr_int
#define	_CPU_Atomic_Fetch_or_rel_int		_CPU_Atomic_Fetch_or_barr_int
#define	_CPU_Atomic_Fetch_and_acq_int		_CPU_Atomic_Fetch_and_barr_int
#define	_CPU_Atomic_Fetch_and_rel_int		_CPU_Atomic_Fetch_and_barr_int
#define	_CPU_Atomic_Fetch_add_acq_int		_CPU_Atomic_Fetch_add_barr_int
#define	_CPU_Atomic_Fetch_add_rel_int		_CPU_Atomic_Fetch_add_barr_int
#define	_CPU_Atomic_Fetch_sub_acq_int		_CPU_Atomic_Fetch_sub_barr_int
#define	_CPU_Atomic_Fetch_sub_rel_int		_CPU_Atomic_Fetch_sub_barr_int
#define	_CPU_Atomic_Compare_exchange_acq_int  _CPU_Atomic_Compare_exchange_int
#define	_CPU_Atomic_Compare_exchange_rel_int  _CPU_Atomic_Compare_exchange_int

#define	_CPU_Atomic_Fetch_or_acq_long		_CPU_Atomic_Fetch_or_barr_long
#define	_CPU_Atomic_Fetch_or_rel_long		_CPU_Atomic_Fetch_or_barr_long
#define	_CPU_Atomic_Fetch_and_acq_long		_CPU_Atomic_Fetch_and_barr_long
#define	_CPU_Atomic_Fetch_and_rel_long		_CPU_Atomic_Fetch_and_barr_long
#define	_CPU_Atomic_Fetch_add_acq_long		_CPU_Atomic_Fetch_add_barr_long
#define	_CPU_Atomic_Fetch_add_rel_long		_CPU_Atomic_Fetch_add_barr_long
#define	_CPU_Atomic_Fetch_sub_acq_long	        _CPU_Atomic_Fetch_sub_barr_long
#define	_CPU_Atomic_Fetch_sub_rel_long	        _CPU_Atomic_Fetch_sub_barr_long
#define	_CPU_Atomic_Compare_exchange_acq_long _CPU_Atomic_Compare_exchange_long
#define	_CPU_Atomic_Compare_exchange_rel_long _CPU_Atomic_Compare_exchange_long

/* Operations on 32-bit double words. */
#define	_CPU_Atomic_Fetch_or_32(p, v)  \
    _CPU_Atomic_Fetch_or_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_or_acq_32(p, v)  \
    _CPU_Atomic_Fetch_or_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_or_rel_32(p, v)  \
    _CPU_Atomic_Fetch_or_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_and_32(p, v)  \
    _CPU_Atomic_Fetch_and_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_and_acq_32(p, v)  \
    _CPU_Atomic_Fetch_and_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_and_rel_32(p, v)  \
    _CPU_Atomic_Fetch_and_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_add_32(p, v)  \
    _CPU_Atomic_Fetch_add_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_add_acq_32(p, v)  \
    _CPU_Atomic_Fetch_add_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_add_rel_32(p, v)  \
    _CPU_Atomic_Fetch_add_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_sub_32(p, v)  \
    _CPU_Atomic_Fetch_sub_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_sub_acq_32(p, v)  \
    _CPU_Atomic_Fetch_sub_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_sub_rel_32(p, v)  \
    _CPU_Atomic_Fetch_sub_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Load_32(p)  \
    _CPU_Atomic_Load_int((volatile Atomic_Int *)(p))
#define	_CPU_Atomic_Load_acq_32(p)  \
    _CPU_Atomic_Load_acq_int((volatile Atomic_Int *)(p))
#define _CPU_Atomic_Store_32(p, v)  \
    _CPU_Atomic_Store_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Store_rel_32(p, v)  \
    _CPU_Atomic_Store_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Compare_exchange_32(dst, old, new)  \
    _CPU_Atomic_Compare_exchange_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), (Atomic_Int)(new))
#define	_CPU_Atomic_Compare_exchange_acq_32(dst, old, new)  \
    _CPU_Atomic_Compare_exchange_acq_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), (Atomic_Int)(new))
#define	_CPU_Atomic_Compare_exchange_rel_32(dst, old, new)  \
    _CPU_Atomic_Compare_exchange_rel_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), (Atomic_Int)(new))

/* Operations on pointers. */
#define	_CPU_Atomic_Fetch_or_ptr(p, v) \
    _CPU_Atomic_Fetch_or_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_or_acq_ptr(p, v) \
    _CPU_Atomic_Fetch_or_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_or_rel_ptr(p, v) \
    _CPU_Atomic_Fetch_or_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_and_ptr(p, v) \
    _CPU_Atomic_Fetch_and_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_and_acq_ptr(p, v) \
    _CPU_Atomic_Fetch_and_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_and_rel_ptr(p, v) \
    _CPU_Atomic_Fetch_and_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_add_ptr(p, v) \
    _CPU_Atomic_Fetch_add_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_add_acq_ptr(p, v) \
    _CPU_Atomic_Fetch_add_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_add_rel_ptr(p, v) \
    _CPU_Atomic_Fetch_add_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_sub_ptr(p, v) \
    _CPU_Atomic_Fetch_sub_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_sub_acq_ptr(p, v) \
    _CPU_Atomic_Fetch_sub_acq_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define	_CPU_Atomic_Fetch_sub_rel_ptr(p, v) \
    _CPU_Atomic_Fetch_sub_rel_int((volatile Atomic_Int *)(p), (Atomic_Int)(v))
#define _CPU_Atomic_Load_ptr(p) \
    _CPU_Atomic_Load_int((volatile Atomic_Int *)(p))
#define	_CPU_Atomic_Load_acq_ptr(p) \
    _CPU_Atomic_Load_acq_int((volatile Atomic_Int *)(p))
#define _CPU_Atomic_Store_ptr(p, v) \
    _CPU_Atomic_Store_int((volatile Atomic_Int *)(p), (v))
#define	_CPU_Atomic_Store_rel_ptr(p, v) \
    _CPU_Atomic_Store_rel_int((volatile Atomic_Int *)(p), (v))
#define	_CPU_Atomic_Compare_exchange_ptr(dst, old, new) \
    _CPU_Atomic_Compare_exchange_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), (Atomic_Int)(new))
#define	_CPU_Atomic_Compare_exchange_acq_ptr(dst, old, new) \
    _CPU_Atomic_Compare_exchange_acq_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), \
            (Atomic_Int)(new))
#define	_CPU_Atomic_Compare_exchange_rel_ptr(dst, old, new) \
    _CPU_Atomic_Compare_exchange_rel_int((volatile Atomic_Int *)(dst), (Atomic_Int)(old), \
            (Atomic_Int)(new))

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
