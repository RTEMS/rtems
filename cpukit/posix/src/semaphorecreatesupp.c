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
 *  _POSIX_Semaphore_Create_support
 */

int _POSIX_Semaphore_Create_support(
  const char                *name,
  int                        pshared,
  unsigned int               value,
  POSIX_Semaphore_Control  **the_sem
)
{
  POSIX_Semaphore_Control   *the_semaphore;
  CORE_semaphore_Attributes *the_sem_attr;

  _Thread_Disable_dispatch();
 
  /* Sharing semaphores among processes is not currently supported */
  if (pshared != 0) {
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( ENOSYS );
  }

  the_semaphore = _POSIX_Semaphore_Allocate();
 
  if ( !the_semaphore ) {
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( ENOSPC );
  }
 
#if defined(RTEMS_MULTIPROCESSING)
  if ( pshared == PTHREAD_PROCESS_SHARED &&
       !( _Objects_MP_Allocate_and_open( &_POSIX_Semaphore_Information, 0,
                            the_semaphore->Object.id, FALSE ) ) ) {
    _POSIX_Semaphore_Free( the_semaphore );
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( EAGAIN );
  }
#endif
 
  the_semaphore->process_shared  = pshared;

  if ( name ) {
    the_semaphore->named = TRUE;
    the_semaphore->open_count = 1;
    the_semaphore->linked = TRUE;
  }
  else 
    the_semaphore->named = FALSE;

  the_sem_attr = &the_semaphore->Semaphore.Attributes;
 
  /* XXX
   *
   *  Note should this be based on the current scheduling policy?
   */

  the_sem_attr->discipline = CORE_SEMAPHORE_DISCIPLINES_FIFO;

  /*
   *  This effectively disables limit checking.
   */

  the_sem_attr->maximum_count = 0xFFFFFFFF;

  _CORE_semaphore_Initialize(
    &the_semaphore->Semaphore,
    OBJECTS_POSIX_SEMAPHORES,
    the_sem_attr,
    value,
    0      /* XXX - proxy_extract_callout is unused */
  );
 
  /* XXX - need Names to be a string!!! */
  _Objects_Open(
    &_POSIX_Semaphore_Information,
    &the_semaphore->Object,
    (char *) name
  );
 
  *the_sem = the_semaphore;
 
#if defined(RTEMS_MULTIPROCESSING)
  if ( pshared == PTHREAD_PROCESS_SHARED )
    _POSIX_Semaphore_MP_Send_process_packet(
      POSIX_SEMAPHORE_MP_ANNOUNCE_CREATE,
      the_semaphore->Object.id,
      (char *) name,
      0                /* proxy id - Not used */
    );
#endif
 
  _Thread_Enable_dispatch();
  return 0;
}
