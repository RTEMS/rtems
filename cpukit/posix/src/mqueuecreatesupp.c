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

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>

#include <rtems/system.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/wkspace.h>
#include <rtems/seterr.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/time.h>

/*
 *  _POSIX_Message_queue_Create_support
 *
 *  This routine does the actual creation and initialization of
 *  a poxix message queue.
 */

int _POSIX_Message_queue_Create_support(
  const char                    *name_arg,
  size_t                         name_len,
  int                            pshared,
  struct mq_attr                *attr_ptr,
  POSIX_Message_queue_Control  **message_queue
)
{
  POSIX_Message_queue_Control   *the_mq;
  CORE_message_queue_Attributes *the_mq_attr;
  struct mq_attr                 attr;
  char                          *name;

  /* length of name has already been validated */

  _Thread_Disable_dispatch();

  /*
   *  There is no real basis for the default values.  They will work
   *  but were not compared against any existing implementation for
   *  compatibility.  See README.mqueue for an example program we
   *  think will print out the defaults.  Report anything you find with it.
   */
  if ( attr_ptr == NULL ) {
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = 16;
  } else {
    if ( attr_ptr->mq_maxmsg <= 0 ){
      _Thread_Enable_dispatch();
      rtems_set_errno_and_return_minus_one( EINVAL );
    }

    if ( attr_ptr->mq_msgsize <= 0 ){
      _Thread_Enable_dispatch();
      rtems_set_errno_and_return_minus_one( EINVAL );
    }

    attr = *attr_ptr;
  }

  the_mq = _POSIX_Message_queue_Allocate();
  if ( !the_mq ) {
    _Thread_Enable_dispatch();
    rtems_set_errno_and_return_minus_one( ENFILE );
  }

  /*
   * Make a copy of the user's string for name just in case it was
   * dynamically constructed.
   */
  name = _Workspace_String_duplicate( name_arg, name_len );
  if ( !name ) {
    _POSIX_Message_queue_Free( the_mq );
    _Thread_Enable_dispatch();
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  the_mq->process_shared  = pshared;
  the_mq->named = true;
  the_mq->open_count = 1;
  the_mq->linked = true;

  /*
   *  NOTE: That thread blocking discipline should be based on the
   *  current scheduling policy.
   *
   *  Joel: Cite POSIX or OpenGroup on above statement so we can determine
   *        if it is a real requirement.
   */
  the_mq_attr = &the_mq->Message_queue.Attributes;
  the_mq_attr->discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;

  if ( !_CORE_message_queue_Initialize(
           &the_mq->Message_queue,
           the_mq_attr,
           attr.mq_maxmsg,
           attr.mq_msgsize
      ) ) {

    _POSIX_Message_queue_Free( the_mq );
    _Workspace_Free(name);
    _Thread_Enable_dispatch();
    rtems_set_errno_and_return_minus_one( ENOSPC );
  }

  _Objects_Open_string(
    &_POSIX_Message_queue_Information,
    &the_mq->Object,
    name
  );

  *message_queue = the_mq;

  _Thread_Enable_dispatch();
  return 0;
}
