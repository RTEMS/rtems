/**
 *  @file
 *
 *  This file contains the initialization of the POSIX Spinlock Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
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

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>
#include <rtems/posix/spinlockimpl.h>

Objects_Information _POSIX_Spinlock_Information;

/**
 *  @brief _POSIX_Spinlock_Manager_initialization
 */
static void _POSIX_Spinlock_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Spinlock_Information,    /* object information table */
    OBJECTS_POSIX_API,               /* object API */
    OBJECTS_POSIX_SPINLOCKS,         /* object class */
    Configuration_POSIX_API.maximum_spinlocks,
                                     /* maximum objects of this class */
    sizeof( POSIX_Spinlock_Control ),/* size of this object's control block */
    true,                            /* true if the name is a string */
    _POSIX_PATH_MAX,                 /* maximum length of each object's name */
    NULL                             /* Proxy extraction support callout */
  );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Spinlock_Manager_initialization,
  RTEMS_SYSINIT_POSIX_SPINLOCK,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
