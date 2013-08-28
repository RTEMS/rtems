/**
 * @file  rtems/score/atomic.h
 * 
 * This include file defines the interface for all the atomic
 * operations which can be used in the synchronization primitives
 * or in the lock-less algorithms. You should not use these API
 * in the other components directly. 
 */

/*
 * COPYRIGHT (c) 2012-2013 Deng Hengyi.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ATOMIC_H
#define _RTEMS_SCORE_ATOMIC_H

#include <rtems/score/cpuatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS atomic interface
 *
 */

/**@{*/

/**
 * @brief atomic data initializer for static initialization.
 */
#define ATOMIC_INITIALIZER_UINT(value) CPU_ATOMIC_INITIALIZER_UINT(value)
#define ATOMIC_INITIALIZER_PTR(value) CPU_ATOMIC_INITIALIZER_PTR(value)

/**
 * @brief Initializes an atomic flag object to the cleared state.
 */
#define ATOMIC_INITIALIZER_FLAG CPU_ATOMIC_INITIALIZER_FLAG

/**
 * @brief Initializes an atomic type value into a atomic object.
 *
 * @param object an atomic type pointer of object.
 * @param value a value to be stored into object.
 */
static inline void _Atomic_Init_uint(
  volatile Atomic_Uint *object,
  uint_fast32_t value
)
{
  _CPU_atomic_Init_uint(object, value);
}

static inline void _Atomic_Init_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value
)
{
  _CPU_atomic_Init_ptr(object, value);
}

/**
 * @brief Atomically load an atomic type value from atomic object.
 *
 * @param object an atomic type pointer of object.
 * @param order a type of Atomic_Order. 
 * 
 * The order shall not be ATOMIC_ORDER_RELEASE.
 */
static inline uint_fast32_t _Atomic_Load_uint(
  volatile Atomic_Uint *object,
  Atomic_Order order
)
{
  return _CPU_atomic_Load_uint( object, order );
}

static inline uintptr_t _Atomic_Load_ptr(
  volatile Atomic_Pointer *object,
  Atomic_Order order
)
{
  return _CPU_atomic_Load_ptr( object, order );
}

/**
 * @brief Atomically store an atomic type value into a atomic object.
 *
 * @param object an atomic type pointer of object.
 * @param value a value to be stored into object.
 * @param order a type of Atomic_Order. 
 * 
 * The order shall not be ATOMIC_ORDER_ACQUIRE.
 */
static inline void _Atomic_Store_uint(
  volatile Atomic_Uint *object,
  uint_fast32_t value,
  Atomic_Order order
)
{
  _CPU_atomic_Store_uint( object, value, order );
}

static inline void _Atomic_Store_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value,
  Atomic_Order order
)
{
  _CPU_atomic_Store_ptr( object, value, order );
}

/**
 * @brief Atomically load-add-store an atomic type value into object
 *
 * @param object a atomic type pointer of object.
 * @param value a value to be add and store into object.
 * @param order a type of Atomic_Order. 
 * 
 * @retval a result value before add ops.
 */
static inline uint_fast32_t _Atomic_Fetch_add_uint(
  volatile Atomic_Uint *object,
  uint_fast32_t value,
  Atomic_Order order
)
{
  return _CPU_atomic_Fetch_add_uint( object, value, order );
}

static inline uintptr_t _Atomic_Fetch_add_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value,
  Atomic_Order order
)
{
  return _CPU_atomic_Fetch_add_ptr( object, value, order );
}

/**
 * @brief Atomically load-sub-store an atomic type value into object
 *
 * @param object a atomic type pointer of object.
 * @param value a value to be sub and store into object.
 * @param order a type of Atomic_Order. 
 * 
 * @retval a result value before sub ops.
 */
static inline uint_fast32_t _Atomic_Fetch_sub_uint(
  volatile Atomic_Uint *object,
  uint_fast32_t value,
  Atomic_Order order
)
{
  return _CPU_atomic_Fetch_sub_uint( object, value, order );
}

static inline uintptr_t _Atomic_Fetch_sub_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value,
  Atomic_Order order
)
{
  return _CPU_atomic_Fetch_sub_ptr( object, value, order );
}

/**
 * @brief Atomically load-or-store an atomic type value into object
 *
 * @param object a atomic type pointer of object.
 * @param value a value to be or and store into object.
 * @param order a type of Atomic_Order. 
 * 
 * @retval a result value before or ops.
 */
static inline uint_fast32_t _Atomic_Fetch_or_uint(
  volatile Atomic_Uint *object,
  uint_fast32_t value,
  Atomic_Order order
)
{
  return _CPU_atomic_Fetch_or_uint( object, value, order );
}

static inline uintptr_t _Atomic_Fetch_or_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value,
  Atomic_Order order
)
{
  return _CPU_atomic_Fetch_or_ptr( object, value, order );
}

/**
 * @brief Atomically load-and-store an atomic type value into object
 *
 * @param object a atomic type pointer of object.
 * @param value a value to be and and store into object.
 * @param order a type of Atomic_Order. 
 * 
 * @retval a result value before and ops.
 */
static inline uint_fast32_t _Atomic_Fetch_and_uint(
  volatile Atomic_Uint *object,
  uint_fast32_t value,
  Atomic_Order order
)
{
  return _CPU_atomic_Fetch_and_uint( object, value, order );
}

static inline uintptr_t _Atomic_Fetch_and_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value,
  Atomic_Order order
)
{
  return _CPU_atomic_Fetch_and_ptr( object, value, order );
}

/**
 * @brief Atomically exchange an atomic type value into object
 *
 * @param object a atomic type pointer of object.
 * @param value a value to exchange and and store into object.
 * @param order a type of Atomic_Order. 
 * 
 * @retval a result value before exchange ops.
 */
static inline uint_fast32_t _Atomic_Exchange_uint(
 volatile Atomic_Uint *object,
 uint_fast32_t value,
 Atomic_Order order
)
{
  return _CPU_atomic_Exchange_uint( object, value, order );
}

static inline uintptr_t _Atomic_Exchange_ptr(
 volatile Atomic_Pointer *object,
 uintptr_t value,
 Atomic_Order order
)
{
  return _CPU_atomic_Exchange_ptr( object, value, order );
}

/**
 * @brief Atomically compare the value stored at object with a
 * old_value and if the two values are equal, update the value of a
 * address with a new_value
 *
 * @param object a atomic type pointer of object.
 * @param old_value pointer of a value.
 * @param new_value a atomic type value.
 * @param order_succ a type of Atomic_Order for successful exchange. 
 * @param order_fail a type of Atomic_Order for failed exchange.
 * 
 * @retval true if the compare exchange successully.
 * @retval false if the compare exchange failed.
 */
static inline bool _Atomic_Compare_exchange_uint(
  volatile Atomic_Uint *object,
  uint_fast32_t *old_value,
  uint_fast32_t new_value,
  Atomic_Order order_succ,
  Atomic_Order order_fail
)
{
  return _CPU_atomic_Compare_exchange_uint( object, old_value, new_value,
    order_succ, order_fail );
}

static inline bool _Atomic_Compare_exchange_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t *old_value,
  uintptr_t new_value,
  Atomic_Order order_succ,
  Atomic_Order order_fail
)
{
  return _CPU_atomic_Compare_exchange_ptr( object, old_value, new_value,
    order_succ, order_fail );
}

/**
 * @brief Atomically clears an atomic flag.
 *
 * @param[in, out] object Pointer to the atomic flag object.
 * @param[in] order The atomic memory order.
 * 
 */
static inline void _Atomic_Flag_clear(
  volatile Atomic_Flag *object,
  Atomic_Order order
)
{
  _CPU_atomic_Flag_clear( object, order );
}

/**
 * @brief Atomically tests and sets an atomic flag.
 *
 * @param[in, out] object Pointer to the atomic flag object.
 * @param[in] order The atomic memory order.
 * 
 * @retval true The atomic flag was already set.
 * @retval false Otherwise.
 */
static inline bool _Atomic_Flag_test_and_set(
  volatile Atomic_Flag *object,
  Atomic_Order order
)
{
  return _CPU_atomic_Flag_test_and_set( object, order );
}

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
