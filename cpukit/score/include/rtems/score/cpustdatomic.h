/**
 * @file  rtems/score/cpustdatomic.h
 * 
 * This include file defines the generic data struct and implementation
 * based on stdatomic.h for all the support architectures. You should not
 * include this header file directly, because it will be used by atomic.h
 * which should be included by score components
 */

/*
 * COPYRIGHT (c) 2013 Deng Hengyi.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_GENERAL_STDATOMIC_CPU_H_
#define _RTEMS_SCORE_GENERAL_STDATOMIC_CPU_H_

#include <stdatomic.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS general stdatomic data type and implementation.
 *
 */

/**@{*/

/**
 * @brief atomic operation unsigned integer type
 */
typedef atomic_ulong Atomic_Ulong;

/**
 * @brief atomic operation unsigned integer the size of a pointer type
 */
typedef atomic_uintptr_t Atomic_Pointer;

/**
 * @brief atomic operation flag type
 */
typedef atomic_flag Atomic_Flag;

/**
 * @brief the enumeration Atomic_Memory_barrier specifies the detailed regular
 * memory synchronization operations used in the atomic operation API
 * definitions.
 */
typedef enum {
  /** no operation orders memory. */
  ATOMIC_ORDER_RELAXED = memory_order_relaxed,
  /** a load operation performs an acquire operation on the affected memory
  * location. This flag guarantees that the effects of load operation are
  * completed before the effects of any later data accesses.
  */
  ATOMIC_ORDER_ACQUIRE = memory_order_acquire,
  /** a store operation performs a release operation on the affected memory
  * location. This flag guarantee that all effects of all previous data
  * accesses are completed before the store operation takes place.
  */
  ATOMIC_ORDER_RELEASE = memory_order_release
} Atomic_Order;


/**
 * @brief atomic data initializer for static initialization.
 */
#define CPU_ATOMIC_INITIALIZER_ULONG(value) ATOMIC_VAR_INIT(value)
#define CPU_ATOMIC_INITIALIZER_PTR(pointer) \
  ATOMIC_VAR_INIT((uintptr_t) pointer)

#define CPU_ATOMIC_INITIALIZER_FLAG ATOMIC_FLAG_INIT

/**
 * @brief Initializes an atomic type value into a atomic object.
 *
 * @param object an atomic type pointer of object.
 * @param value a value to be stored into object.
 */
static inline void _CPU_atomic_Init_ulong(
  volatile Atomic_Ulong *object,
  unsigned long value
)
{
  atomic_init(object, value);
}

static inline void _CPU_atomic_Init_ptr(
  volatile Atomic_Pointer *object,
  void *pointer
)
{
  atomic_init(object, pointer);
}

/**
 * @brief Atomically load an atomic type value from atomic object.
 *
 * @param object an atomic type pointer of object.
 * @param order a type of Atomic_Order. 
 * 
 * The order shall not be ATOMIC_ORDER_RELEASE.
 */
static inline unsigned long _CPU_atomic_Load_ulong(
  volatile Atomic_Ulong *object,
  Atomic_Order order
)
{
  return atomic_load_explicit( object, order );
}

static inline void *_CPU_atomic_Load_ptr(
  volatile Atomic_Pointer *object,
  Atomic_Order order
)
{
  return atomic_load_explicit( object, order );
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
static inline void _CPU_atomic_Store_ulong(
  volatile Atomic_Ulong *object,
  unsigned long value,
  Atomic_Order order
)
{
  atomic_store_explicit( object, value, order );
}

static inline void _CPU_atomic_Store_ptr(
  volatile Atomic_Pointer *object,
  void *pointer,
  Atomic_Order order
)
{
  atomic_store_explicit( object, pointer, order );
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
static inline unsigned long _CPU_atomic_Fetch_add_ulong(
  volatile Atomic_Ulong *object,
  unsigned long value,
  Atomic_Order order
)
{
  return atomic_fetch_add_explicit( object, value, order );
}

static inline uintptr_t _CPU_atomic_Fetch_add_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value,
  Atomic_Order order
)
{
  return atomic_fetch_add_explicit( object, value, order );
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
static inline unsigned long _CPU_atomic_Fetch_sub_ulong(
  volatile Atomic_Ulong *object,
  unsigned long value,
  Atomic_Order order
)
{
  return atomic_fetch_sub_explicit( object, value, order );
}

static inline uintptr_t _CPU_atomic_Fetch_sub_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value,
  Atomic_Order order
)
{
  return atomic_fetch_sub_explicit( object, value, order );
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
static inline unsigned long _CPU_atomic_Fetch_or_ulong(
  volatile Atomic_Ulong *object,
  unsigned long value,
  Atomic_Order order
)
{
  return atomic_fetch_or_explicit( object, value, order );
}

static inline uintptr_t _CPU_atomic_Fetch_or_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value,
  Atomic_Order order
)
{
  return atomic_fetch_or_explicit( object, value, order );
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
static inline unsigned long _CPU_atomic_Fetch_and_ulong(
  volatile Atomic_Ulong *object,
  unsigned long value,
  Atomic_Order order
)
{
  return atomic_fetch_and_explicit( object, value, order );
}

static inline uintptr_t _CPU_atomic_Fetch_and_ptr(
  volatile Atomic_Pointer *object,
  uintptr_t value,
  Atomic_Order order
)
{
  return atomic_fetch_and_explicit( object, value, order );
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
static inline unsigned long _CPU_atomic_Exchange_ulong(
 volatile Atomic_Ulong *object,
 unsigned long value,
 Atomic_Order order
)
{
  return atomic_exchange_explicit( object, value, order );
}

static inline void *_CPU_atomic_Exchange_ptr(
 volatile Atomic_Pointer *object,
 void *pointer,
 Atomic_Order order
)
{
  return atomic_exchange_explicit( object, pointer, pointer );
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
static inline bool _CPU_atomic_Compare_exchange_ulong(
  volatile Atomic_Ulong *object,
  unsigned long *old_value,
  unsigned long new_value,
  Atomic_Order order_succ,
  Atomic_Order order_fail
)
{
  return atomic_compare_exchange_strong_explicit( object, old_value,
    new_value, order_succ, order_fail );
}

static inline bool _CPU_atomic_Compare_exchange_ptr(
  volatile Atomic_Pointer *object,
  void **old_pointer,
  void *new_pointer,
  Atomic_Order order_succ,
  Atomic_Order order_fail
)
{
  return atomic_compare_exchange_strong_explicit( object, old_pointer,
    new_pointer, order_succ, order_fail );
}

static inline void _CPU_atomic_Flag_clear(
  volatile Atomic_Flag *object,
  Atomic_Order order
)
{
  return atomic_flag_clear_explicit( object, order );
}

static inline bool _CPU_atomic_Flag_test_and_set(
  volatile Atomic_Flag *object,
  Atomic_Order order
)
{
  return atomic_flag_test_and_set_explicit( object, order );
}

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
