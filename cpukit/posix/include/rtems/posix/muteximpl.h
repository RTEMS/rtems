/**
 * @file
 *
 * @brief Private Inlined Routines for POSIX Mutex's.
 * 
 * This include file contains the static inline implementation of the private 
 * inlined routines for POSIX mutex's.
 */

/*  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/posix/mutex.h>
#include <rtems/score/coremuteximpl.h>

#include <errno.h>

#ifndef _RTEMS_POSIX_MUTEXIMPL_H
#define _RTEMS_POSIX_MUTEXIMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Mutex_Information;

/**
 *  The default mutex attributes structure.
 */
POSIX_EXTERN pthread_mutexattr_t _POSIX_Mutex_Default_attributes;

/**
 *  This array contains a mapping from Score Mutex return codes to
 *  POSIX return codes.
 */
extern const int _POSIX_Mutex_Return_codes[CORE_MUTEX_STATUS_LAST + 1];

/**
 *  @brief POSIX Mutex Manager Initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _POSIX_Mutex_Manager_initialization(void);

/**
 *  @brief POSIX Mutex Allocate
 *
 *  This function allocates a mutexes control block from
 *  the inactive chain of free mutexes control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Allocate( void )
{
  return (POSIX_Mutex_Control *) _Objects_Allocate( &_POSIX_Mutex_Information );
}

/**
 *  @brief POSIX Mutex Free
 *
 *  This routine frees a mutexes control block to the
 *  inactive chain of free mutexes control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Mutex_Free(
  POSIX_Mutex_Control *the_mutex
)
{
  _CORE_mutex_Destroy( &the_mutex->Mutex );
  _Objects_Free( &_POSIX_Mutex_Information, &the_mutex->Object );
}


/**
 *  @brief POSIX Mutex Lock Support Method
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of mutex lock.
 */
int _POSIX_Mutex_Lock_support(
  pthread_mutex_t           *mutex,
  bool                       blocking,
  Watchdog_Interval          timeout
);

/**
 * @brief Convert Score mutex status codes into POSIX status values
 *
 * A support routine which converts core mutex status codes into the
 * appropriate POSIX status values.
 *
 * @param[in] the_mutex_status is the mutex status code to translate
 *
 * @retval 0 Mutex status code indicates the operation completed successfully.
 * @retval EBUSY Mutex status code indicates that the operation unable to 
 *         complete immediately because the resource was unavailable.
 * @retval EDEADLK Mutex status code indicates that an attempt was made to
 *         relock a mutex for which nesting is not configured.
 * @retval EPERM Mutex status code indicates that an attempt was made to 
 *         release a mutex by a thread other than the thread which locked it.
 * @retval EINVAL Mutex status code indicates that the thread was blocked
 *         waiting for an operation to complete and the mutex was deleted.
 * @retval ETIMEDOUT Mutex status code indicates that the calling task was
 *         willing to block but the operation was unable to complete
 *         within the time allotted because the resource never became
 *         available.
 */
RTEMS_INLINE_ROUTINE int _POSIX_Mutex_Translate_core_mutex_return_code(
  CORE_mutex_Status  the_mutex_status
)
{
  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( the_mutex_status > CORE_MUTEX_STATUS_LAST )
      return EINVAL;
  #endif
  return _POSIX_Mutex_Return_codes[the_mutex_status];
}

/**
 *  @brief POSIX Mutex Get (Thread Dispatch Disable)
 *
 *  A support routine which translates the mutex id into a local pointer.
 *  As a side-effect, it may create the mutex.
 *
 *  @note This version of the method uses a dispatching critical section.
 */
POSIX_Mutex_Control *_POSIX_Mutex_Get (
  pthread_mutex_t   *mutex,
  Objects_Locations *location
);

/**
 *  @brief POSIX Mutex Get (Interrupt Disable)
 *
 *  A support routine which translates the mutex id into a local pointer.
 *  As a side-effect, it may create the mutex.
 *
 *  @note: This version of the method uses an interrupt critical section.
 */
POSIX_Mutex_Control *_POSIX_Mutex_Get_interrupt_disable (
  pthread_mutex_t   *mutex,
  Objects_Locations *location,
  ISR_lock_Context  *lock_context
);

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */

