/**
 * @file  rtems/score/genericcpuatomic.h
 * 
 * This include file includes the general atomic data type
 * for all the architecture. 
 */

/*
 * COPYRIGHT (c) 2012 Deng Hengyi.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 */

#ifndef _RTEMS_SCORE_GENERAL_ATOMIC_CPU_H
#define _RTEMS_SCORE_GEMERAL_ATOMIC_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS general atomic data type
 *
 */

/**@{*/

/**
 * @brief atomic operation unsigned integer type
 */
typedef unsigned int Atomic_Int;

/**
 * @brief atomic operation unsigned long integer type
 */
typedef unsigned long Atomic_Long;

/**
 * @brief atomic operation unsigned 32-bit integer type
 */
typedef uint32_t Atomic_Int32;

/**
 * @brief atomic operation unsigned 64-bit integer type
 */
typedef uint64_t Atomic_Int64;

/**
 * @brief atomic operation unsigned integer the size of a pointer type
 */
typedef uintptr_t Atomic_Pointer;

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
