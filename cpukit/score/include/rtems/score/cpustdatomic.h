/**
 * @file
 * 
 * @brief Atomic Operations CPU API
 */

/*
 * COPYRIGHT (c) 2013 Deng Hengyi.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUSTDATOMIC_H
#define _RTEMS_SCORE_CPUSTDATOMIC_H

#include <stdint.h>

#if defined(__cplusplus) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 9

/*
 * The GCC 4.9 ships its own <stdatomic.h> which is not C++ compatible.  The
 * suggested solution was to include <atomic> in case C++ is used.  This works
 * at least with GCC 4.9.  See also:
 *
 * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60932
 * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60940
 */

#include <atomic>

typedef std::atomic_uint CPU_atomic_Uint;

typedef std::atomic_ulong CPU_atomic_Ulong;

typedef std::atomic_uintptr_t CPU_atomic_Pointer;

typedef std::atomic_flag CPU_atomic_Flag;

#define CPU_ATOMIC_ORDER_RELAXED std::memory_order_relaxed

#define CPU_ATOMIC_ORDER_ACQUIRE std::memory_order_acquire

#define CPU_ATOMIC_ORDER_RELEASE std::memory_order_release

#define CPU_ATOMIC_ORDER_SEQ_CST std::memory_order_seq_cst

#define CPU_ATOMIC_INITIALIZER_UINT( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_ULONG( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_PTR( value ) \
  ATOMIC_VAR_INIT( (uintptr_t) (value) )

#define CPU_ATOMIC_INITIALIZER_FLAG ATOMIC_FLAG_INIT

#define _CPU_atomic_Fence( order ) atomic_thread_fence( order )

#define _CPU_atomic_Init_uint( obj, desired ) \
  (obj)->store( desired )

#define _CPU_atomic_Init_ulong( obj, desired ) \
  (obj)->store( desired )

#define _CPU_atomic_Init_ptr( obj, desired ) \
  (obj)->store( desired )

#define _CPU_atomic_Load_uint( obj, order ) \
  (obj)->load( order )

#define _CPU_atomic_Load_ulong( obj, order ) \
  (obj)->load( order )

#define _CPU_atomic_Load_ptr( obj, order ) \
  (void *) (obj)->load( order )

#define _CPU_atomic_Store_uint( obj, desr, order ) \
  (obj)->store( desr, order )

#define _CPU_atomic_Store_ulong( obj, desr, order ) \
  (obj)->store( desr, order )

#define _CPU_atomic_Store_ptr( obj, desr, order ) \
  (obj)->store( (uintptr_t) desr, order )

#define _CPU_atomic_Fetch_add_uint( obj, arg, order ) \
  (obj)->fetch_add( arg, order )

#define _CPU_atomic_Fetch_add_ulong( obj, arg, order ) \
  (obj)->fetch_add( arg, order )

#define _CPU_atomic_Fetch_add_ptr( obj, arg, order ) \
  (obj)->fetch_add( arg, (uintptr_t) order )

#define _CPU_atomic_Fetch_sub_uint( obj, arg, order ) \
  (obj)->fetch_sub( arg, order )

#define _CPU_atomic_Fetch_sub_ulong( obj, arg, order ) \
  (obj)->fetch_sub( arg, order )

#define _CPU_atomic_Fetch_sub_ptr( obj, arg, order ) \
  (obj)->fetch_sub( arg, (uintptr_t) order )

#define _CPU_atomic_Fetch_or_uint( obj, arg, order ) \
  (obj)->fetch_or( arg, order )

#define _CPU_atomic_Fetch_or_ulong( obj, arg, order ) \
  (obj)->fetch_or( arg, order )

#define _CPU_atomic_Fetch_or_ptr( obj, arg, order ) \
  (obj)->fetch_or( arg, (uintptr_t) order )

#define _CPU_atomic_Fetch_and_uint( obj, arg, order ) \
  (obj)->fetch_and( arg, order )

#define _CPU_atomic_Fetch_and_ulong( obj, arg, order ) \
  (obj)->fetch_and( arg, order )

#define _CPU_atomic_Fetch_and_ptr( obj, arg, order ) \
  (obj)->fetch_and( arg, (uintptr_t) order )

#define _CPU_atomic_Exchange_uint( obj, desr, order ) \
  (obj)->exchange( desr, order )

#define _CPU_atomic_Exchange_ulong( obj, desr, order ) \
  (obj)->exchange( desr, order )

#define _CPU_atomic_Exchange_ptr( obj, desr, order ) \
  (void *) (obj)->exchange( desr, (uintptr_t) order )

#define _CPU_atomic_Compare_exchange_uint( obj, expected, desired, succ, fail ) \
  (obj)->compare_exchange_strong( expected, desired, succ, fail )

#define _CPU_atomic_Compare_exchange_ulong( obj, expected, desired, succ, fail ) \
  (obj)->compare_exchange_strong( expected, desired, succ, fail )

#define _CPU_atomic_Compare_exchange_ptr( obj, expected, desired, succ, fail ) \
  (obj)->compare_exchange_strong( (void **) expected, (uintptr_t) desired, succ, fail )

#define _CPU_atomic_Flag_clear( obj, order ) \
  (obj)->clear( order )

#define _CPU_atomic_Flag_test_and_set( obj, order ) \
  (obj)->test_and_set( order )

#else /* __cplusplus */

#include <stdatomic.h>

typedef atomic_uint CPU_atomic_Uint;

typedef atomic_ulong CPU_atomic_Ulong;

typedef atomic_uintptr_t CPU_atomic_Pointer;

typedef atomic_flag CPU_atomic_Flag;

#define CPU_ATOMIC_ORDER_RELAXED memory_order_relaxed

#define CPU_ATOMIC_ORDER_ACQUIRE memory_order_acquire

#define CPU_ATOMIC_ORDER_RELEASE memory_order_release

#define CPU_ATOMIC_ORDER_SEQ_CST memory_order_seq_cst

#define CPU_ATOMIC_INITIALIZER_UINT( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_ULONG( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_PTR( value ) \
  ATOMIC_VAR_INIT( (uintptr_t) (value) )

#define CPU_ATOMIC_INITIALIZER_FLAG ATOMIC_FLAG_INIT

#define _CPU_atomic_Fence( order ) atomic_thread_fence( order )

#define _CPU_atomic_Init_uint( obj, desired ) \
  atomic_init( obj, desired )

#define _CPU_atomic_Init_ulong( obj, desired ) \
  atomic_init( obj, desired )

#define _CPU_atomic_Init_ptr( obj, desired ) \
  atomic_init( obj, (uintptr_t) desired )

#define _CPU_atomic_Load_uint( obj, order ) \
  atomic_load_explicit( obj, order )

#define _CPU_atomic_Load_ulong( obj, order ) \
  atomic_load_explicit( obj, order )

#define _CPU_atomic_Load_ptr( obj, order ) \
  (void *) atomic_load_explicit( obj, order )

#define _CPU_atomic_Store_uint( obj, desr, order ) \
  atomic_store_explicit( obj, desr, order )

#define _CPU_atomic_Store_ulong( obj, desr, order ) \
  atomic_store_explicit( obj, desr, order )

#define _CPU_atomic_Store_ptr( obj, desr, order ) \
  atomic_store_explicit( obj, desr, order )

#define _CPU_atomic_Fetch_add_uint( obj, arg, order ) \
  atomic_fetch_add_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_add_ulong( obj, arg, order ) \
  atomic_fetch_add_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_add_ptr( obj, arg, order ) \
  atomic_fetch_add_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_sub_uint( obj, arg, order ) \
  atomic_fetch_sub_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_sub_ulong( obj, arg, order ) \
  atomic_fetch_sub_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_sub_ptr( obj, arg, order ) \
  atomic_fetch_sub_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_or_uint( obj, arg, order ) \
  atomic_fetch_or_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_or_ulong( obj, arg, order ) \
  atomic_fetch_or_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_or_ptr( obj, arg, order ) \
  atomic_fetch_or_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_and_uint( obj, arg, order ) \
  atomic_fetch_and_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_and_ulong( obj, arg, order ) \
  atomic_fetch_and_explicit( obj, arg, order )

#define _CPU_atomic_Fetch_and_ptr( obj, arg, order ) \
  atomic_fetch_and_explicit( obj, arg, order )

#define _CPU_atomic_Exchange_uint( obj, desr, order ) \
  atomic_exchange_explicit( obj, desr, order )

#define _CPU_atomic_Exchange_ulong( obj, desr, order ) \
  atomic_exchange_explicit( obj, desr, order )

#define _CPU_atomic_Exchange_ptr( obj, desr, order ) \
  atomic_exchange_explicit( obj, desr, order )

#define _CPU_atomic_Compare_exchange_uint( obj, expected, desired, succ, fail ) \
  atomic_compare_exchange_strong_explicit( obj, expected, desired, succ, fail )

#define _CPU_atomic_Compare_exchange_ulong( obj, expected, desired, succ, fail ) \
  atomic_compare_exchange_strong_explicit( obj, expected, desired, succ, fail )

#define _CPU_atomic_Compare_exchange_ptr( obj, expected, desired, succ, fail ) \
  atomic_compare_exchange_strong_explicit( obj, expected, desired, succ, fail )

#define _CPU_atomic_Flag_clear( obj, order ) \
  atomic_flag_clear_explicit( obj, order )

#define _CPU_atomic_Flag_test_and_set( obj, order ) \
  atomic_flag_test_and_set_explicit( obj, order )

#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_CPUSTDATOMIC_H */
