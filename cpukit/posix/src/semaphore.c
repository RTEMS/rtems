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
    &_POSIX_Semaphore_Information,
    OBJECTS_POSIX_SEMAPHORES,
    TRUE,
    maximum_semaphores,
    sizeof( POSIX_Semaphore_Control ),
    TRUE,
    _POSIX_PATH_MAX,
    FALSE
  );
}

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


/*PAGE
 *
 *  11.2.1 Initialize an Unnamed Semaphore, P1003.1b-1993, p.219
 */

int sem_init(
  sem_t         *sem,
  int            pshared,
  unsigned int   value
)
{
  int                        status;
  POSIX_Semaphore_Control   *the_semaphore;

  status = _POSIX_Semaphore_Create_support(
    NULL,
    pshared,
    value,
    &the_semaphore
  );
    
  if ( status != -1 )
    *sem = the_semaphore->Object.id;

  return status;
}

/*PAGE
 *
 *  11.2.2 Destroy an Unnamed Semaphore, P1003.1b-1993, p.220
 */

int sem_destroy(
  sem_t *sem
)
{
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations                 location;
 
  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:
      /*
       *  Undefined operation on a named semaphore.
       */

      if ( the_semaphore->named == TRUE ) {
        set_errno_and_return_minus_one( EINVAL );
      }
 
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
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

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

/*PAGE
 *
 *  _POSIX_Semaphore_Delete
 */

void _POSIX_Semaphore_Delete(
  POSIX_Semaphore_Control *the_semaphore
)
{
  if ( !the_semaphore->linked && !the_semaphore->open_count ) {
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

/*PAGE
 *
 *  11.2.4 Close a Named Semaphore, P1003.1b-1993, p.224
 */

int sem_close(
  sem_t *sem
)
{
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations                 location;
 
  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:
      the_semaphore->open_count -= 1;
      _POSIX_Semaphore_Delete( the_semaphore );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  11.2.5 Remove a Named Semaphore, P1003.1b-1993, p.225
 */

int sem_unlink(
  const char *name
)
{
  int  status;
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Id                        the_semaphore_id;
  Objects_Locations                 location;
 
  status = _POSIX_Semaphore_Name_to_id( name, &the_semaphore_id );
   
  if ( !status )
    set_errno_and_return_minus_one( status );

  the_semaphore = _POSIX_Semaphore_Get( &the_semaphore_id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:

#if defined(RTEMS_MULTIPROCESSING)
      if ( the_semaphore->process_shared == PTHREAD_PROCESS_SHARED ) {
        _Objects_MP_Close(
          &_POSIX_Semaphore_Information,
          the_semaphore->Object.id
        );
      }
#endif

      the_semaphore->linked = FALSE;

      _POSIX_Semaphore_Delete( the_semaphore );

      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  _POSIX_Semaphore_Wait_support
 */

int _POSIX_Semaphore_Wait_support(
  sem_t              *sem,
  boolean             blocking,
  Watchdog_Interval   timeout
)
{
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations                 location;
  int                               code;
 
  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:
      _CORE_semaphore_Seize(
        &the_semaphore->Semaphore,
        the_semaphore->Object.id,
        blocking,
        timeout
      );
      _Thread_Enable_dispatch();
      code = _Thread_Executing->Wait.return_code;
      switch (_Thread_Executing->Wait.return_code) {
        case 1:
	  errno = EAGAIN;
	  code = -1;
	  break;
	case 3:
	  errno = ETIMEDOUT;
	  code = -1;
	  break;
      }

      /*return _Thread_Executing->Wait.return_code;*/
      return code;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  11.2.6 Lock a Semaphore, P1003.1b-1993, p.226
 *
 *  NOTE: P1003.4b/D8 adds sem_timedwait(), p. 27
 */

int sem_wait(
  sem_t *sem
)
{
  return _POSIX_Semaphore_Wait_support( sem, TRUE, THREAD_QUEUE_WAIT_FOREVER );
}

/*PAGE
 *
 *  11.2.6 Lock a Semaphore, P1003.1b-1993, p.226
 *
 *  NOTE: P1003.4b/D8 adds sem_timedwait(), p. 27
 */

int sem_trywait(
  sem_t *sem
)
{
  return _POSIX_Semaphore_Wait_support( sem, FALSE, THREAD_QUEUE_WAIT_FOREVER );
}

/*PAGE
 *
 *  11.2.6 Lock a Semaphore, P1003.1b-1993, p.226
 *
 *  NOTE: P1003.4b/D8 adds sem_timedwait(), p. 27
 */

int sem_timedwait(
  sem_t                 *sem,
  const struct timespec *timeout
)
{
  return _POSIX_Semaphore_Wait_support(
    sem,
    TRUE,
    _POSIX_Timespec_to_interval( timeout )
  );
}

/*PAGE
 *
 *  11.2.7 Unlock a Semaphore, P1003.1b-1993, p.227
 */

void POSIX_Semaphore_MP_support(
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  (void) POSIX_MP_NOT_IMPLEMENTED();
}
 

int sem_post(
  sem_t  *sem
)
{
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations                 location;
 
  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:
      _CORE_semaphore_Surrender(
        &the_semaphore->Semaphore,
        the_semaphore->Object.id,
#if defined(RTEMS_MULTIPROCESSING)
        POSIX_Semaphore_MP_support
#else
        NULL
#endif
      );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  11.2.8 Get the Value of a Semaphore, P1003.1b-1993, p.229
 */

int sem_getvalue(
  sem_t  *sem,
  int    *sval
)
{
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations                 location;
 
  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:
      *sval = _CORE_semaphore_Get_count( &the_semaphore->Semaphore );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  _POSIX_Semaphore_Name_to_id
 *
 *  XXX
 */

int _POSIX_Semaphore_Name_to_id(
  const char          *name,
  Objects_Id          *id
)
{
  Objects_Name_to_id_errors  status;

  status = _Objects_Name_to_id( &_POSIX_Semaphore_Information, (char *)name, 0, id );

  if ( status == OBJECTS_SUCCESSFUL ) {
	  return 0;
  } else {
	  return EINVAL;
  }
}

