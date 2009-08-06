/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

/* pure ANSI mode does not have this prototype */
size_t strnlen(const char *, size_t);

/*
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
  Objects_Name_or_id_lookup_errors  status;
  Objects_Id                        the_id;

   if ( !name )
     return EINVAL;

  if ( !name[0] )
    return EINVAL;

  if ( strnlen( name, NAME_MAX ) >= NAME_MAX )
    return ENAMETOOLONG;

  status = _Objects_Name_to_id_string(
    &_POSIX_Message_queue_Information,
    name,
    &the_id
  );
  *id = the_id;

  if ( status == OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL )
    return 0;

  return ENOENT;
}
