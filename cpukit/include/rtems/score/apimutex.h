/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreAPIMutex
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreAPIMutex.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_APIMUTEX_H
#define _RTEMS_SCORE_APIMUTEX_H

#include <rtems/score/thread.h>

#include <sys/lock.h>

/**
 * @defgroup RTEMSScoreAPIMutex API Mutex Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the API Mutex Handler implementation.
 *
 * This handler provides routines to ensure mutual exclusion in a thread
 * context at the API level.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Control block used to manage each API mutex.
 */
typedef struct {
  /**
   * A recursive mutex.
   */
  struct _Mutex_recursive_Control Mutex;

  /**
   * @brief The thread life protection state before the outer-most mutex
   * obtain.
   */
  Thread_Life_state previous_thread_life_state;
} API_Mutex_Control;

/**
 * @brief Statically initialize an API mutex.
 */
#define API_MUTEX_INITIALIZER( name ) \
  { _MUTEX_RECURSIVE_NAMED_INITIALIZER( name ), 0 }

/**
 * @brief Acquires the specified API mutex.
 *
 * @param[in, out] mutex The API mutex to acquire.
 */
void _API_Mutex_Lock( API_Mutex_Control *mutex );

/**
 * @brief Releases the specified API mutex.
 *
 * @param[in, out] mutex The API mutex to release.
 */
void _API_Mutex_Unlock( API_Mutex_Control *mutex );

/**
 * @brief Checks if the specified API mutex is owned by the executing thread.
 *
 * @param[in] mutex The API mutex to check the owner from.
 */
bool _API_Mutex_Is_owner( const API_Mutex_Control *mutex );

/** @} */

/**
 * @defgroup RTEMSScoreAllocatorMutex RTEMS Allocator Mutex
 *
 * @ingroup RTEMSScoreAPIMutex
 *
 * @brief Protection for all memory allocations and deallocations in RTEMS.
 *
 * When the APIs all use this for allocation and deallocation protection, then
 * this possibly should be renamed and moved to a higher level in the
 * hierarchy.
 *
 * @{
 */

void _RTEMS_Lock_allocator( void );

void _RTEMS_Unlock_allocator( void );

bool _RTEMS_Allocator_is_owner( void );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
