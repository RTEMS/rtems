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
 *  COPYRIGHT (c) 1989-2009.
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

/*
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
  va_list                         arg;
  mode_t                          mode;
  struct mq_attr                 *attr = NULL;
  int                             status;
  Objects_Id                      the_mq_id;
  POSIX_Message_queue_Control    *the_mq;
  POSIX_Message_queue_Control_fd *the_mq_fd;
  Objects_Locations               location;
  size_t                          name_len;

  _Thread_Disable_dispatch();

  if ( oflag & O_CREAT ) {
    va_start(arg, oflag);
    mode = va_arg( arg, mode_t );
    attr = va_arg( arg, struct mq_attr * );
    va_end(arg);
  }

  the_mq_fd = _POSIX_Message_queue_Allocate_fd();
  if ( !the_mq_fd ) {
    _Thread_Enable_dispatch();
    rtems_set_errno_and_return_minus_one( ENFILE );
  }
  the_mq_fd->oflag = oflag;

  status = _POSIX_Message_queue_Name_to_id( name, &the_mq_id, &name_len );

  /*
   *  If the name to id translation worked, then the message queue exists
   *  and we can just return a pointer to the id.  Otherwise we may
   *  need to check to see if this is a "message queue does not exist"
   *  or some other miscellaneous error on the name.
   */
  if ( status ) {
    /*
     * Unless provided a valid name that did not already exist
     * and we are willing to create then it is an error.
     */
    if ( !( status == ENOENT && (oflag & O_CREAT) ) ) {
      _POSIX_Message_queue_Free_fd( the_mq_fd );
      _Thread_Enable_dispatch();
      rtems_set_errno_and_return_minus_one_cast( status, mqd_t );
    }

  } else {                /* name -> ID translation succeeded */
    /*
     * Check for existence with creation.
     */
    if ( (oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL) ) {
      _POSIX_Message_queue_Free_fd( the_mq_fd );
      _Thread_Enable_dispatch();
      rtems_set_errno_and_return_minus_one_cast( EEXIST, mqd_t );
    }

    /*
     * In this case we need to do an ID->pointer conversion to
     * check the mode.
     */
    the_mq = _POSIX_Message_queue_Get( the_mq_id, &location );
    the_mq->open_count += 1;
    the_mq_fd->Queue = the_mq;
    _Objects_Open_string(
      &_POSIX_Message_queue_Information_fds,
      &the_mq_fd->Object,
      NULL
    );
    _Thread_Enable_dispatch();
    _Thread_Enable_dispatch();
    return (mqd_t)the_mq_fd->Object.id;

  }

  /*
   *  At this point, the message queue does not exist and everything has been
   *  checked. We should go ahead and create a message queue.
   */
  status = _POSIX_Message_queue_Create_support(
    name,
    name_len,
    true,         /* shared across processes */
    attr,
    &the_mq
  );

  /*
   * errno was set by Create_support, so don't set it again.
   */
  if ( status == -1 ) {
    _POSIX_Message_queue_Free_fd( the_mq_fd );
    _Thread_Enable_dispatch();
    return (mqd_t) -1;
  }

  the_mq_fd->Queue = the_mq;
  _Objects_Open_string(
    &_POSIX_Message_queue_Information_fds,
    &the_mq_fd->Object,
    NULL
  );

  _Thread_Enable_dispatch();

  return (mqd_t) the_mq_fd->Object.id;
}
