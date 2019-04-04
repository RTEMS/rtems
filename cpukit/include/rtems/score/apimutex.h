/**
 * @file
 *
 * @ingroup RTEMSScoreAPIMutex
 *
 * @brief API Mutex Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
 * @brief Provides routines to ensure mutual exclusion on API level.
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
