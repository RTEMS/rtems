/*
 *  NOTE:  The structure of the routines is identical to that of POSIX
 *         Message_queues to leave the option of having unnamed message
 *         queues at a future date.  They are currently not part of the
 *         POSIX standard but unnamed message_queues are.  This is also 
 *         the reason for the apparently unnecessary tracking of 
 *         the process_shared attribute.  [In addition to the fact that
 *         it would be trivial to add pshared to the mq_attr structure
 *         and have process private message queues.]
 *
 *         This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open
 *         time.
 *
 *  $Id$
 */

#include <stdarg.h>

#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>

#include <rtems/system.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  _POSIX_Message_queue_Manager_initialization
 *
 *  This routine initializes all message_queue manager related data structures.
 *
 *  Input parameters:
 *    maximum_message_queues - maximum configured message_queues
 *
 *  Output parameters:  NONE
 */
 
void _POSIX_Message_queue_Manager_initialization(
  unsigned32 maximum_message_queues
)
{
  _Objects_Initialize_information(
    &_POSIX_Message_queue_Information,
    OBJECTS_POSIX_MESSAGE_QUEUES,
    TRUE,
    maximum_message_queues,
    sizeof( POSIX_Message_queue_Control ),
    TRUE,
    _POSIX_PATH_MAX,
    FALSE
  );
}

/*PAGE
 *
 *  _POSIX_Message_queue_Create_support
 */
 
int _POSIX_Message_queue_Create_support(
  const char                    *name,
  int                            pshared,
  unsigned int                   oflag,
  struct mq_attr                *attr,
  POSIX_Message_queue_Control  **message_queue
)
{
  POSIX_Message_queue_Control   *the_mq;
 
  _Thread_Disable_dispatch();
 
  the_mq = _POSIX_Message_queue_Allocate();
 
  if ( !the_mq ) {
    _Thread_Enable_dispatch();
    seterrno( ENFILE );
    return -1;
  }
 
  if ( pshared == PTHREAD_PROCESS_SHARED &&
       !( _Objects_MP_Allocate_and_open( &_POSIX_Message_queue_Information, 0,
                            the_mq->Object.id, FALSE ) ) ) {
    _POSIX_Message_queue_Free( the_mq );
    _Thread_Enable_dispatch();
    seterrno( ENFILE );
    return -1;
  }
 
  the_mq->process_shared  = pshared;
 
  if ( name ) {
    the_mq->named = TRUE;
    the_mq->open_count = 1;
    the_mq->linked = TRUE;
  }
  else
    the_mq->named = FALSE;
 
  if ( oflag & O_NONBLOCK ) 
    the_mq->blocking = FALSE;
  else
    the_mq->blocking = TRUE;
 
  /* XXX
   *
   *  Note that this should be based on the current scheduling policy.
   */

  /* XXX
   *
   *  Message and waiting disciplines are not distinguished.
   */
/*
  the_mq_attr->message_discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;
  the_mq_attr->waiting_discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;
 */

  the_mq->Message_queue.Attributes.discipline =
                                         CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;
 
  if ( ! _CORE_message_queue_Initialize(
           &the_mq->Message_queue,
           OBJECTS_POSIX_MESSAGE_QUEUES,
           &the_mq->Message_queue.Attributes,
           attr->mq_maxmsg,
           attr->mq_msgsize,
           _POSIX_Message_queue_MP_Send_extract_proxy ) ) {

    if ( pshared == PTHREAD_PROCESS_SHARED )
      _Objects_MP_Close( &_POSIX_Message_queue_Information, the_mq->Object.id );
 
    _POSIX_Message_queue_Free( the_mq );
    _Thread_Enable_dispatch();
    seterrno( ENOSPC );
    return -1;
  }

 
  /* XXX - need Names to be a string!!! */
  _Objects_Open(
    &_POSIX_Message_queue_Information,
    &the_mq->Object,
    (char *) name
  );
 
  *message_queue = the_mq;
 
  if ( pshared == PTHREAD_PROCESS_SHARED )
    _POSIX_Message_queue_MP_Send_process_packet(
      POSIX_MESSAGE_QUEUE_MP_ANNOUNCE_CREATE,
      the_mq->Object.id,
      (char *) name,
      0                          /* Not used */
    );
 
  _Thread_Enable_dispatch();
  return 0;
}

/*PAGE
 *
 *  15.2.2 Open a Message Queue, P1003.1b-1993, p. 272
 */

mqd_t mq_open(
  const char *name,
  int         oflag,
  ...
  /* mode_t mode, */
  /* struct mq_attr  attr */
)
{
  va_list arg;
  mode_t mode;
  struct mq_attr *attr;
  int                        status;
  Objects_Id                 the_mq_id;
  POSIX_Message_queue_Control   *the_mq;
 
  if ( oflag & O_CREAT ) {
    va_start(arg, oflag);
    mode = (mode_t) va_arg( arg, mode_t * );
    attr = (struct mq_attr *) va_arg( arg, struct mq_attr ** );
    va_end(arg);
  }
 
  status = _POSIX_Message_queue_Name_to_id( name, &the_mq_id );
 
  /*
   *  If the name to id translation worked, then the message queue exists
   *  and we can just return a pointer to the id.  Otherwise we may
   *  need to check to see if this is a "message queue does not exist"
   *  or some other miscellaneous error on the name.
   */
 
  if ( status ) {
 
    if ( status == EINVAL ) {      /* name -> ID translation failed */
      if ( !(oflag & O_CREAT) ) {  /* willing to create it? */
        seterrno( ENOENT );
        return (mqd_t) -1;
      }
      /* we are willing to create it */
    }
    seterrno( status );               /* some type of error */
    return (mqd_t) -1;
 
  } else {                /* name -> ID translation succeeded */
 
    if ( (oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL) ) {
      seterrno( EEXIST );
      return (mqd_t) -1;
    }
 
    /*
     * XXX In this case we need to do an ID->pointer conversion to
     *     check the mode.   This is probably a good place for a subroutine.
     */
 
    the_mq->open_count += 1;
 
    return (mqd_t)&the_mq->Object.id;
 
  }
 
  /* XXX verify this comment...
   *
   *  At this point, the message queue does not exist and everything has been
   *  checked. We should go ahead and create a message queue.
   */
 
  status = _POSIX_Message_queue_Create_support(
    name,
    TRUE,         /* shared across processes */
    oflag,
    attr,
    &the_mq
  );
 
  if ( status == -1 )
    return (mqd_t) -1;
 
  return (mqd_t) &the_mq->Object.id;
}

/*PAGE
 *
 *  _POSIX_Message_queue_Delete
 */
 
void _POSIX_Message_queue_Delete(
  POSIX_Message_queue_Control *the_mq
)
{
  if ( !the_mq->linked && !the_mq->open_count ) {
    _POSIX_Message_queue_Free( the_mq );
 
    if ( the_mq->process_shared == PTHREAD_PROCESS_SHARED ) {
 
      _Objects_MP_Close(
        &_POSIX_Message_queue_Information,
        the_mq->Object.id
      );
 
      _POSIX_Message_queue_MP_Send_process_packet(
        POSIX_MESSAGE_QUEUE_MP_ANNOUNCE_DELETE,
        the_mq->Object.id,
        0,                         /* Not used */
        0                          /* Not used */
      );
    }
 
  }
}

/*PAGE
 *
 *  15.2.2 Close a Message Queue, P1003.1b-1993, p. 275
 */

int mq_close(
  mqd_t  mqdes
)
{
  register POSIX_Message_queue_Control *the_mq;
  Objects_Locations                     location;
 
  the_mq = _POSIX_Message_queue_Get( mqdes, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_LOCAL:
      the_mq->open_count -= 1;
      _POSIX_Message_queue_Delete( the_mq );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  15.2.2 Remove a Message Queue, P1003.1b-1993, p. 276
 */

int mq_unlink(
  const char *name
)
{
  int  status;
  register POSIX_Message_queue_Control *the_mq;
  Objects_Id                        the_mq_id;
  Objects_Locations                 location;
 
  status = _POSIX_Message_queue_Name_to_id( name, &the_mq_id );
 
  if ( !status ) {
    seterrno( status );
    return -1;
  }
 
  the_mq = _POSIX_Message_queue_Get( the_mq_id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_LOCAL:
 
      _Objects_MP_Close(
        &_POSIX_Message_queue_Information,
        the_mq->Object.id
      );
 
      the_mq->linked = FALSE;
 
      _POSIX_Message_queue_Delete( the_mq );
 
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  _POSIX_Message_queue_Send_support
 */
 
int _POSIX_Message_queue_Send_support(
  mqd_t               mqdes,
  const char         *msg_ptr,
  unsigned32          msg_len,
  Priority_Control    msg_prio,
  Watchdog_Interval   timeout
)
{
  register POSIX_Message_queue_Control *the_mq;
  Objects_Locations                     location;
 
  the_mq = _POSIX_Message_queue_Get( mqdes, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_LOCAL:
      /* XXX must add support for timeout and priority */
      _CORE_message_queue_Send(
        &the_mq->Message_queue,
        (void *) msg_ptr,
        msg_len,
        mqdes,
        NULL       /* XXX _POSIX_Message_queue_Core_message_queue_mp_support*/
      );
      _Thread_Enable_dispatch();
      return _Thread_Executing->Wait.return_code;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  15.2.4 Send a Message to a Message Queue, P1003.1b-1993, p. 277
 *
 *  NOTE: P1003.4b/D8, p. 45 adds mq_timedsend().
 */

int mq_send(
  mqd_t         mqdes,
  const char   *msg_ptr,
  size_t        msg_len,
  unsigned int  msg_prio 
)
{
  return _POSIX_Message_queue_Send_support(
    mqdes,
    msg_ptr,
    msg_len,
    msg_prio,
    THREAD_QUEUE_WAIT_FOREVER
  );
}

/*PAGE
 *
 *  15.2.4 Send a Message to a Message Queue, P1003.1b-1993, p. 277
 *
 *  NOTE: P1003.4b/D8, p. 45 adds mq_timedsend().
 */

int mq_timedsend(
  mqd_t                  mqdes,
  const char            *msg_ptr,
  size_t                 msg_len,
  unsigned int           msg_prio,
  const struct timespec *timeout
)
{
  return _POSIX_Message_queue_Send_support(
    mqdes,
    msg_ptr,
    msg_len,
    msg_prio,
    _POSIX_Timespec_to_interval( timeout )
  );
}

/*PAGE
 *
 *  _POSIX_Message_queue_Receive_support
 */
 
/* XXX be careful ... watch the size going through all the layers ... */

ssize_t _POSIX_Message_queue_Receive_support(
  mqd_t               mqdes,
  char               *msg_ptr,
  size_t              msg_len,
  unsigned int       *msg_prio,
  Watchdog_Interval   timeout
)
{
  register POSIX_Message_queue_Control *the_mq;
  Objects_Locations                     location;
  unsigned32                            status = 0;
  unsigned32                            length_out;
 
  the_mq = _POSIX_Message_queue_Get( mqdes, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_LOCAL:
      /* XXX need to define the options argument to this */
      length_out = msg_len;
      _CORE_message_queue_Seize(
        &the_mq->Message_queue,
        mqdes,
        msg_ptr,
        &length_out,
        /* msg_prio,    XXXX */
        the_mq->blocking,
        timeout
      );
      _Thread_Enable_dispatch();
      if ( !status )
        return length_out;
      /* XXX --- the return codes gotta be looked at .. fix this */
      return _Thread_Executing->Wait.return_code;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  15.2.5 Receive a Message From a Message Queue, P1003.1b-1993, p. 279
 *
 *  NOTE: P1003.4b/D8, p. 45 adds mq_timedreceive().
 */

ssize_t mq_receive(
  mqd_t         mqdes,
  char         *msg_ptr,
  size_t        msg_len,
  unsigned int *msg_prio
)
{
  return _POSIX_Message_queue_Receive_support(
    mqdes,
    msg_ptr,
    msg_len,
    msg_prio,
    THREAD_QUEUE_WAIT_FOREVER
  );
}

/*PAGE
 *
 *  15.2.5 Receive a Message From a Message Queue, P1003.1b-1993, p. 279
 *
 *  NOTE: P1003.4b/D8, p. 45 adds mq_timedreceive().
 */

int mq_timedreceive(                  /* XXX: should this be ssize_t */
  mqd_t                  mqdes,
  char                  *msg_ptr,
  size_t                 msg_len,
  unsigned int          *msg_prio,
  const struct timespec *timeout
)
{
  return _POSIX_Message_queue_Receive_support(
    mqdes,
    msg_ptr,
    msg_len,
    msg_prio,
    _POSIX_Timespec_to_interval( timeout )
  );
}

/*PAGE
 *
 *  _POSIX_Message_queue_Notify_handler
 *
 */

void _POSIX_Message_queue_Notify_handler(
  void    *user_data
)
{
  POSIX_Message_queue_Control *the_mq;

  the_mq = user_data;

  /* XXX do something with signals here!!!! */
}

/*PAGE
 *
 *  15.2.6 Notify Process that a Message is Available on a Queue, 
 *         P1003.1b-1993, p. 280
 */

int mq_notify(
  mqd_t                  mqdes,
  const struct sigevent *notification
)
{
  register POSIX_Message_queue_Control *the_mq;
  Objects_Locations                     location;
 
  the_mq = _POSIX_Message_queue_Get( mqdes, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      seterrno( EBADF );
      return( -1 );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_LOCAL:
      if ( notification ) {
        if ( _CORE_message_queue_Is_notify_enabled( &the_mq->Message_queue ) ) {
          _Thread_Enable_dispatch();
          seterrno( EBUSY );
          return( -1 );
        }

        _CORE_message_queue_Set_notify( &the_mq->Message_queue, NULL, NULL );

        the_mq->notification = *notification;
      
        _CORE_message_queue_Set_notify(
          &the_mq->Message_queue,
          _POSIX_Message_queue_Notify_handler,
          the_mq
        );
      } else {

        _CORE_message_queue_Set_notify( &the_mq->Message_queue, NULL, NULL );

      }

      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  15.2.7 Set Message Queue Attributes, P1003.1b-1993, p. 281
 */

int mq_setattr(
  mqd_t                 mqdes,
  const struct mq_attr *mqstat,
  struct mq_attr       *omqstat
)
{
  register POSIX_Message_queue_Control *the_mq;
  Objects_Locations                     location;
  CORE_message_queue_Attributes        *the_mq_attr;
 
  the_mq = _POSIX_Message_queue_Get( mqdes, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_LOCAL:
      /*
       *  Return the old values.
       */

      /* XXX this is the same stuff as is in mq_getattr... and probably */
      /* XXX should be in an inlined private routine */

      the_mq_attr = &the_mq->Message_queue.Attributes;

      omqstat->mq_flags   = the_mq->flags;
      omqstat->mq_msgsize = the_mq->Message_queue.maximum_message_size;
      omqstat->mq_maxmsg  = the_mq->Message_queue.maximum_pending_messages;
      omqstat->mq_curmsgs = the_mq->Message_queue.number_of_pending_messages;
  
      /*
       *  Ignore everything except the O_NONBLOCK bit. 
       */

      if (  mqstat->mq_flags & O_NONBLOCK ) 
        the_mq->blocking = FALSE;
      else
        the_mq->blocking = TRUE;
 
      the_mq->flags = mqstat->mq_flags;

      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  15.2.8 Get Message Queue Attributes, P1003.1b-1993, p. 283
 */

int mq_getattr(
  mqd_t           mqdes,
  struct mq_attr *mqstat
)
{
  register POSIX_Message_queue_Control *the_mq;
  Objects_Locations                     location;
  CORE_message_queue_Attributes        *the_mq_attr;
 
  the_mq = _POSIX_Message_queue_Get( mqdes, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      seterrno( EINVAL );
      return( -1 );
    case OBJECTS_LOCAL:
      /*
       *  Return the old values.
       */
 
      /* XXX this is the same stuff as is in mq_setattr... and probably */
      /* XXX should be in an inlined private routine */
 
      the_mq_attr = &the_mq->Message_queue.Attributes;
 
      mqstat->mq_flags   = the_mq->flags;
      mqstat->mq_msgsize = the_mq->Message_queue.maximum_message_size;
      mqstat->mq_maxmsg  = the_mq->Message_queue.maximum_pending_messages;
      mqstat->mq_curmsgs = the_mq->Message_queue.number_of_pending_messages;
 
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
