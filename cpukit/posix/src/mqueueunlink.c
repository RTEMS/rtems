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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>

#include <rtems/system.h>
#include <rtems/score/watchdog.h>
#include <rtems/seterr.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  15.2.2 Remove a Message Queue, P1003.1b-1993, p. 276
 */

int mq_unlink(
  const char *name
)
{
  int                                   status;
  register POSIX_Message_queue_Control *the_mq;
  Objects_Id                            the_mq_id;
 
  _Thread_Disable_dispatch();

  status = _POSIX_Message_queue_Name_to_id( name, &the_mq_id );
   if ( status != 0 ) {
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( status );
   }

  /*
   *  Don't support unlinking a remote message queue.
   */

  if ( !_Objects_Is_local_id(the_mq_id) ) {
    _Thread_Enable_dispatch();
    set_errno_and_return_minus_one( ENOSYS );
  }

  the_mq = (POSIX_Message_queue_Control *) _Objects_Get_local_object(
    &_POSIX_Message_queue_Information,
    _Objects_Get_index( the_mq_id )
  );
  
#if 0 && defined(RTEMS_MULTIPROCESSING)
  if ( the_mq->process_shared == PTHREAD_PROCESS_SHARED ) {
    _Objects_MP_Close( &_POSIX_Message_queue_Information, the_mq_id );
  }
#endif

 
  the_mq->linked = FALSE;
  _POSIX_Message_queue_Namespace_remove( the_mq );
  _POSIX_Message_queue_Delete( the_mq );

  _Thread_Enable_dispatch();
  return 0;
}

