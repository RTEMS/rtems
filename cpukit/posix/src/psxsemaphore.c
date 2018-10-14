/**
 * @file
 *
 * @brief POSIX Function Initializes Semaphore Manager
 * @ingroup POSIXAPI
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

#include <rtems/posix/semaphoreimpl.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>

#include <limits.h>

Objects_Information _POSIX_Semaphore_Information;

/*
 *  _POSIX_Semaphore_Manager_initialization
 *
 *  This routine initializes all semaphore manager related data structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

static void _POSIX_Semaphore_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Semaphore_Information, /* object information table */
    OBJECTS_POSIX_API,          /* object API */
    OBJECTS_POSIX_SEMAPHORES,   /* object class */
    _Configuration_POSIX_Maximum_named_semaphores,
    sizeof( POSIX_Semaphore_Control ),
                                /* size of this object's control block */
    true,                       /* true if names for this object are strings */
    _POSIX_PATH_MAX,            /* maximum length of each object's name */
    NULL                        /* Proxy extraction support callout */
  );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Semaphore_Manager_initialization,
  RTEMS_SYSINIT_POSIX_SEMAPHORE,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
