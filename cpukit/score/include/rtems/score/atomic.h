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
}ATOMIC_memory_barrier;

/**
 * @brief Atomically load an atomic type value from address @a address with
 * a type of ATOMIC_memory_barrier @a memory_barrier. The @a memory_barrier
 * shall not be ATOMIC_RELEASE_BARRIER.
 */
ATOMIC_int _Atomic_Load_int(
  volatile ATOMIC_int *address,
  ATOMIC_memory_barrier memory_barrier
);
ATOMIC_long _Atomic_Load_long(
  volatile ATOMIC_long *address,
  ATOMIC_memory_barrier memory_barrier
);
ATOMIC_ptr _Atomic_Load_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_memory_barrier memory_barrier
);
ATOMIC_32 _Atomic_Load_32(
  volatile ATOMIC_32 *address,
  ATOMIC_memory_barrier memory_barrier
);
ATOMIC_64 _Atomic_Load_64(
  volatile ATOMIC_64 *address,
  ATOMIC_memory_barrier memory_barrier
);

/**
 * @brief Atomically store an atomic type value @a value into address @a 
 * address with a type of ATOMIC_memory_barrier @a memory_barrier. The @a 
 * memory_barrier shall not be ATOMIC_ACQUIRE_BARRIER.
 */
void _Atomic_Store_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Store_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Store_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Store_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Store_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
);

/**
 * @brief Atomically load-add-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
void _Atomic_Fetch_Add_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Add_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Add_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Add_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Add_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
);

/**
 * @brief Atomically load-sub-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
void _Atomic_Fetch_Sub_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Sub_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Sub_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Sub_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Sub_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
);

/**
 * @brief Atomically load-or-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
void _Atomic_Fetch_Or_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Or_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Or_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Or_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_Or_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
);

/**
 * @brief Atomically load-and-store an atomic type value @a value into address
 * @a address with a type of ATOMIC_memory_barrier @a memory_barrier.
 */
void _Atomic_Fetch_And_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_And_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_And_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_And_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
);
void _Atomic_Fetch_And_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
);

/**
 * @brief Atomically compare the value stored at @a address with @a 
 * old_value and if the two values are equal, update the value of @a 
 * address with @a new_value. Returns zero if the compare failed, 
 * nonzero otherwise. The operation uses a type of ATOMIC_memory_barrier
 * @a memory_barrier.
 */
int _Atomic_Compare_Exchange_int(
  volatile ATOMIC_int *address,
  ATOMIC_int old_value,
  ATOMIC_int new_value,
  ATOMIC_memory_barrier memory_barrier
);
int _Atomic_Compare_Exchange_long(
  volatile ATOMIC_long *address,
  ATOMIC_long old_value,
  ATOMIC_long new_value,
  ATOMIC_memory_barrier memory_barrier
);
int _Atomic_Compare_Exchange_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr old_value,
  ATOMIC_ptr new_value,
  ATOMIC_memory_barrier memory_barrier  
);
int _Atomic_Compare_Exchange_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 old_value,
  ATOMIC_32 new_value,
  ATOMIC_memory_barrier memory_barrier
);
int _Atomic_Compare_Exchange_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 old_value,
  ATOMIC_64 new_value,
  ATOMIC_memory_barrier memory_barrier
);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
