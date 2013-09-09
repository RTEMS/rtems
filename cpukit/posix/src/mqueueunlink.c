/**
 * @file
 *
 * @brief Remove a Message Queue
 * @ingroup POSIX_MQUEUE Message Queues
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/posix/mqueueimpl.h>
#include <rtems/posix/time.h>

/*
 *  15.2.2 Remove a Message Queue, P1003.1b-1993, p. 276
 */

int mq_unlink(
  const char *name
)
{
  int                                   status;
  register POSIX_Message_queue_Control *the_mq;
  Objects_Id                            the_mq_id;
  size_t                                name_len;

  _Thread_Disable_dispatch();

  status = _POSIX_Message_queue_Name_to_id( name, &the_mq_id, &name_len );
   if ( status != 0 ) {
    _Thread_Enable_dispatch();
    rtems_set_errno_and_return_minus_one( status );
   }

  the_mq = (POSIX_Message_queue_Control *) _Objects_Get_local_object(
    &_POSIX_Message_queue_Information,
    _Objects_Get_index( the_mq_id )
  );

  the_mq->linked = false;
  _POSIX_Message_queue_Namespace_remove( the_mq );
  _POSIX_Message_queue_Delete( the_mq );

  _Thread_Enable_dispatch();
  return 0;
}
