/**
 * @file
 *
 * @brief POSIX Mutex Manager Initialization
 * @ingroup POSIX_MUTEX POSIX Mutex Support
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>

#include <rtems/config.h>
#include <rtems/sysinit.h>
#include <rtems/posix/muteximpl.h>
#include <rtems/score/objectimpl.h>

Objects_Information _POSIX_Mutex_Information;

const pthread_mutexattr_t _POSIX_Mutex_Default_attributes = {
#if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)
  .type           = PTHREAD_MUTEX_DEFAULT,
#endif
  .is_initialized = true,
  .process_shared = PTHREAD_PROCESS_PRIVATE,
  .prio_ceiling   = INT_MAX,
  .protocol       = PTHREAD_PRIO_NONE,
  .recursive      = false
};

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

static void _POSIX_Mutex_Manager_initialization(void)
{
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
    _POSIX_PATH_MAX,            /* maximum length of each object's name */
    NULL                        /* Proxy extraction support callout */
  );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Mutex_Manager_initialization,
  RTEMS_SYSINIT_POSIX_MUTEX,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
