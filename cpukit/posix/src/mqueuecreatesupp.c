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
#include <rtems/posix/seterr.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  _POSIX_Message_queue_Create_support
 */
 
int _POSIX_Message_queue_Create_support(
  const char                    *name,
  int                            pshared,
  unsigned int                   oflag,
  struct mq_attr                *attr_ptr,
  POSIX_Message_queue_Control  **message_queue
)
{
  POSIX_Message_queue_Control   *the_mq;
  CORE_message_queue_Attributes *the_mq_attr;
  struct mq_attr                 attr;

  _Thread_Disable_dispatch();
 
  if ( attr_ptr == NULL ) {
    attr.mq_maxmsg  = 0;  /* XXX */
    attr.mq_msgsize = 0;  /* XXX */
  } else {
    if ( attr_ptr->mq_maxmsg < 0 ){
      _Thread_Enable_dispatch();
      set_errno_and_return_minus_one( EINVAL );
    }

    if ( attr_ptr->mq_msgsize < 0 ){
      _Thread_Enable_dispatch();
      set_errno_and_return_minus_one( EINVAL );
    }

    attr = *attr_ptr;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( pshared == PTHREAD_PROCESS_SHARED &&
       !( _Objects_MP_Allocate_and_open( &_POSIX_Message_queue_Information, 0,
                            the_mq->Object.id, FALSE ) ) ) {
    _POSIX_Message_queue_Free( the_mq );
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( ENFILE );
  }
#endif
 
  if ( name ) {

    if( strlen(name) > PATH_MAX ) {  /* XXX - Is strlen ok to use here ? */
      _Thread_Enable_dispatch();
      set_errno_and_return_minus_one( ENAMETOOLONG );
    }

    /*
     * XXX Greater than NAME_MAX while POSIX_NO_TRUNC in effect.
     * XXX Error description in POSIX book different for mq_open and mq_unlink
     *     Why???
     */
  }

  the_mq = _POSIX_Message_queue_Allocate();
  if ( !the_mq ) {
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( ENFILE );
  }
 
  the_mq->process_shared  = pshared;
 
  if ( name ) {
    the_mq->named = TRUE;
    the_mq->open_count = 1;
    the_mq->linked = TRUE;
 }
  else {
    the_mq->named = FALSE;

  }

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
  the_mq_attr = &the_mq->Message_queue.Attributes;

  if ( ! _CORE_message_queue_Initialize(
           &the_mq->Message_queue,
           OBJECTS_POSIX_MESSAGE_QUEUES,
           the_mq_attr,
           attr.mq_maxmsg,
           attr.mq_msgsize,
#if defined(RTEMS_MULTIPROCESSING)
           _POSIX_Message_queue_MP_Send_extract_proxy
#else
           NULL
#endif
      ) ) {

#if defined(RTEMS_MULTIPROCESSING)
    if ( pshared == PTHREAD_PROCESS_SHARED )
      _Objects_MP_Close( &_POSIX_Message_queue_Information, the_mq->Object.id );
#endif
 
    _POSIX_Message_queue_Free( the_mq );
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( ENOSPC );
  }

 
  /* XXX - need Names to be a string!!! */
  _Objects_Open(
    &_POSIX_Message_queue_Information,
    &the_mq->Object,
    (char *) name
  );
 
  *message_queue = the_mq;
 
#if defined(RTEMS_MULTIPROCESSING)
  if ( pshared == PTHREAD_PROCESS_SHARED )
    _POSIX_Message_queue_MP_Send_process_packet(
      POSIX_MESSAGE_QUEUE_MP_ANNOUNCE_CREATE,
      the_mq->Object.id,
      (char *) name,
      0                          /* Not used */
    );
#endif
 
  _Thread_Enable_dispatch();
  return 0;
}





