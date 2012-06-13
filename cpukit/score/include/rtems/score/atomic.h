/**
 * @file  rtems/score/atomic.h
 * 
 * This include file defines the interface for all the atomic
 * operations which can be used in the synchronization primitives
 * or in the lock-less algorithms. You should not use these API
 * in the other components directly. 
 */

/*
 * COPYRIGHT (c) 1989-2012.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ATOMIC_H
#define _RTEMS_SCORE_ATOMIC_H

#include <rtems/score/atomic_cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS atomic interface
 *
 */

/**@{*/

/**
 * @brief the enumeration ATOMIC_memory_barrier specifies the detailed regular
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
 * a type of ATOMIC_memory_barrier @a memory_barrier. The @a memory_barrier
 * shall not be ATOMIC_RELEASE_BARRIER.
 */
RTEMS_INLINE_ROUTINE u_int _Atomic_Load_int(
  volatile u_int *address,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE u_long _Atomic_Load_long(
  volatile u_long *address,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE uintptr_t _Atomic_Load_ptr(
  volatile uintptr_t *address,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE uint32_t _Atomic_Load_32(
  volatile uint32_t *address,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE uint64_t _Atomic_Load_64(
  volatile uint64_t *address,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically store an atomic type value @a value into address @a 
 * address with a type of ATOMIC_memory_barrier @a memory_barrier. The @a 
 * memory_barrier shall not be ATOMIC_ACQUIRE_BARRIER.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Store_int(
  volatile u_int *address,
  u_int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Store_long(
  volatile u_long *address,
  u_long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Store_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Store_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Store_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-add-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_int(
  volatile u_int *address,
  u_int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_long(
  volatile u_long *address,
  u_long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-sub-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_int(
  volatile u_int *address,
  u_int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_long(
  volatile u_long *address,
  u_long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-or-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_int(
  volatile u_int *address,
  u_int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_long(
  volatile u_long *address,
  u_long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-and-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_int(
  volatile u_int *address,
  u_int value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_long(
  volatile u_long *address,
  u_long value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically compare the value stored at @a address with @a 
 * old_value and if the two values are equal, update the value of @a 
 * address with @a new_value. Returns zero if the compare failed, 
 * nonzero otherwise. The operation uses a type of ATOMIC_memory_barrier
 * @a memory_barrier.
 */
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_int(
  volatile u_int *address,
  u_int old_value,
  u_int new_value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_long(
  volatile u_long *address,
  u_long old_value,
  u_long new_value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_ptr(
  volatile uintptr_t *address,
  uintptr_t old_value,
  uintptr_t new_value,
  Atomic_Memory_barrier memory_barrier  
);
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_32(
  volatile uint32_t *address,
  uint32_t old_value,
  uint32_t new_value,
  Atomic_Memory_barrier memory_barrier
);
RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_64(
  volatile uint64_t *address,
  uint64_t old_value,
  uint64_t new_value,
  Atomic_Memory_barrier memory_barrier
);

#include <rtems/score/atomic.inl>

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
