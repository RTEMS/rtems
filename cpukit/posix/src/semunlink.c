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
#include <rtems/seterr.h>

/*PAGE
 *
 *  sem_unlink
 *  
 *  Unlinks a named semaphore, sem_close must also be called to remove
 *  the semaphore.
 *
 *  11.2.5 Remove a Named Semaphore, P1003.1b-1993, p.225
 */

int sem_unlink(
  const char *name
)
{
  int  status;
  register POSIX_Semaphore_Control *the_semaphore;
  sem_t                        the_semaphore_id;
 
  _Thread_Disable_dispatch();

  status = _POSIX_Semaphore_Name_to_id( name, &the_semaphore_id );
  if ( status != 0 ) {
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( status );
  }

  /*
   *  Don't support unlinking a remote semaphore.
   */

  if ( !_Objects_Is_local_id(the_semaphore_id) ) {
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( ENOSYS );
  }

  the_semaphore = (POSIX_Semaphore_Control *) _Objects_Get_local_object(
    &_POSIX_Semaphore_Information,
    _Objects_Get_index( the_semaphore_id )
  );
  
#if defined(RTEMS_MULTIPROCESSING)
  if ( the_semaphore->process_shared == PTHREAD_PROCESS_SHARED ) {
    _Objects_MP_Close( &_POSIX_Semaphore_Information, the_semaphore_id );
  }
#endif

  the_semaphore->linked = FALSE;
  _POSIX_Semaphore_Namespace_remove( the_semaphore );
  _POSIX_Semaphore_Delete( the_semaphore );

  _Thread_Enable_dispatch();
  return 0;
}
