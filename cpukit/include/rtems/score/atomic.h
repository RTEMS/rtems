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
 * Copyright (C) 2015 embedded brains GmbH & Co. KG
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

#include <rtems/score/basedefs.h>

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

#ifdef RTEMS_SMP
  #if defined(__cplusplus) \
    && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9))
    /*
     * The GCC 4.9 ships its own <stdatomic.h> which is not C++ compatible.  The
     * suggested solution was to include <atomic> in case C++ is used.  This works
     * at least with GCC 4.9.  See also:
     *
     * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60932
     * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60940
     */
    #include <atomic>
    #define _RTEMS_SCORE_ATOMIC_USE_ATOMIC
  #else
    #include <stdatomic.h>
    #define _RTEMS_SCORE_ATOMIC_USE_STDATOMIC
  #endif
#else
  #include <rtems/score/isrlevel.h>
#endif

#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)

typedef std::atomic_uint Atomic_Uint;

typedef std::atomic_ulong Atomic_Ulong;

typedef std::atomic_uintptr_t Atomic_Uintptr;

typedef std::atomic_flag Atomic_Flag;

typedef std::memory_order Atomic_Order;

#define ATOMIC_ORDER_RELAXED std::memory_order_relaxed

#define ATOMIC_ORDER_ACQUIRE std::memory_order_acquire

#define ATOMIC_ORDER_RELEASE std::memory_order_release

#define ATOMIC_ORDER_ACQ_REL std::memory_order_acq_rel

#define ATOMIC_ORDER_SEQ_CST std::memory_order_seq_cst

#define ATOMIC_INITIALIZER_UINT( value ) ATOMIC_VAR_INIT( value )

#define ATOMIC_INITIALIZER_ULONG( value ) ATOMIC_VAR_INIT( value )

#define ATOMIC_INITIALIZER_UINTPTR( value ) ATOMIC_VAR_INIT( value )

#define ATOMIC_INITIALIZER_FLAG ATOMIC_FLAG_INIT

#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)

typedef atomic_uint Atomic_Uint;

typedef atomic_ulong Atomic_Ulong;

typedef atomic_uintptr_t Atomic_Uintptr;

typedef atomic_flag Atomic_Flag;

typedef memory_order Atomic_Order;

#define ATOMIC_ORDER_RELAXED memory_order_relaxed

#define ATOMIC_ORDER_ACQUIRE memory_order_acquire

#define ATOMIC_ORDER_RELEASE memory_order_release

#define ATOMIC_ORDER_ACQ_REL memory_order_acq_rel

#define ATOMIC_ORDER_SEQ_CST memory_order_seq_cst

#define ATOMIC_INITIALIZER_UINT( value ) ATOMIC_VAR_INIT( value )

#define ATOMIC_INITIALIZER_ULONG( value ) ATOMIC_VAR_INIT( value )

#define ATOMIC_INITIALIZER_UINTPTR( value ) ATOMIC_VAR_INIT( value )

#define ATOMIC_INITIALIZER_FLAG ATOMIC_FLAG_INIT

#else

typedef unsigned int Atomic_Uint;

typedef unsigned long Atomic_Ulong;

typedef uintptr_t Atomic_Uintptr;

typedef bool Atomic_Flag;

typedef int Atomic_Order;

#define ATOMIC_ORDER_RELAXED 0

#define ATOMIC_ORDER_ACQUIRE 2

#define ATOMIC_ORDER_RELEASE 3

#define ATOMIC_ORDER_ACQ_REL 4

#define ATOMIC_ORDER_SEQ_CST 5

#define ATOMIC_INITIALIZER_UINT( value ) ( value )

#define ATOMIC_INITIALIZER_ULONG( value ) ( value )

#define ATOMIC_INITIALIZER_UINTPTR( value ) ( value )

#define ATOMIC_INITIALIZER_FLAG false

#endif

/**
 * @brief Sets up a cpu fence.
 *
 * @param[out] order The order for the fence.
 */
static inline void _Atomic_Fence( Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  std::atomic_thread_fence( order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  atomic_thread_fence( order );
#else
  (void) order;
  RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

/**
 * @brief Initializes Uint.
 *
 * @param[out] obj The CPU atomic Uint to initialize.
 * @param desired The desired value for @a obj.
 */
static inline void _Atomic_Init_uint( Atomic_Uint *obj, unsigned int desired )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  obj->store( desired );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  atomic_init( obj, desired );
#else
  *obj = desired;
#endif
}

/**
 * @brief Initializes Ulong.
 *
 * @param[out] obj The CPU atomic Ulong to initialize.
 * @param desired The desired value for @a obj.
 */
static inline void _Atomic_Init_ulong( Atomic_Ulong *obj, unsigned long desired )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  obj->store( desired );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  atomic_init( obj, desired );
#else
  *obj = desired;
#endif
}

/**
 * @brief Initializes Uintptr.
 *
 * @param[out] obj The CPU atomic Uintptr to initialize.
 * @param desired The desired value for @a obj.
 */
static inline void _Atomic_Init_uintptr( Atomic_Uintptr *obj, uintptr_t desired )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  obj->store( desired );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  atomic_init( obj, desired );
#else
  *obj = desired;
#endif
}

/**
 * @brief Loads value of Uint considering the order.
 *
 * @param obj The CPU atomic Uint to get the value from.
 * @param order The atomic order for getting the value.
 *
 * @return The value of @a obj considering the @a order.
 */
static inline unsigned int _Atomic_Load_uint( const Atomic_Uint *obj, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->load( order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_load_explicit( obj, order );
#else
  unsigned int val;

  (void) order;
  val = *obj;
  RTEMS_COMPILER_MEMORY_BARRIER();

  return val;
#endif
}

/**
 * @brief Loads value of Ulong considering the order.
 *
 * @param obj The CPU atomic Ulong to get the value from.
 * @param order The atomic order for getting the value.
 *
 * @return The value of @a obj considering the @a order.
 */
static inline unsigned long _Atomic_Load_ulong( const Atomic_Ulong *obj, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->load( order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_load_explicit( obj, order );
#else
  unsigned long val;

  (void) order;
  val = *obj;
  RTEMS_COMPILER_MEMORY_BARRIER();

  return val;
#endif
}

/**
 * @brief Loads value of Uintptr considering the order.
 *
 * @param obj The CPU atomic Uintptr to get the value from.
 * @param order The atomic order for getting the value.
 *
 * @return The value of @a obj considering the @a order.
 */
static inline uintptr_t _Atomic_Load_uintptr( const Atomic_Uintptr *obj, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->load( order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_load_explicit( obj, order );
#else
  uintptr_t val;

  (void) order;
  val = *obj;
  RTEMS_COMPILER_MEMORY_BARRIER();

  return val;
#endif
}

/**
 * @brief Stores a value to Uint considering the order.
 *
 * @param[out] obj The CPU atomic Uint to store a value in.
 * @param desired The desired value for @a obj.
 * @param order The atomic order for storing the value.
 */
static inline void _Atomic_Store_uint( Atomic_Uint *obj, unsigned int desired, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  obj->store( desired, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  atomic_store_explicit( obj, desired, order );
#else
  (void) order;
  RTEMS_COMPILER_MEMORY_BARRIER();
  *obj = desired;
#endif
}

/**
 * @brief Stores a value to Ulong considering the order.
 *
 * @param[out] obj The CPU atomic Ulong to store a value in.
 * @param desired The desired value for @a obj.
 * @param order The atomic order for storing the value.
 */
static inline void _Atomic_Store_ulong( Atomic_Ulong *obj, unsigned long desired, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  obj->store( desired, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  atomic_store_explicit( obj, desired, order );
#else
  (void) order;
  RTEMS_COMPILER_MEMORY_BARRIER();
  *obj = desired;
#endif
}

/**
 * @brief Stores a value to Uintptr considering the order.
 *
 * @param[out] obj The CPU atomic Uintptr to store a value in.
 * @param desired The desired value for @a obj.
 * @param order The atomic order for storing the value.
 */
static inline void _Atomic_Store_uintptr( Atomic_Uintptr *obj, uintptr_t desired, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  obj->store( desired, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  atomic_store_explicit( obj, desired, order );
#else
  (void) order;
  RTEMS_COMPILER_MEMORY_BARRIER();
  *obj = desired;
#endif
}

/**
 * @brief Fetches current value of Uint and adds a value to the stored value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and add @a arg to.
 * @param arg The value to add to @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the addition of @a arg.
 */
static inline unsigned int _Atomic_Fetch_add_uint( Atomic_Uint *obj, unsigned int arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_add( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_add_explicit( obj, arg, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val + arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and adds a value to the stored value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and add @a arg to.
 * @param arg The value to add to @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the addition of @a arg.
 */
static inline unsigned long _Atomic_Fetch_add_ulong( Atomic_Ulong *obj, unsigned long arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_add( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_add_explicit( obj, arg, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val + arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and adds a value to the stored value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and add @a arg to.
 * @param arg The value to add to @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the addition of @a arg.
 */
static inline uintptr_t _Atomic_Fetch_add_uintptr( Atomic_Uintptr *obj, uintptr_t arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_add( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_add_explicit( obj, arg, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val + arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uint and subtracts a value from the stored value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and subtract @a arg from.
 * @param arg The value to subtract from @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the subtraction of @a arg.
 */
static inline unsigned int _Atomic_Fetch_sub_uint( Atomic_Uint *obj, unsigned int arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_sub( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_sub_explicit( obj, arg, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val - arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and subtracts a value from the stored value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and subtract @a arg from.
 * @param arg The value to subtract from @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the subtraction of @a arg.
 */
static inline unsigned long _Atomic_Fetch_sub_ulong( Atomic_Ulong *obj, unsigned long arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_sub( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_sub_explicit( obj, arg, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val - arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and subtracts a value from the stored value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and subtract @a arg from.
 * @param arg The value to subtract from @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the subtraction of @a arg.
 */
static inline uintptr_t _Atomic_Fetch_sub_uintptr( Atomic_Uintptr *obj, uintptr_t arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_sub( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_sub_explicit( obj, arg, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val - arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uint and ORs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and OR @a arg to.
 * @param arg The value to OR with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the OR operation with @a arg.
 */
static inline unsigned int _Atomic_Fetch_or_uint( Atomic_Uint *obj, unsigned int arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_or( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_or_explicit( obj, arg, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val | arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and ORs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and OR @a arg to.
 * @param arg The value to OR with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the OR operation with @a arg.
 */
static inline unsigned long _Atomic_Fetch_or_ulong( Atomic_Ulong *obj, unsigned long arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_or( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_or_explicit( obj, arg, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val | arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and ORs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and OR @a arg to.
 * @param arg The value to OR with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the OR operation with @a arg.
 */
static inline uintptr_t _Atomic_Fetch_or_uintptr( Atomic_Uintptr *obj, uintptr_t arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_or( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_or_explicit( obj, arg, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val | arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uint and ANDs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and AND @a arg to.
 * @param arg The value to AND with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the AND operation with @a arg.
 */
static inline unsigned int _Atomic_Fetch_and_uint( Atomic_Uint *obj, unsigned int arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_and( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_and_explicit( obj, arg, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val & arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and ANDs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and AND @a arg to.
 * @param arg The value to AND with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the AND operation with @a arg.
 */
static inline unsigned long _Atomic_Fetch_and_ulong( Atomic_Ulong *obj, unsigned long arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_and( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_and_explicit( obj, arg, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val & arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and ANDs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and AND @a arg to.
 * @param arg The value to AND with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the AND operation with @a arg.
 */
static inline uintptr_t _Atomic_Fetch_and_uintptr( Atomic_Uintptr *obj, uintptr_t arg, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->fetch_and( arg, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_fetch_and_explicit( obj, arg, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val & arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uint and sets its value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and set the value to @a desired.
 * @param arg The value to set for @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the exchange with @a desired.
 */
static inline unsigned int _Atomic_Exchange_uint( Atomic_Uint *obj, unsigned int desired, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->exchange( desired, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_exchange_explicit( obj, desired, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = desired;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and sets its value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and set the value to @a desired.
 * @param arg The value to set for @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the exchange with @a desired.
 */
static inline unsigned long _Atomic_Exchange_ulong( Atomic_Ulong *obj, unsigned long desired, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->exchange( desired, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_exchange_explicit( obj, desired, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = desired;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and sets its value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and set the value to @a desired.
 * @param arg The value to set for @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the exchange with @a desired.
 */
static inline uintptr_t _Atomic_Exchange_uintptr( Atomic_Uintptr *obj, uintptr_t desired, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->exchange( desired, order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_exchange_explicit( obj, desired, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = desired;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Checks if value of Uint is as expected.
 *
 * This method checks if the value of @a obj is equal to the value of @a expected.  If
 * this is the case, the value of @a obj is changed to @a desired.  Otherwise, the value
 * of @a obj is changed to @a expected.
 *
 * @param[in, out] obj The CPU atomic Uint to operate upon.
 * @param[in, out] expected The expected value of @a obj.  If @a obj has a different
 *      value, @a expected is changed to the actual value of @a obj.
 * @param desired The new value of @a obj if the old value of @a obj was as expected.
 * @param succ The order if it is successful.
 * @param fail The order if it fails.
 *
 * @retval true The old value of @a obj was as expected.
 * @retval false The old value of @a obj was not as expected.
 */
static inline bool _Atomic_Compare_exchange_uint( Atomic_Uint *obj, unsigned int *expected, unsigned int desired, Atomic_Order succ, Atomic_Order fail )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->compare_exchange_strong( *expected, desired, succ, fail );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_compare_exchange_strong_explicit( obj, expected, desired, succ, fail );
#else
  bool success;
  ISR_Level level;
  unsigned int actual;

  (void) succ;
  (void) fail;
  _ISR_Local_disable( level );
  actual = *obj;
  success = ( actual == *expected );
  if ( success ) {
    *obj = desired;
  } else {
    *expected = actual;
  }
  _ISR_Local_enable( level );

  return success;
#endif
}

/**
 * @brief Checks if value of Ulong is as expected.
 *
 * This method checks if the value of @a obj is equal to the value of @a expected.  If
 * this is the case, the value of @a obj is changed to @a desired.  Otherwise, the value
 * of @a obj is changed to @a expected.
 *
 * @param[in, out] obj The CPU atomic Ulong to operate upon.
 * @param[in, out] expected The expected value of @a obj.  If @a obj has a different
 *      value, @a expected is changed to the actual value of @a obj.
 * @param desired The new value of @a obj if the old value of @a obj was as expected.
 * @param succ The order if it is successful.
 * @param fail The order if it fails.
 *
 * @retval true The old value of @a obj was as expected.
 * @retval false The old value of @a obj was not as expected.
 */
static inline bool _Atomic_Compare_exchange_ulong( Atomic_Ulong *obj, unsigned long *expected, unsigned long desired, Atomic_Order succ, Atomic_Order fail )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->compare_exchange_strong( *expected, desired, succ, fail );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_compare_exchange_strong_explicit( obj, expected, desired, succ, fail );
#else
  bool success;
  ISR_Level level;
  unsigned long actual;

  (void) succ;
  (void) fail;
  _ISR_Local_disable( level );
  actual = *obj;
  success = ( actual == *expected );
  if ( success ) {
    *obj = desired;
  } else {
    *expected = actual;
  }
  _ISR_Local_enable( level );

  return success;
#endif
}

/**
 * @brief Checks if value of Uintptr is as expected.
 *
 * This method checks if the value of @a obj is equal to the value of @a expected.  If
 * this is the case, the value of @a obj is changed to @a desired.  Otherwise, the value
 * of @a obj is changed to @a expected.
 *
 * @param[in, out] obj The CPU atomic Uintptr to operate upon.
 * @param[in, out] expected The expected value of @a obj.  If @a obj has a different
 *      value, @a expected is changed to the actual value of @a obj.
 * @param desired The new value of @a obj if the old value of @a obj was as expected.
 * @param succ The order if it is successful.
 * @param fail The order if it fails.
 *
 * @retval true The old value of @a obj was as expected.
 * @retval false The old value of @a obj was not as expected.
 */
static inline bool _Atomic_Compare_exchange_uintptr( Atomic_Uintptr *obj, uintptr_t *expected, uintptr_t desired, Atomic_Order succ, Atomic_Order fail )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->compare_exchange_strong( *expected, desired, succ, fail );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_compare_exchange_strong_explicit( obj, expected, desired, succ, fail );
#else
  bool success;
  ISR_Level level;
  uintptr_t actual;

  (void) succ;
  (void) fail;
  _ISR_Local_disable( level );
  actual = *obj;
  success = ( actual == *expected );
  if ( success ) {
    *obj = desired;
  } else {
    *expected = actual;
  }
  _ISR_Local_enable( level );

  return success;
#endif
}

/**
 * @brief Clears the atomic flag.
 *
 * @param[out] obj The atomic flag to be cleared.
 * @param order The atomic order for the operation.
 */
static inline void _Atomic_Flag_clear( Atomic_Flag *obj, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  obj->clear( order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  atomic_flag_clear_explicit( obj, order );
#else
  (void) order;
  *obj = false;
#endif
}

/**
 * @brief Returns current flag state and sets it.
 *
 * @param[in, out] obj The atomic flag to be set.
 * @param order The atomic order for the operation.
 *
 * @retval true @a obj was set prior to this operation.
 * @retval false @a obj was not set prior to this operation.
 */
static inline bool _Atomic_Flag_test_and_set( Atomic_Flag *obj, Atomic_Order order )
{
#if defined(_RTEMS_SCORE_ATOMIC_USE_ATOMIC)
  return obj->test_and_set( order );
#elif defined(_RTEMS_SCORE_ATOMIC_USE_STDATOMIC)
  return atomic_flag_test_and_set_explicit( obj, order );
#else
  bool flag;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  flag = *obj;
  *obj = true;
  _ISR_Local_enable( level );

  return flag;
#endif
}

/** @} */

#endif /* _RTEMS_SCORE_ATOMIC_H */
