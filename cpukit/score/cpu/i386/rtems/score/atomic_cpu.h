/**
 * @file  rtems/score/atomic_cpu.h
 * 
 * This include file implements the atomic operations for i386 and defines 
 * atomic date types which are used by the atomic operations API file. This
 * file should use fixed name atomic_cpu.h and should be included in atomic
 * operations API file atomic.h 
 */

/*
 * COPYRIGHT (c) 1989-2012.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ATOMIC_CPU_H
#define _RTEMS_SCORE_ATOMIC_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS atomic implementation
 *
 */

/**@{*/

/**
 * @brief atomic operation unsigned integer type
 */
typedef unsigned int Atomic_int;

/**
 * @brief atomic operation unsigned long integer type
 */
typedef unsigned long Atomic_long;

/**
 * @brief atomic operation unsigned 32-bit integer type
 */
typedef uint32_t Atomic_32;

/**
 * @brief atomic operation unsigned 64-bit integer type
 */
typedef uint64_t Atomic_64;

/**
 * @brief atomic operation unsigned integer the size of a pointer type
 */
typedef uintptr_t Atomic_ptr;

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
