/**
 * @file  rtems/score/genericatomicops.h
 *
 * This include file includes the general atomic functions
 * for all the uniprocessors or working in UP mode architectures.
 * If the architecture is working at the UP mode, the atomic
 * operations will be simulated by disable/enable-irq functions.
 * Supposing that if the macro "RTEMS_SMP" is defined it works
 * in SMP mode, otherwise UP mode.
 */

/*
 * COPYRIGHT (c) 2013 Deng Hengyi.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 */

#ifndef _RTEMS_SCORE_GENERAL_ATOMIC_OPS_H
#define _RTEMS_SCORE_GEMERAL_ATOMIC_OPS_H

#include <rtems/score/genericcpuatomic.h>
#include <rtems/score/isr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS generic UP atomic implementation
 *
 */

/**@{*/

/**
 * @brief Atomically load an atomic type value from address @a address.
 */
#define ATOMIC_LOAD(NAME, TYPE)                                      \
RTEMS_INLINE_ROUTINE Atomic_##TYPE _CPU_Atomic_Load_##NAME(	     \
  volatile Atomic_##TYPE *address                                    \
)                                                                    \
{                                                                    \
  Atomic_##TYPE  tmp;                                                \
  ISR_Level   level;                                                 \
                                                                     \
  _ISR_Disable( level );                                             \
  tmp = *address;                                                    \
  _ISR_Enable( level );                                              \
  return tmp;                                                        \
}                                                                    \

ATOMIC_LOAD(int, Int);
ATOMIC_LOAD(acq_int, Int);
ATOMIC_LOAD(long, Long);
ATOMIC_LOAD(acq_long, Long);
ATOMIC_LOAD(ptr, Pointer);
ATOMIC_LOAD(acq_ptr, Pointer);
ATOMIC_LOAD(32, Int32);
ATOMIC_LOAD(acq_32, Int32);
ATOMIC_LOAD(64, Int64);
ATOMIC_LOAD(acq_64, Int64);

/**
 * @brief Atomically store an atomic type value @a value into address @a
 * address.
 */
#define ATOMIC_STORE(NAME, TYPE)                                     \
RTEMS_INLINE_ROUTINE void _CPU_Atomic_Store_##NAME(	             \
  volatile Atomic_##TYPE *address,                                   \
  Atomic_##TYPE value                                                \
)                                                                    \
{                                                                    \
  ISR_Level   level;                                                 \
                                                                     \
  _ISR_Disable( level );                                             \
  *address = value;                                                  \
  _ISR_Enable( level );                                              \
}                                                                    \

ATOMIC_STORE(int, Int);
ATOMIC_STORE(rel_int, Int);
ATOMIC_STORE(long, Long);
ATOMIC_STORE(rel_long, Long);
ATOMIC_STORE(ptr, Pointer);
ATOMIC_STORE(rel_ptr, Pointer);
ATOMIC_STORE(32, Int32);
ATOMIC_STORE(rel_32, Int32);
ATOMIC_STORE(64, Int64);
ATOMIC_STORE(rel_64, Int64);

/**
 * @brief Atomically load-add-store an atomic type value @a value into address
 * @a address.
 */
#define ATOMIC_FETCH_ADD(NAME, TYPE)                                 \
RTEMS_INLINE_ROUTINE void _CPU_Atomic_Fetch_add_##NAME(	             \
  volatile Atomic_##TYPE *address,                                   \
  Atomic_##TYPE value                                                \
)                                                                    \
{                                                                    \
  ISR_Level   level;                                                 \
                                                                     \
  _ISR_Disable( level );                                             \
  *address += value;                                                 \
  _ISR_Enable( level );                                              \
}                                                                    \

ATOMIC_FETCH_ADD(int, Int);
ATOMIC_FETCH_ADD(acq_int, Int);
ATOMIC_FETCH_ADD(rel_int, Int);
ATOMIC_FETCH_ADD(long, Long);
ATOMIC_FETCH_ADD(acq_long, Long);
ATOMIC_FETCH_ADD(rel_long, Long);
ATOMIC_FETCH_ADD(ptr, Pointer);
ATOMIC_FETCH_ADD(acq_ptr, Pointer);
ATOMIC_FETCH_ADD(rel_ptr, Pointer);
ATOMIC_FETCH_ADD(32, Int32);
ATOMIC_FETCH_ADD(acq_32, Int32);
ATOMIC_FETCH_ADD(rel_32, Int32);
ATOMIC_FETCH_ADD(64, Int64);
ATOMIC_FETCH_ADD(acq_64, Int64);
ATOMIC_FETCH_ADD(rel_64, Int64);

/**
 * @brief Atomically load-sub-store an atomic type value @a value into address
 * @a address.
 */
#define ATOMIC_FETCH_SUB(NAME, TYPE)                                 \
RTEMS_INLINE_ROUTINE void _CPU_Atomic_Fetch_sub_##NAME(	             \
  volatile Atomic_##TYPE *address,                                   \
  Atomic_##TYPE value                                                \
)                                                                    \
{                                                                    \
  ISR_Level   level;                                                 \
                                                                     \
  _ISR_Disable( level );                                             \
  *address -= value;                                                 \
  _ISR_Enable( level );                                              \
}                                                                    \

ATOMIC_FETCH_SUB(int, Int);
ATOMIC_FETCH_SUB(acq_int, Int);
ATOMIC_FETCH_SUB(rel_int, Int);
ATOMIC_FETCH_SUB(long, Long);
ATOMIC_FETCH_SUB(acq_long, Long);
ATOMIC_FETCH_SUB(rel_long, Long);
ATOMIC_FETCH_SUB(ptr, Pointer);
ATOMIC_FETCH_SUB(acq_ptr, Pointer);
ATOMIC_FETCH_SUB(rel_ptr, Pointer);
ATOMIC_FETCH_SUB(32, Int32);
ATOMIC_FETCH_SUB(acq_32, Int32);
ATOMIC_FETCH_SUB(rel_32, Int32);
ATOMIC_FETCH_SUB(64, Int64);
ATOMIC_FETCH_SUB(acq_64, Int64);
ATOMIC_FETCH_SUB(rel_64, Int64);

/**
 * @brief Atomically load-or-store an atomic type value @a value into address
 * @a address.
 */
#define ATOMIC_FETCH_OR(NAME, TYPE)                                  \
RTEMS_INLINE_ROUTINE void _CPU_Atomic_Fetch_or_##NAME(	             \
  volatile Atomic_##TYPE *address,                                   \
  Atomic_##TYPE value                                                \
)                                                                    \
{                                                                    \
  ISR_Level   level;                                                 \
                                                                     \
  _ISR_Disable( level );                                             \
  *address |= value;                                                 \
  _ISR_Enable( level );                                              \
}                                                                    \

ATOMIC_FETCH_OR(int, Int);
ATOMIC_FETCH_OR(acq_int, Int);
ATOMIC_FETCH_OR(rel_int, Int);
ATOMIC_FETCH_OR(long, Long);
ATOMIC_FETCH_OR(acq_long, Long);
ATOMIC_FETCH_OR(rel_long, Long);
ATOMIC_FETCH_OR(ptr, Pointer);
ATOMIC_FETCH_OR(acq_ptr, Pointer);
ATOMIC_FETCH_OR(rel_ptr, Pointer);
ATOMIC_FETCH_OR(32, Int32);
ATOMIC_FETCH_OR(acq_32, Int32);
ATOMIC_FETCH_OR(rel_32, Int32);
ATOMIC_FETCH_OR(64, Int64);
ATOMIC_FETCH_OR(acq_64, Int64);
ATOMIC_FETCH_OR(rel_64, Int64);

/**
 * @brief Atomically load-and-store an atomic type value @a value into address
 * @a address.
 */
#define ATOMIC_FETCH_AND(NAME, TYPE)                                 \
RTEMS_INLINE_ROUTINE void _CPU_Atomic_Fetch_and_##NAME(	             \
  volatile Atomic_##TYPE *address,                                   \
  Atomic_##TYPE value                                                \
)                                                                    \
{                                                                    \
  ISR_Level   level;                                                 \
                                                                     \
  _ISR_Disable( level );                                             \
  *address &= value;                                                 \
  _ISR_Enable( level );                                              \
}                                                                    \

ATOMIC_FETCH_AND(int, Int);
ATOMIC_FETCH_AND(acq_int, Int);
ATOMIC_FETCH_AND(rel_int, Int);
ATOMIC_FETCH_AND(long, Long);
ATOMIC_FETCH_AND(acq_long, Long);
ATOMIC_FETCH_AND(rel_long, Long);
ATOMIC_FETCH_AND(ptr, Pointer);
ATOMIC_FETCH_AND(acq_ptr, Pointer);
ATOMIC_FETCH_AND(rel_ptr, Pointer);
ATOMIC_FETCH_AND(32, Int32);
ATOMIC_FETCH_AND(acq_32, Int32);
ATOMIC_FETCH_AND(rel_32, Int32);
ATOMIC_FETCH_AND(64, Int64);
ATOMIC_FETCH_AND(acq_64, Int64);
ATOMIC_FETCH_AND(rel_64, Int64);

/**
 * @brief Atomically compare the value stored at @a address with @a
 * old_value and if the two values are equal, update the value of @a
 * address with @a new_value. Returns zero if the compare failed,
 * nonzero otherwise.
 */
#define ATOMIC_COMPARE_EXCHANGE(NAME, TYPE)                          \
RTEMS_INLINE_ROUTINE int _CPU_Atomic_Compare_exchange_##NAME(        \
  volatile Atomic_##TYPE *address,                                   \
  Atomic_##TYPE old_value,                                           \
  Atomic_##TYPE new_value                                            \
)                                                                    \
{                                                                    \
  ISR_Level   level;                                                 \
  int ret;                                                           \
                                                                     \
  _ISR_Disable( level );                                             \
  if (*address == old_value) {                                       \
    *address = new_value;                                            \
    ret = TRUE;                                                      \
  } else {                                                           \
    ret = FALSE;                                                     \
  }                                                                  \
  _ISR_Enable( level );                                              \
                                                                     \
  return ret;                                                        \
}                                                                    \

ATOMIC_COMPARE_EXCHANGE(int, Int);
ATOMIC_COMPARE_EXCHANGE(acq_int, Int);
ATOMIC_COMPARE_EXCHANGE(rel_int, Int);
ATOMIC_COMPARE_EXCHANGE(long, Long);
ATOMIC_COMPARE_EXCHANGE(acq_long, Long);
ATOMIC_COMPARE_EXCHANGE(rel_long, Long);
ATOMIC_COMPARE_EXCHANGE(ptr, Pointer);
ATOMIC_COMPARE_EXCHANGE(acq_ptr, Pointer);
ATOMIC_COMPARE_EXCHANGE(rel_ptr, Pointer);
ATOMIC_COMPARE_EXCHANGE(32, Int32);
ATOMIC_COMPARE_EXCHANGE(acq_32, Int32);
ATOMIC_COMPARE_EXCHANGE(rel_32, Int32);
ATOMIC_COMPARE_EXCHANGE(64, Int64);
ATOMIC_COMPARE_EXCHANGE(acq_64, Int64);
ATOMIC_COMPARE_EXCHANGE(rel_64, Int64);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
