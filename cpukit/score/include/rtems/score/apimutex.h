/**
 * @file
 *
 * @ingroup ScoreAPIMutex
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

#include <rtems/score/coremutex.h>
#include <rtems/score/object.h>

/**
 * @defgroup ScoreAPIMutex API Mutex Handler
 *
 * @ingroup Score
 *
 * @brief Provides routines to ensure mutual exclusion on API level.
 */
/**@{**/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Control block used to manage each API mutex.
 */
typedef struct {
  /**
   * @brief Allows each API Mutex to be a full-fledged RTEMS object.
   */
  Objects_Control Object;

  /**
   * Contains the SuperCore mutex information.
   */
  CORE_mutex_Control Mutex;

  /**
   * @brief The thread life protection state before the outer-most mutex
   * obtain.
   */
  bool previous_thread_life_protection;
} API_Mutex_Control;

/**
 *  @brief Initialization for the API Mutexe Handler.
 *
 *  The value @a maximum_mutexes is the maximum number of API mutexes that may
 *  exist at any time.
 *
 *  @param[in] maximum_mutexes is the maximum number of API mutexes.
 */
void _API_Mutex_Initialization( uint32_t maximum_mutexes );

/**
 * @brief Allocates an API mutex from the inactive set and returns it in
 * @a mutex.
 */
void _API_Mutex_Allocate( API_Mutex_Control **mutex );

/**
 * @brief Acquires the specified API mutex.
 *
 * @param[in] mutex The API mutex.
 */
void _API_Mutex_Lock( API_Mutex_Control *mutex );

/**
 * @brief Releases the specified API mutex.
 *
 * @param[in] mutex The API mutex.
 */
void _API_Mutex_Unlock( API_Mutex_Control *mutex );

/**
 * @brief Checks if the specified API mutex is owned by the executing thread.
 *
 * @param[in] mutex The API mutex.
 */
bool _API_Mutex_Is_owner( const API_Mutex_Control *mutex );

/** @} */

/**
 * @defgroup ScoreAllocatorMutex RTEMS Allocator Mutex
 *
 * @ingroup ScoreAPIMutex
 *
 * @brief Protection for all memory allocations and deallocations in RTEMS.
 *
 * When the APIs all use this for allocation and deallocation protection, then
 * this possibly should be renamed and moved to a higher level in the
 * hierarchy.
 */
/**@{**/

/**
 *  @brief Memory allocation mutex.
 *
 *  This points to the API Mutex instance used to ensure that only
 *  one thread at a time is allocating or freeing memory.
 */
SCORE_EXTERN API_Mutex_Control *_RTEMS_Allocator_Mutex;

static inline void _RTEMS_Lock_allocator( void )
{
  _API_Mutex_Lock( _RTEMS_Allocator_Mutex );
}

static inline void _RTEMS_Unlock_allocator( void )
{
  _API_Mutex_Unlock( _RTEMS_Allocator_Mutex );
}

static inline bool _RTEMS_Allocator_is_owner( void )
{
  return _API_Mutex_Is_owner( _RTEMS_Allocator_Mutex );
}

SCORE_EXTERN API_Mutex_Control *_Once_Mutex;

static inline void _Once_Lock( void )
{
  _API_Mutex_Lock( _Once_Mutex );
}

static inline void _Once_Unlock( void )
{
  _API_Mutex_Unlock( _Once_Mutex );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
