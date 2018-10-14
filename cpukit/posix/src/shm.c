/**
 * @file
 */

/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/libio.h>
#include <rtems/sysinit.h>
#include <rtems/posix/shmimpl.h>

Objects_Information _POSIX_Shm_Information;

static void _POSIX_Shm_Manager_initialization( void )
{
  _Objects_Initialize_information(
    &_POSIX_Shm_Information,    /* object information table */
    OBJECTS_POSIX_API,          /* object API */
    OBJECTS_POSIX_SHMS,         /* object class */
    _Configuration_POSIX_Maximum_shms,
    sizeof( POSIX_Shm_Control ),
                                /* size of this object's control block */
    true,                       /* true if names for this object are strings */
    _POSIX_PATH_MAX,            /* maximum length of each object's name */
    NULL                        /* Proxy extraction support callout */
  );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Shm_Manager_initialization,
  RTEMS_SYSINIT_POSIX_SHM,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
