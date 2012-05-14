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

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/object.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

/*
 *  _POSIX_Semaphore_Manager_initialization
 *
 *  This routine initializes all semaphore manager related data structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _POSIX_Semaphore_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Semaphore_Information, /* object information table */
    OBJECTS_POSIX_API,          /* object API */
    OBJECTS_POSIX_SEMAPHORES,   /* object class */
    Configuration_POSIX_API.maximum_semaphores,
                                /* maximum objects of this class */
    sizeof( POSIX_Semaphore_Control ),
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
