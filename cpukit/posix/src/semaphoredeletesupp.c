/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/object.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

/*
 *  _POSIX_Semaphore_Delete
 */

void _POSIX_Semaphore_Delete(
  POSIX_Semaphore_Control *the_semaphore
)
{
  if ( !the_semaphore->linked && !the_semaphore->open_count ) {
      _Objects_Close( &_POSIX_Semaphore_Information, &the_semaphore->Object );

      _CORE_semaphore_Flush(
        &the_semaphore->Semaphore,
        NULL,
        -1
      );

    _POSIX_Semaphore_Free( the_semaphore );
  }
}
