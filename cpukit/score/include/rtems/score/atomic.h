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
 * @brief Atomically load an atomic type value from address @a address without
 * any memory barrier.
 */
ATOMIC_int _Atomic_Load_int(
  volatile ATOMIC_int *address
);
ATOMIC_long _Atomic_Load_long(
  volatile ATOMIC_long *address
);
ATOMIC_ptr _Atomic_Load_ptr(
  volatile ATOMIC_ptr *address
);
ATOMIC_32 _Atomic_Load_32(
  volatile ATOMIC_32 *address
);
ATOMIC_64 _Atomic_Load_64(
  volatile ATOMIC_64 *address
);

/**
 * @brief Atomically load an atomic type value from address @a address with 
 * a read memory barrier.
 *
 * Those routines guarantee that the effects of load operation are completed
 * before the effects of any later data accesses.
 */
ATOMIC_int _Atomic_Load_acq_int(
  volatile ATOMIC_int *address
);
ATOMIC_long _Atomic_Load_acq_long(
  volatile ATOMIC_long *address
);
ATOMIC_ptr _Atomic_Load_acq_ptr(
  volatile ATOMIC_ptr *address
);
ATOMIC_32 _Atomic_Load_acq_32(
  volatile ATOMIC_32 *address
);
ATOMIC_64 _Atomic_Load_acq_64(
  volatile ATOMIC_64 *address
);

/**
 * @brief Atomically store an atomic type value @a value into address @a 
 * address without any memory barrier.
 */
void _Atomic_Store_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Store_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Store_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Store_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Store_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically store an atomic type value @a value into address @a 
 * address with a write memory barrier.
 *
 * Those routines guarantee that all effects of all previous data accesses
 * are completed before the store operation takes place.
 */

void _Atomic_Store_rel_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Store_rel_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Store_rel_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Store_rel_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Store_rel_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-add-store an atomic type value @a value into address
 * @a address without any memory barrier.
 */
void _Atomic_Fetch_Add_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_Add_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_Add_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_Add_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_Add_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-add-store an atomic type value @a value into address
 * @a address with a read memory barrier.
 *
 * Those routines guarantee that the effects of this operation are completed
 * before the effects of any later data accesses.
 */
void _Atomic_Fetch_Add_acq_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_Add_acq_long(
  volatile ATOMIC_long *address, 
  ATOMIC_long value
);
void _Atomic_Fetch_Add_acq_ptr(
  volatile ATOMIC_ptr *address, 
  ATOMIC_ptr value
);
void _Atomic_Fetch_Add_acq_32(
  volatile ATOMIC_32 *address, 
  ATOMIC_32 value
);
void _Atomic_Fetch_Add_acq_64(
  volatile ATOMIC_64 *address, 
  ATOMIC_64 value
);

/**
 * @brief Atomically load-add-store an atomic type value @a value into address
 * @a address with a write memory barrier.
 *
 * Those routines guarantee that all effects of all previous data accesses
 * are completed before this operation takes place.
 */
void _Atomic_Fetch_Add_rel_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_Add_rel_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_Add_rel_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_Add_rel_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_Add_rel_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-sub-store an atomic type value @a value into address
 * @a address without any memory barrier.
 */
void _Atomic_Fetch_Sub_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_Sub_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_Sub_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_Sub_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_Sub_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-sub-store an atomic type value @a value into address
 * @a address with a read memory barrier.
 * 
 * Those routines guarantee that the effects of this operation are completed
 * before the effects of any later data accesses.
 */
void _Atomic_Fetch_Sub_acq_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_Sub_acq_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_Sub_acq_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_Sub_acq_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_Sub_acq_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-sub-store an atomic type value @a value into address
 * @a address with a write memory barrier.
 *
 * Those routines guarantee that all effects of all previous data accesses
 * are completed before this operation takes place.
 */
void _Atomic_Fetch_Sub_rel_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_Sub_rel_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_Sub_rel_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_Sub_rel_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_Sub_rel_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-or-store an atomic type value @a value into address
 * @a address without any memory barrier.
 */
void _Atomic_Fetch_Or_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_Or_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_Or_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_Or_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_Or_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-or-store an atomic type value @a value into address
 * @a address with a read memory barrier.
 * 
 * Those routines guarantee that the effects of this operation are completed
 * before the effects of any later data accesses.
 */
void _Atomic_Fetch_Or_acq_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_Or_acq_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_Or_acq_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_Or_acq_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_Or_acq_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-or-store an atomic type value @a value into address
 * @a address with a write memory barrier.
 *
 * Those routines guarantee that all effects of all previous data accesses
 * are completed before this operation takes place.
 */
void _Atomic_Fetch_Or_rel_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_Or_rel_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_Or_rel_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_Or_rel_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_Or_rel_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-and-store an atomic type value @a value into address
 * @a address without any memory barrier.
 */
void _Atomic_Fetch_And_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_And_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_And_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_And_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_And_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-and-store an atomic type value @a value into address
 * @a address with a read memory barrier.
 * 
 * Those routines guarantee that the effects of this operation are completed
 * before the effects of any later data accesses.
 */
void _Atomic_Fetch_And_acq_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_And_acq_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_And_acq_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_And_acq_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_And_acq_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically load-and-store an atomic type value @a value into address
 * @a address with a write memory barrier.
 *
 * Those routines guarantee that all effects of all previous data accesses
 * are completed before this operation takes place.
 */
void _Atomic_Fetch_And_rel_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value
);
void _Atomic_Fetch_And_rel_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value
);
void _Atomic_Fetch_And_rel_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value
);
void _Atomic_Fetch_And_rel_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value
);
void _Atomic_Fetch_And_rel_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value
);

/**
 * @brief Atomically compare the value stored at @a address with @a 
 * old_value and if the two values are equal, update the value of @a 
 * address with @a new_value. Returns zero if the compare failed, 
 * nonzero otherwise. The operation does not use any memory barrier.
 */
int _Atomic_Compare_Exchange_int(
  volatile ATOMIC_int *address,
  ATOMIC_int old_value,
  ATOMIC_int new_value
);
int _Atomic_Compare_Exchange_long(
  volatile ATOMIC_long *address,
  ATOMIC_long old_value,
  ATOMIC_long new_value
);
int _Atomic_Compare_Exchange_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr old_value,
  ATOMIC_ptr new_value
  
);
int _Atomic_Compare_Exchange_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 old_value,
  ATOMIC_32 new_value
);
int _Atomic_Compare_Exchange_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 old_value,
  ATOMIC_64 new_value
);

/**
 * @brief Atomically compare the value stored at @a address with @a 
 * old_value and if the two values are equal, update the value of @a 
 * address with @a new_value. Returns zero if the compare failed, 
 * nonzero otherwise. The operation uses a read memory barrier.
 * 
 * Those routines guarantee that the effects of this operation are completed
 * before the effects of any later data accesses.
 */
int _Atomic_Compare_Exchange_acq_int(
  volatile ATOMIC_int *address,
  ATOMIC_int old_value,
  ATOMIC_int new_value
);
int _Atomic_Compare_Exchange_acq_long(
  volatile ATOMIC_long *address,
  ATOMIC_long old_value,
  ATOMIC_long new_value
);
int _Atomic_Compare_Exchange_acq_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr old_value,
  ATOMIC_ptr new_value
);
int _Atomic_Compare_Exchange_acq_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 old_value,
  ATOMIC_32 new_value
);
int _Atomic_Compare_Exchange_acq_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 old_value,
  ATOMIC_64 new_value
);

/**
 * @brief Atomically compare the value stored at @a address with @a 
 * old_value and if the two values are equal, update the value of @a 
 * address with @a new_value. Returns zero if the compare failed, 
 * nonzero otherwise. The operation uses a write memory barrier.
 *
 * Those routines guarantee that all effects of all previous data accesses
 * are completed before this operation takes place.
 */
int _Atomic_Compare_Exchange_rel_int(
  volatile ATOMIC_int *address,
  ATOMIC_int old_value,
  ATOMIC_int new_value
);
int _Atomic_Compare_Exchange_rel_long(
  volatile ATOMIC_long *address,
  ATOMIC_long old_value,
  ATOMIC_long new_value
);
int _Atomic_Compare_Exchange_rel_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr old_value,
  ATOMIC_ptr new_value
);
int _Atomic_Compare_Exchange_rel_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 old_value,
  ATOMIC_32 new_value
);
int _Atomic_Compare_Exchange_rel_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 old_value,
  ATOMIC_64 new_value
);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
