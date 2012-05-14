/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <limits.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/watchdog.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*
 *  _POSIX_Mutex_Manager_initialization
 *
 *  This routine initializes all mutex manager related data structures.
 *
 *  Input parameters:
 *    maximum_mutexes - maximum configured mutexes
 *
 *  Output parameters:  NONE
 */

void _POSIX_Mutex_Manager_initialization(void)
{
  pthread_mutexattr_t *default_attr = &_POSIX_Mutex_Default_attributes;

  /*
   * Since the maximum priority is run-time configured, this
   * structure cannot be initialized statically.
   */
  default_attr->is_initialized = true;
  default_attr->process_shared = PTHREAD_PROCESS_PRIVATE;
  default_attr->prio_ceiling   = POSIX_SCHEDULER_MAXIMUM_PRIORITY;
  default_attr->protocol       = PTHREAD_PRIO_NONE;
  default_attr->recursive      = false;
  #if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)
    default_attr->type         = PTHREAD_MUTEX_DEFAULT;
  #endif

  /*
   * Initialize the POSIX mutex object class information structure.
   */
  _Objects_Initialize_information(
    &_POSIX_Mutex_Information,  /* object information table */
    OBJECTS_POSIX_API,          /* object API */
    OBJECTS_POSIX_MUTEXES,      /* object class */
    Configuration_POSIX_API.maximum_mutexes,
                                /* maximum objects of this class */
    sizeof( POSIX_Mutex_Control ),
                                /* size of this object's control block */
    true,                       /* true if names for this object are strings */
    _POSIX_PATH_MAX             /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                      /* true if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );
}
