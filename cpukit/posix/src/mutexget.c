/*
 *  Convert POSIX Mutex ID to local object pointer
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/coremutex.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/posix/mutex.h>


/*
 *  _POSIX_Mutex_Get_support
 *
 *  NOTE: The support macro makes it possible for both to use exactly
 *        the same code to check for NULL id pointer and
 *        PTHREAD_MUTEX_INITIALIZER without adding overhead.
 */

#define ___POSIX_Mutex_Get_support_error_check( _id, _location ) \
  do { \
    if ( !_id ) { \
      *_location = OBJECTS_ERROR; \
      return (POSIX_Mutex_Control *) 0; \
    }  \
  } while (0)

#define ___POSIX_Mutex_Get_support_auto_initialization( _id, _location ) \
  do { \
    int _status; \
    \
    if ( *_id == PTHREAD_MUTEX_INITIALIZER ) { \
      /* \
       *  Do an "auto-create" here. \
       */ \
      \
      _status = pthread_mutex_init( (pthread_mutex_t *)_id, 0 ); \
      if ( _status ) { \
        *_location = OBJECTS_ERROR;  \
        return (POSIX_Mutex_Control *) 0; \
      } \
    } \
  } while (0)

POSIX_Mutex_Control *_POSIX_Mutex_Get (
  pthread_mutex_t   *mutex,
  Objects_Locations *location
)
{
  ___POSIX_Mutex_Get_support_error_check( mutex, location );

  ___POSIX_Mutex_Get_support_auto_initialization( mutex, location );

  return (POSIX_Mutex_Control *)
    _Objects_Get( &_POSIX_Mutex_Information, (Objects_Id) *mutex, location );
}

POSIX_Mutex_Control *_POSIX_Mutex_Get_interrupt_disable (
  pthread_mutex_t   *mutex,
  Objects_Locations *location,
  ISR_Level         *level
)
{
  ___POSIX_Mutex_Get_support_error_check( mutex, location );

  ___POSIX_Mutex_Get_support_auto_initialization( mutex, location );

  return (POSIX_Mutex_Control *) _Objects_Get_isr_disable(
    &_POSIX_Mutex_Information,
    (Objects_Id) *mutex,
    location,
    level
  );
}
