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

