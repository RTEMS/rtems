/**
 * @file
 *
 * @ingroup ScoreAtomic
 *
 * @brief Atomic Operations API
 */

/*
 * COPYRIGHT (c) 2012-2013 Deng Hengyi.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ATOMIC_H
#define _RTEMS_SCORE_ATOMIC_H

#include <rtems/score/cpuatomic.h>

/**
 * @defgroup ScoreAtomic Atomic Operations
 *
 * @ingroup Score
 *
 * @brief Support for atomic operations.
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

typedef CPU_atomic_Pointer Atomic_Pointer;

typedef CPU_atomic_Flag Atomic_Flag;

typedef CPU_atomic_Order Atomic_Order;

#define ATOMIC_ORDER_RELAXED CPU_ATOMIC_ORDER_RELAXED

#define ATOMIC_ORDER_ACQUIRE CPU_ATOMIC_ORDER_ACQUIRE

#define ATOMIC_ORDER_RELEASE CPU_ATOMIC_ORDER_RELEASE

#define ATOMIC_ORDER_ACQ_REL CPU_ATOMIC_ORDER_ACQ_REL

#define ATOMIC_ORDER_SEQ_CST CPU_ATOMIC_ORDER_SEQ_CST

#define ATOMIC_INITIALIZER_UINT( value ) CPU_ATOMIC_INITIALIZER_UINT( value )

#define ATOMIC_INITIALIZER_ULONG( value ) CPU_ATOMIC_INITIALIZER_ULONG( value )

#define ATOMIC_INITIALIZER_PTR( value ) CPU_ATOMIC_INITIALIZER_PTR( value )

#define ATOMIC_INITIALIZER_FLAG CPU_ATOMIC_INITIALIZER_FLAG

#define _Atomic_Fence( order ) _CPU_atomic_Fence( order )

#define _Atomic_Init_uint( obj, desired ) \
  _CPU_atomic_Init_uint( obj, desired )

#define _Atomic_Init_ulong( obj, desired ) \
  _CPU_atomic_Init_ulong( obj, desired )

#define _Atomic_Init_ptr( obj, desired ) \
  _CPU_atomic_Init_ptr( obj, desired )

#define _Atomic_Load_uint( obj, order ) \
  _CPU_atomic_Load_uint( obj, order )

#define _Atomic_Load_ulong( obj, order ) \
  _CPU_atomic_Load_ulong( obj, order )

#define _Atomic_Load_ptr( obj, order ) \
  _CPU_atomic_Load_ptr( obj, order )

#define _Atomic_Store_uint( obj, desr, order ) \
  _CPU_atomic_Store_uint( obj, desr, order )

#define _Atomic_Store_ulong( obj, desr, order ) \
  _CPU_atomic_Store_ulong( obj, desr, order )

#define _Atomic_Store_ptr( obj, desr, order ) \
  _CPU_atomic_Store_ptr( obj, desr, order )

#define _Atomic_Fetch_add_uint( obj, arg, order ) \
  _CPU_atomic_Fetch_add_uint( obj, arg, order )

#define _Atomic_Fetch_add_ulong( obj, arg, order ) \
  _CPU_atomic_Fetch_add_ulong( obj, arg, order )

#define _Atomic_Fetch_add_ptr( obj, arg, order ) \
  _CPU_atomic_Fetch_add_ptr( obj, arg, order )

#define _Atomic_Fetch_sub_uint( obj, arg, order ) \
  _CPU_atomic_Fetch_sub_uint( obj, arg, order )

#define _Atomic_Fetch_sub_ulong( obj, arg, order ) \
  _CPU_atomic_Fetch_sub_ulong( obj, arg, order )

#define _Atomic_Fetch_sub_ptr( obj, arg, order ) \
  _CPU_atomic_Fetch_sub_ptr( obj, arg, order )

#define _Atomic_Fetch_or_uint( obj, arg, order ) \
  _CPU_atomic_Fetch_or_uint( obj, arg, order )

#define _Atomic_Fetch_or_ulong( obj, arg, order ) \
  _CPU_atomic_Fetch_or_ulong( obj, arg, order )

#define _Atomic_Fetch_or_ptr( obj, arg, order ) \
  _CPU_atomic_Fetch_or_ptr( obj, arg, order )

#define _Atomic_Fetch_and_uint( obj, arg, order ) \
  _CPU_atomic_Fetch_and_uint( obj, arg, order )

#define _Atomic_Fetch_and_ulong( obj, arg, order ) \
  _CPU_atomic_Fetch_and_ulong( obj, arg, order )

#define _Atomic_Fetch_and_ptr( obj, arg, order ) \
  _CPU_atomic_Fetch_and_ptr( obj, arg, order )

#define _Atomic_Exchange_uint( obj, desr, order ) \
  _CPU_atomic_Exchange_uint( obj, desr, order )

#define _Atomic_Exchange_ulong( obj, desr, order ) \
  _CPU_atomic_Exchange_ulong( obj, desr, order )

#define _Atomic_Exchange_ptr( obj, desr, order ) \
  _CPU_atomic_Exchange_ptr( obj, desr, order )

#define _Atomic_Compare_exchange_uint( obj, expected, desired, succ, fail ) \
  _CPU_atomic_Compare_exchange_uint( obj, expected, desired, succ, fail )

#define _Atomic_Compare_exchange_ulong( obj, expected, desired, succ, fail ) \
  _CPU_atomic_Compare_exchange_ulong( obj, expected, desired, succ, fail )

#define _Atomic_Compare_exchange_ptr( obj, expected, desired, succ, fail ) \
  _CPU_atomic_Compare_exchange_ptr( obj, expected, desired, succ, fail )

#define _Atomic_Flag_clear( obj, order ) \
  _CPU_atomic_Flag_clear( obj, order )

#define _Atomic_Flag_test_and_set( obj, order ) \
  _CPU_atomic_Flag_test_and_set( obj, order )

/** @} */

#endif /* _RTEMS_SCORE_ATOMIC_H */
