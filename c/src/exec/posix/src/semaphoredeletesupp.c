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
#if defined(RTEMS_MULTIPROCESSING)
        _POSIX_Semaphore_MP_Send_object_was_deleted,
#else
        NULL,
#endif
        -1  /* XXX should also seterrno -> EINVAL */
      );

    _POSIX_Semaphore_Free( the_semaphore );

#if defined(RTEMS_MULTIPROCESSING)
    if ( the_semaphore->process_shared == PTHREAD_PROCESS_SHARED ) {

      _Objects_MP_Close(
        &_POSIX_Semaphore_Information,
        the_semaphore->Object.id
      );

      _POSIX_Semaphore_MP_Send_process_packet(
        POSIX_SEMAPHORE_MP_ANNOUNCE_DELETE,
        the_semaphore->Object.id,
        0,                         /* Not used */
        0                          /* Not used */
      );
    }
#endif

  }
}
