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
#include <rtems/score/wkspace.h>
#include <rtems/seterr.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/time.h>
#if defined(RTEMS_DEBUG)
  #include <rtems/bspIo.h>
#endif

/*
 *  _POSIX_Message_queue_Delete
 */

void _POSIX_Message_queue_Delete(
  POSIX_Message_queue_Control *the_mq
)
{
  if ( !the_mq->linked && !the_mq->open_count ) {
      Objects_Control *the_object = &the_mq->Object;

      #if defined(RTEMS_DEBUG)
        /*
         *  the name memory will have been freed by unlink.
         */
	if ( the_object->name.name_p ) {
          printk(
            "POSIX MQ name (%p) not freed by unlink\n",
	    (void *)the_object->name.name_p
          );
	  _Workspace_Free( (void *)the_object->name.name_p );
        }
      #endif

      _Objects_Close( &_POSIX_Message_queue_Information, the_object );

      _CORE_message_queue_Close(
        &the_mq->Message_queue,
        NULL,        /* no MP support */
        CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED
      );

    _POSIX_Message_queue_Free( the_mq );

  }
}
