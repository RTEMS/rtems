/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreAtomic
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreAtomic.
 */

/*
 * COPYRIGHT (c) 2012-2013 Deng Hengyi.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_ATOMIC_H
#define _RTEMS_SCORE_ATOMIC_H

#include <rtems/score/cpuatomic.h>

/**
 * @defgroup RTEMSScoreAtomic Atomic Operations
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the atomic operations implementation.
 *
 * Atomic operations can be used to implement low-level synchronization
 * primitives on SMP systems, like spin locks.  All atomic operations are
 * defined in terms of C11 (ISO/IEC 9899:2011) or C++11 (ISO/IEC 14882:2011).
 * For documentation use the standard documents.
 *
 * @{
 */

typedef CPU_atomic_Uint Atomic_Uint;

typedef CPU_atomic_Ulong Atomic_Ulong;

typedef CPU_atomic_Uintptr Atomic_Uintptr;

typedef CPU_atomic_Flag Atomic_Flag;

typedef CPU_atomic_Order Atomic_Order;

#define ATOMIC_ORDER_RELAXED CPU_ATOMIC_ORDER_RELAXED

#define ATOMIC_ORDER_ACQUIRE CPU_ATOMIC_ORDER_ACQUIRE

#define ATOMIC_ORDER_RELEASE CPU_ATOMIC_ORDER_RELEASE

#define ATOMIC_ORDER_ACQ_REL CPU_ATOMIC_ORDER_ACQ_REL

#define ATOMIC_ORDER_SEQ_CST CPU_ATOMIC_ORDER_SEQ_CST

#define ATOMIC_INITIALIZER_UINT( value ) CPU_ATOMIC_INITIALIZER_UINT( value )

#define ATOMIC_INITIALIZER_ULONG( value ) CPU_ATOMIC_INITIALIZER_ULONG( value )

#define ATOMIC_INITIALIZER_UINTPTR( value ) CPU_ATOMIC_INITIALIZER_UINTPTR( value )

#define ATOMIC_INITIALIZER_FLAG CPU_ATOMIC_INITIALIZER_FLAG

#define _Atomic_Fence( order ) _CPU_atomic_Fence( order )

#define _Atomic_Init_uint( obj, desired ) \
  _CPU_atomic_Init_uint( obj, desired )

#define _Atomic_Init_ulong( obj, desired ) \
  _CPU_atomic_Init_ulong( obj, desired )

#define _Atomic_Init_uintptr( obj, desired ) \
  _CPU_atomic_Init_uintptr( obj, desired )

#define _Atomic_Load_uint( obj, order ) \
  _CPU_atomic_Load_uint( obj, order )

#define _Atomic_Load_ulong( obj, order ) \
  _CPU_atomic_Load_ulong( obj, order )

#define _Atomic_Load_uintptr( obj, order ) \
  _CPU_atomic_Load_uintptr( obj, order )

#define _Atomic_Store_uint( obj, desr, order ) \
  _CPU_atomic_Store_uint( obj, desr, order )

#define _Atomic_Store_ulong( obj, desr, order ) \
  _CPU_atomic_Store_ulong( obj, desr, order )

#define _Atomic_Store_uintptr( obj, desr, order ) \
  _CPU_atomic_Store_uintptr( obj, desr, order )

#define _Atomic_Fetch_add_uint( obj, arg, order ) \
  _CPU_atomic_Fetch_add_uint( obj, arg, order )

#define _Atomic_Fetch_add_ulong( obj, arg, order ) \
  _CPU_atomic_Fetch_add_ulong( obj, arg, order )

#define _Atomic_Fetch_add_uintptr( obj, arg, order ) \
  _CPU_atomic_Fetch_add_uintptr( obj, arg, order )

#define _Atomic_Fetch_sub_uint( obj, arg, order ) \
  _CPU_atomic_Fetch_sub_uint( obj, arg, order )

#define _Atomic_Fetch_sub_ulong( obj, arg, order ) \
  _CPU_atomic_Fetch_sub_ulong( obj, arg, order )

#define _Atomic_Fetch_sub_uintptr( obj, arg, order ) \
  _CPU_atomic_Fetch_sub_uintptr( obj, arg, order )

#define _Atomic_Fetch_or_uint( obj, arg, order ) \
  _CPU_atomic_Fetch_or_uint( obj, arg, order )

#define _Atomic_Fetch_or_ulong( obj, arg, order ) \
  _CPU_atomic_Fetch_or_ulong( obj, arg, order )

#define _Atomic_Fetch_or_uintptr( obj, arg, order ) \
  _CPU_atomic_Fetch_or_uintptr( obj, arg, order )

#define _Atomic_Fetch_and_uint( obj, arg, order ) \
  _CPU_atomic_Fetch_and_uint( obj, arg, order )

#define _Atomic_Fetch_and_ulong( obj, arg, order ) \
  _CPU_atomic_Fetch_and_ulong( obj, arg, order )

#define _Atomic_Fetch_and_uintptr( obj, arg, order ) \
  _CPU_atomic_Fetch_and_uintptr( obj, arg, order )

#define _Atomic_Exchange_uint( obj, desr, order ) \
  _CPU_atomic_Exchange_uint( obj, desr, order )

#define _Atomic_Exchange_ulong( obj, desr, order ) \
  _CPU_atomic_Exchange_ulong( obj, desr, order )

#define _Atomic_Exchange_uintptr( obj, desr, order ) \
  _CPU_atomic_Exchange_uintptr( obj, desr, order )

#define _Atomic_Compare_exchange_uint( obj, expected, desired, succ, fail ) \
  _CPU_atomic_Compare_exchange_uint( obj, expected, desired, succ, fail )

#define _Atomic_Compare_exchange_ulong( obj, expected, desired, succ, fail ) \
  _CPU_atomic_Compare_exchange_ulong( obj, expected, desired, succ, fail )

#define _Atomic_Compare_exchange_uintptr( obj, expected, desired, succ, fail ) \
  _CPU_atomic_Compare_exchange_uintptr( obj, expected, desired, succ, fail )

#define _Atomic_Flag_clear( obj, order ) \
  _CPU_atomic_Flag_clear( obj, order )

#define _Atomic_Flag_test_and_set( obj, order ) \
  _CPU_atomic_Flag_test_and_set( obj, order )

/** @} */

#endif /* _RTEMS_SCORE_ATOMIC_H */
