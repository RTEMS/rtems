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
    &_POSIX_Message_queue_Information, /* object information table */
    OBJECTS_POSIX_API,                 /* object API */
    OBJECTS_POSIX_MESSAGE_QUEUES,      /* object class */
    maximum_message_queues,            /* maximum objects of this class */
    sizeof( POSIX_Message_queue_Control ),
                                /* size of this object's control block */
    FALSE,                      /* TRUE if names for this object are strings */
    _POSIX_PATH_MAX             /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                      /* TRUE if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );
  _Objects_Initialize_information(
    &_POSIX_Message_queue_Information_fds,
    OBJECTS_POSIX_API,
    OBJECTS_POSIX_MESSAGE_QUEUE_FDS,
    maximum_message_queues,
    sizeof( POSIX_Message_queue_Control_fd ),
                                /* size of this object's control block */
    FALSE,                      /* TRUE if names for this object are strings */
    0                           /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                      /* TRUE if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );
}
