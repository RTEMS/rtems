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
 *  sem_open
 *
 *  Opens a named semaphore.  Used in conjunction with the sem_close
 *  and sem_unlink commands.
 *
 *  11.2.3 Initialize/Open a Named Semaphore, P1003.1b-1993, p.221
 *
 *  NOTE: When oflag is O_CREAT, then optional third and fourth
 *        parameters must be present.
 */

sem_t *sem_open(
  const char *name,
  int         oflag,
  ...
  /* mode_t mode, */
  /* unsigned int value */
)
{
  va_list                    arg;
  mode_t                     mode;
  unsigned int               value = 0;
  int                        status;
  Objects_Id                 the_semaphore_id;
  POSIX_Semaphore_Control   *the_semaphore;
  Objects_Locations          location;
  size_t                     name_len;

  _Thread_Disable_dispatch();

  if ( oflag & O_CREAT ) {
    va_start(arg, oflag);
    mode = va_arg( arg, mode_t );
    value = va_arg( arg, unsigned int );
    va_end(arg);
  }

  status = _POSIX_Semaphore_Name_to_id( name, &the_semaphore_id, &name_len );

  /*
   *  If the name to id translation worked, then the semaphore exists
   *  and we can just return a pointer to the id.  Otherwise we may
   *  need to check to see if this is a "semaphore does not exist"
   *  or some other miscellaneous error on the name.
   */

  if ( status ) {

    /*
     * Unless provided a valid name that did not already exist
     * and we are willing to create then it is an error.
     */

    if ( !( status == ENOENT && (oflag & O_CREAT) ) ) {
      _Thread_Enable_dispatch();
      rtems_set_errno_and_return_minus_one_cast( status, sem_t * );
    }
  } else {

    /*
     * Check for existence with creation.
     */

    if ( (oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL) ) {
      _Thread_Enable_dispatch();
      rtems_set_errno_and_return_minus_one_cast( EEXIST, sem_t * );
    }

    the_semaphore = _POSIX_Semaphore_Get( (sem_t *) &the_semaphore_id, &location );
    the_semaphore->open_count += 1;
    _Thread_Enable_dispatch();
    _Thread_Enable_dispatch();
    goto return_id;
  }

  /*
   *  At this point, the semaphore does not exist and everything has been
   *  checked. We should go ahead and create a semaphore.
   */

  status =_POSIX_Semaphore_Create_support(
    name,
    name_len,
    false,         /* not shared across processes */
    value,
    &the_semaphore
  );

  /*
   * errno was set by Create_support, so don't set it again.
   */

  _Thread_Enable_dispatch();

  if ( status == -1 )
    return SEM_FAILED;

return_id:
  #if defined(RTEMS_USE_16_BIT_OBJECT)
    the_semaphore->Semaphore_id = the_semaphore->Object.id;
    return &the_semaphore->Semaphore_id;
  #else
    return (sem_t *)&the_semaphore->Object.id;
  #endif
}
