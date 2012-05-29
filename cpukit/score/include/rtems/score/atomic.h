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
 * @brief Atomically load an unsigned integer from address @a p
 */
#define _Atomic_Load_acq_int(p)  \
        _CPU_Atomic_Load_acq_int((volatile ATOMIC_int *)(p))

/**
 * @brief Atomically load an unsigned long integer from address @a p
 */
#define _Atomic_Load_acq_long(p)  \
        _CPU_Atomic_Load_acq_long((volatile ATOMIC_long *)(p))

/**
 * @brief Atomically load an unsigned integer pointer from address @a p
 */
#define _Atomic_Load_acq_ptr(p)  \
        _CPU_Atomic_Load_acq_ptr((volatile ATOMIC_ptr *)(p))

/**
 * @brief Atomically load an unsigned 32-bit integer from address @a p
 */
#define _Atomic_Load_acq_32(p)  \
        _CPU_Atomic_Load_acq_32((volatile ATOMIC_32 *)(p)) 

/**
 * @brief Atomically load an unsigned 64-bit integer from address @a p
 */
#define _Atomic_Load_acq_64(p)  \
        _CPU_Atomic_Load_acq_64((volatile ATOMIC_64 *)(p))

/**
 * @brief Atomically store an unsigned integer @a v into address @a p
 */
#define _Atomic_Store_rel_int(p, v)  \
        _CPU_Atomic_Store_rel_int((volatile ATOMIC_int *)(p), (ATOMIC_int)(v))

/**
 * @brief Atomically store an unsigned long integer @a v into address @a p
 */
#define _Atomic_Store_rel_long(p, v)  \
     _CPU_Atomic_Store_rel_long((volatile ATOMIC_long *)(p), (ATOMIC_long)(v))

/**
 * @brief Atomically store an unsigned integer pointer @a v into address @a p
 */
#define _Atomic_Store_rel_ptr(p, v)  \
        _CPU_Atomic_Store_rel_ptr((volatile ATOMIC_ptr *)(p), (ATOMIC_ptr)(v))

/**
 * @brief Atomically store an unsigned 32-bit integer @a v into address @a p
 */
#define _Atomic_Store_rel_32(p, v)  \
        _CPU_Atomic_Store_rel_32((volatile ATOMIC_32 *)(p), (ATOMIC_32)(v)) 

/**
 * @brief Atomically store an unsigned 64-bit integer @a v into address @a p
 */
#define _Atomic_Store_rel_64(p, v)  \
        _CPU_Atomic_Store_rel_64((volatile ATOMIC_64 *)(p), (ATOMIC_64)(v)) 

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
