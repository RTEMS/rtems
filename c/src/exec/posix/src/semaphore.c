/*
 *  $Id$
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
#include <rtems/score/object.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

/*PAGE
 *
 *  _POSIX_Semaphore_Manager_initialization
 *
 *  This routine initializes all semaphore manager related data structures.
 *
 *  Input parameters:
 *    maximum_semaphores - maximum configured semaphores
 *
 *  Output parameters:  NONE
 */
 
void _POSIX_Semaphore_Manager_initialization(
  unsigned32 maximum_semaphores
)
{
  _Objects_Initialize_information(
    &_POSIX_Semaphore_Information, /* object information table */
    OBJECTS_POSIX_API,             /* object API */
    OBJECTS_POSIX_SEMAPHORES,      /* object class */
    maximum_semaphores             /* maximum objects of this class */,
    sizeof( POSIX_Semaphore_Control ),
                                /* size of this object's control block */
    FALSE,                      /* TRUE if names for this object are strings */
    _POSIX_PATH_MAX             /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                      /* TRUE if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );
}
