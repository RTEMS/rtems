/**
 * @file
 *
 * @ingroup ScoreAPIMutex
 *
 * @brief API Mutex Handler API.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_APIMUTEX_H
#define _RTEMS_SCORE_APIMUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreAPIMutex API Mutex Handler
 *
 * @ingroup Score
 *
 * @brief Provides routines to ensure mutual exclusion on API level.
 *
 * @{
 */

#include <rtems/score/coremutex.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>

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
} API_Mutex_Control;

/**
 * @brief Information control block used to manage this class of objects.
 */
SCORE_EXTERN Objects_Information _API_Mutex_Information;

/**
 * @brief Performs the initialization necessary for this handler.
 *
 * The value @a maximum_mutexes is the maximum number of API mutexes that may
 * exist at any time.
 */
void _API_Mutex_Initialization( uint32_t maximum_mutexes );

/**
 * @brief Allocates an API mutex from the inactive set and returns it in
 * @a mutex.
 */
void _API_Mutex_Allocate( API_Mutex_Control **mutex );

/**
 * @brief Acquires the specified API mutex @a mutex.
 */
void _API_Mutex_Lock( API_Mutex_Control *mutex );

/**
 * @brief Releases the specified API mutex @a mutex.
 */
void _API_Mutex_Unlock( API_Mutex_Control *mutex );

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
 *
 * @{
 */

/**
 *  @brief Memory Allocation Mutex
 *
 *  This points to the API Mutex instance used to ensure that only
 *  one thread at a time is allocating or freeing memory.
 */
SCORE_EXTERN API_Mutex_Control *_RTEMS_Allocator_Mutex;

/**
 *  @brief Macro to Ease Locking the Allocator Mutex
 *
 *  This macro makes it explicit that one is locking the allocator mutex.
 */
#define _RTEMS_Lock_allocator() \
  _API_Mutex_Lock( _RTEMS_Allocator_Mutex )

/**
 *  @brief Macro to Ease Unlocking the Allocator Mutex
 *
 *  This macro makes it explicit that one is unlocking the allocator mutex.
 */
#define _RTEMS_Unlock_allocator() \
  _API_Mutex_Unlock( _RTEMS_Allocator_Mutex )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
