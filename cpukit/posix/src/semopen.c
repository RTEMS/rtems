/*
 *  $Id$
 */

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
#include <rtems/posix/seterr.h>

/*PAGE
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
 

  if ( oflag & O_CREAT ) {
    va_start(arg, oflag);
    /*mode = (mode_t) va_arg( arg, mode_t * );*/
    mode = va_arg( arg, mode_t );
    /*value = (unsigned int) va_arg( arg, unsigned int * );*/
    value = va_arg( arg, unsigned int );
    va_end(arg);
  }

  status = _POSIX_Semaphore_Name_to_id( name, &the_semaphore_id );
   
  /*
   *  If the name to id translation worked, then the semaphore exists
   *  and we can just return a pointer to the id.  Otherwise we may
   *  need to check to see if this is a "semaphore does not exist"
   *  or some other miscellaneous error on the name.
   */

  if ( status ) {

    if ( status == EINVAL ) {      /* name -> ID translation failed */
      if ( !(oflag & O_CREAT) ) {  /* willing to create it? */
        set_errno_and_return_minus_one_cast( ENOENT, sem_t * );
      }
      /* we are willing to create it */
    }
    /* some type of error */
    /*set_errno_and_return_minus_one_cast( status, sem_t * );*/

  } else {                /* name -> ID translation succeeded */

    if ( (oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL) ) {
      set_errno_and_return_minus_one_cast( EEXIST, sem_t * );
    }

    /* 
     * XXX In this case we need to do an ID->pointer conversion to
     *     check the mode.   This is probably a good place for a subroutine.
     */

    the_semaphore = _POSIX_Semaphore_Get( &the_semaphore_id, &location );
    the_semaphore->open_count += 1;

    return (sem_t *)&the_semaphore->Object.id;

  } 
  
  /* XXX verify this comment...
   *
   *  At this point, the semaphore does not exist and everything has been
   *  checked. We should go ahead and create a semaphore.
   */

  status = _POSIX_Semaphore_Create_support(
    name,
    FALSE,         /* not shared across processes */
    value,
    &the_semaphore
  );
 
  if ( status == -1 )
    return (sem_t *) -1;

  return (sem_t *) &the_semaphore->Object.id;
 
}
