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
 *  _POSIX_Message_queue_Name_to_id
 *
 *  Look up the specified name and attempt to locate the id
 *  for the associated message queue.
 */

int _POSIX_Message_queue_Name_to_id(
  const char          *name,
  Objects_Id          *id
)
{
  Objects_Name_to_id_errors  status;

   if ( !name )
     return EINVAL;

  if ( !name[0] )
    return EINVAL;

  if( strlen(name) > PATH_MAX )
    return ENAMETOOLONG;

  status = _Objects_Name_to_id( 
    &_POSIX_Message_queue_Information, (char *)name, 0, id );

  if ( status == OBJECTS_SUCCESSFUL )
    return 0;

  return ENOENT;
}
