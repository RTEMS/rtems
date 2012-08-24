/**
 * @file  rtems/score/atomic.h
 * 
 * This include file defines the interface for all the atomic
 * operations which can be used in the synchronization primitives
 * or in the lock-less algorithms. You should not use these API
 * in the other components directly. 
 */

/*
 * COPYRIGHT (c) 2012 Deng Hengyi.
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
 * @brief the enumeration Atomic_Memory_barrier specifies the detailed regular
 * memory synchronization operations used in the atomic operation API 
 * definitions.  
 */
typedef enum {
  /** no operation orders memory. */
  ATOMIC_RELAXED_BARRIER,
  /** a load operation performs an acquire operation on the affected memory
  * location. This flag guarantees that the effects of load operation are 
  * completed before the effects of any later data accesses.
  */
  ATOMIC_ACQUIRE_BARRIER,
  /** a store operation performs a release operation on the affected memory
  * location. This flag guarantee that all effects of all previous data 
  * accesses are completed before the store operation takes place.
  */
  ATOMIC_RELEASE_BARRIER
}Atomic_Memory_barrier;

/**
 * @brief Atomically load an atomic type value from address @a address with
 * a type of Atomic_Memory_barrier @a memory_barrier. The @a memory_barrier
 * shall not be ATOMIC_RELEASE_BARRIER.
 */
RTEMS_INLINE_ROUTINE Atomic_Int _Atomic_Load_int(
  volatile Atomic_Int *address,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE Atomic_Long _Atomic_Load_long(
  volatile Atomic_Long *address,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE Atomic_Pointer _Atomic_Load_ptr(
  volatile Atomic_Pointer *address,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE Atomic_Int32 _Atomic_Load_32(
  volatile Atomic_Int32 *address,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE Atomic_Int64 _Atomic_Load_64(
  volatile Atomic_Int64 *address,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically store an atomic type value @a value into address @a 
 * address with a type of Atomic_Memory_barrier @a memory_barrier. The @a 
 * memory_barrier shall not be ATOMIC_ACQUIRE_BARRIER.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Store_int(
  volatile Atomic_Int *address,
  Atomic_Int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Store_long(
  volatile Atomic_Long *address,
  Atomic_Long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Store_ptr(
  volatile Atomic_Pointer *address,
  Atomic_Pointer value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Store_32(
  volatile Atomic_Int32 *address,
  Atomic_Int32 value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Store_64(
  volatile Atomic_Int64 *address,
  Atomic_Int64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-add-store an atomic type value @a value into address
 * @a address with a type of Atomic_Memory_barrier @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_int(
  volatile Atomic_Int *address,
  Atomic_Int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_long(
  volatile Atomic_Long *address,
  Atomic_Long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_ptr(
  volatile Atomic_Pointer *address,
  Atomic_Pointer value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_32(
  volatile Atomic_Int32 *address,
  Atomic_Int32 value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_64(
  volatile Atomic_Int64 *address,
  Atomic_Int64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-sub-store an atomic type value @a value into address
 * @a address with a type of Atomic_Memory_barrier @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_int(
  volatile Atomic_Int *address,
  Atomic_Int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_long(
  volatile Atomic_Long *address,
  Atomic_Long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_ptr(
  volatile Atomic_Pointer *address,
  Atomic_Pointer value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_32(
  volatile Atomic_Int32 *address,
  Atomic_Int32 value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_64(
  volatile Atomic_Int64 *address,
  Atomic_Int64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-or-store an atomic type value @a value into address
 * @a address with a type of Atomic_Memory_barrier @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_int(
  volatile Atomic_Int *address,
  Atomic_Int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_long(
  volatile Atomic_Long *address,
  Atomic_Long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_ptr(
  volatile Atomic_Pointer *address,
  Atomic_Pointer value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_32(
  volatile Atomic_Int32 *address,
  Atomic_Int32 value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_64(
  volatile Atomic_Int64 *address,
  Atomic_Int64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-and-store an atomic type value @a value into address
 * @a address with a type of Atomic_Memory_barrier @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_int(
  volatile Atomic_Int *address,
  Atomic_Int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_long(
  volatile Atomic_Long *address,
  Atomic_Long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_ptr(
  volatile Atomic_Pointer *address,
  Atomic_Pointer value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_32(
  volatile Atomic_Int32 *address,
  Atomic_Int32 value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_64(
  volatile Atomic_Int64 *address,
  Atomic_Int64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically compare the value stored at @a address with @a 
 * old_value and if the two values are equal, update the value of @a 
 * address with @a new_value. Returns zero if the compare failed, 
 * nonzero otherwise. The operation uses a type of Atomic_Memory_barrier
 * @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_int(
  volatile Atomic_Int *address,
  Atomic_Int old_value,
  Atomic_Int new_value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_long(
  volatile Atomic_Long *address,
  Atomic_Long old_value,
  Atomic_Long new_value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_ptr(
  volatile Atomic_Pointer *address,
  Atomic_Pointer old_value,
  Atomic_Pointer new_value,
  Atomic_Memory_barrier memory_barrier  
);
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_32(
  volatile Atomic_Int32 *address,
  Atomic_Int32 old_value,
  Atomic_Int32 new_value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_64(
  volatile Atomic_Int64 *address,
  Atomic_Int64 old_value,
  Atomic_Int64 new_value,
  Atomic_Memory_barrier memory_barrier
);

#include <rtems/score/atomic.inl>

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
