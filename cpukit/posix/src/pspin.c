/*
 *  Spinlock Manager
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Spinlock Manager.
 *
 *  Directives provided are:
 *
 *     + create a spinlock
 *     + get an ID of a spinlock
 *     + delete a spinlock
 *     + acquire a spinlock
 *     + release a spinlock
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
#include <rtems/posix/spinlock.h>

/**
 *  @brief _POSIX_Spinlock_Manager_initialization
 */

void _POSIX_Spinlock_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Spinlock_Information,    /* object information table */
    OBJECTS_POSIX_API,               /* object API */
    OBJECTS_POSIX_SPINLOCKS,         /* object class */
    Configuration_POSIX_API.maximum_spinlocks,
                                     /* maximum objects of this class */
    sizeof( POSIX_Spinlock_Control ),/* size of this object's control block */
    true,                            /* true if the name is a string */
    _POSIX_PATH_MAX                  /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                           /* true if this is a global object class */
    NULL                             /* Proxy extraction support callout */
#endif
  );
}
