/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/watchdog.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  11.3.2 Initializing and Destroying a Mutex, P1003.1c/Draft 10, p. 87
 */

int pthread_mutex_destroy(
  pthread_mutex_t           *mutex
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
 
  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      return EINVAL;
#endif
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_LOCAL:
       /*
        * XXX: There is an error for the mutex being locked
        *  or being in use by a condition variable.
        */

      if ( _CORE_mutex_Is_locked( &the_mutex->Mutex ) ) {
        _Thread_Enable_dispatch();
        return EBUSY;
      }
 
      _Objects_Close( &_POSIX_Mutex_Information, &the_mutex->Object );
 
      _CORE_mutex_Flush(
        &the_mutex->Mutex,
#if defined(RTEMS_MULTIPROCESSING)
        _POSIX_Mutex_MP_Send_object_was_deleted,
#else
        NULL,
#endif
        EINVAL
      );
 
      _POSIX_Mutex_Free( the_mutex );
 
#if defined(RTEMS_MULTIPROCESSING)
      if ( the_mutex->process_shared == PTHREAD_PROCESS_SHARED ) {
 
        _Objects_MP_Close( &_POSIX_Mutex_Information, the_mutex->Object.id );
 
        _POSIX_Mutex_MP_Send_process_packet(
          POSIX_MUTEX_MP_ANNOUNCE_DELETE,
          the_mutex->Object.id,
          0,                         /* Not used */
          0                          /* Not used */
        );
      }
#endif
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
