/*
 *  RWLock Manager -- Initialization
 *
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

#include <limits.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/posix/rwlock.h>

/**
 *  @brief _POSIX_RWLock_Manager_initialization
 */

void _POSIX_RWLock_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_RWLock_Information,     /* object information table */
    OBJECTS_POSIX_API,              /* object API */
    OBJECTS_POSIX_SPINLOCKS,        /* object class */
    Configuration_POSIX_API.maximum_rwlocks,
                                    /* maximum objects of this class */
    sizeof( POSIX_RWLock_Control ), /* size of this object's control block */
    true,                           /* true if the name is a string */
    _POSIX_PATH_MAX                 /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                         /* true if this is a global object class */
    NULL                           /* Proxy extraction support callout */
#endif
  );
}
