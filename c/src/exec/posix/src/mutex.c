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
 *  _POSIX_Mutex_Manager_initialization
 *
 *  This routine initializes all mutex manager related data structures.
 *
 *  Input parameters:
 *    maximum_mutexes - maximum configured mutexes
 *
 *  Output parameters:  NONE
 */
 
void _POSIX_Mutex_Manager_initialization(
  unsigned32 maximum_mutexes
)
{
  _Objects_Initialize_information(
    &_POSIX_Mutex_Information,
    OBJECTS_POSIX_MUTEXES,
    TRUE,
    maximum_mutexes,
    sizeof( POSIX_Mutex_Control ),
    FALSE,
    0,
    FALSE
  );
}
