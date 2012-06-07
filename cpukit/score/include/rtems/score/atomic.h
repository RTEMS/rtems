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

#ifndef _RTEMS_ATOMIC_H
#define _RTEMS_ATOMIC_H

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
Atomic_int _Atomic_Load_int(
  volatile Atomic_int *address,
  Atomic_Memory_barrier memory_barrier
);
Atomic_long _Atomic_Load_long(
  volatile Atomic_long *address,
  Atomic_Memory_barrier memory_barrier
);
Atomic_ptr _Atomic_Load_ptr(
  volatile Atomic_ptr *address,
  Atomic_Memory_barrier memory_barrier
);
Atomic_32 _Atomic_Load_32(
  volatile Atomic_32 *address,
  Atomic_Memory_barrier memory_barrier
);
Atomic_64 _Atomic_Load_64(
  volatile Atomic_64 *address,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically store an atomic type value @a value into address @a 
 * address with a type of ATOMIC_memory_barrier @a memory_barrier. The @a 
 * memory_barrier shall not be ATOMIC_ACQUIRE_BARRIER.
 */
void _Atomic_Store_int(
  volatile Atomic_int *address,
  Atomic_int value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Store_long(
  volatile Atomic_long *address,
  Atomic_long value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Store_ptr(
  volatile Atomic_ptr *address,
  Atomic_ptr value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Store_32(
  volatile Atomic_32 *address,
  Atomic_32 value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Store_64(
  volatile Atomic_64 *address,
  Atomic_64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-add-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
void _Atomic_Fetch_add_int(
  volatile Atomic_int *address,
  Atomic_int value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_add_long(
  volatile Atomic_long *address,
  Atomic_long value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_add_ptr(
  volatile Atomic_ptr *address,
  Atomic_ptr value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_add_32(
  volatile Atomic_32 *address,
  Atomic_32 value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_add_64(
  volatile Atomic_64 *address,
  Atomic_64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-sub-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
void _Atomic_Fetch_sub_int(
  volatile Atomic_int *address,
  Atomic_int value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_sub_long(
  volatile Atomic_long *address,
  Atomic_long value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_sub_ptr(
  volatile Atomic_ptr *address,
  Atomic_ptr value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_sub_32(
  volatile Atomic_32 *address,
  Atomic_32 value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_sub_64(
  volatile Atomic_64 *address,
  Atomic_64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-or-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
void _Atomic_Fetch_or_int(
  volatile Atomic_int *address,
  Atomic_int value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_or_long(
  volatile Atomic_long *address,
  Atomic_long value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_or_ptr(
  volatile Atomic_ptr *address,
  Atomic_ptr value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_or_32(
  volatile Atomic_32 *address,
  Atomic_32 value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_or_64(
  volatile Atomic_64 *address,
  Atomic_64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically load-and-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
void _Atomic_Fetch_and_int(
  volatile Atomic_int *address,
  Atomic_int value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_and_long(
  volatile Atomic_long *address,
  Atomic_long value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_and_ptr(
  volatile Atomic_ptr *address,
  Atomic_ptr value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_and_32(
  volatile Atomic_32 *address,
  Atomic_32 value,
  Atomic_Memory_barrier memory_barrier
);
void _Atomic_Fetch_and_64(
  volatile Atomic_64 *address,
  Atomic_64 value,
  Atomic_Memory_barrier memory_barrier
);

/**
 * @brief Atomically compare the value stored at @a address with @a 
 * old_value and if the two values are equal, update the value of @a 
 * address with @a new_value. Returns zero if the compare failed, 
 * nonzero otherwise. The operation uses a type of ATOMIC_memory_barrier
 * @a memory_barrier.
 */
int _Atomic_Compare_exchange_int(
  volatile Atomic_int *address,
  Atomic_int old_value,
  Atomic_int new_value,
  Atomic_Memory_barrier memory_barrier
);
int _Atomic_Compare_exchange_long(
  volatile Atomic_long *address,
  Atomic_long old_value,
  Atomic_long new_value,
  Atomic_Memory_barrier memory_barrier
);
int _Atomic_Compare_exchange_ptr(
  volatile Atomic_ptr *address,
  Atomic_ptr old_value,
  Atomic_ptr new_value,
  Atomic_Memory_barrier memory_barrier  
);
int _Atomic_Compare_exchange_32(
  volatile Atomic_32 *address,
  Atomic_32 old_value,
  Atomic_32 new_value,
  Atomic_Memory_barrier memory_barrier
);
int _Atomic_Compare_exchange_64(
  volatile Atomic_64 *address,
  Atomic_64 old_value,
  Atomic_64 new_value,
  Atomic_Memory_barrier memory_barrier
);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
