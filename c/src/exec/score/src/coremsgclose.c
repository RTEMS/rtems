/*
 *  CORE Message Queue Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif

/*PAGE
 *
 *  _CORE_message_queue_Close
 *
 *  This function closes a message by returning all allocated space and
 *  flushing the message_queue's task wait queue.
 *
 *  Input parameters:
 *    the_message_queue      - the message_queue to be flushed
 *    remote_extract_callout - function to invoke remotely
 *    status                 - status to pass to thread
 *
 *  Output parameters:  NONE
 */
 
void _CORE_message_queue_Close(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Flush_callout  remote_extract_callout,
  unsigned32                  status
)
{
 
  if ( the_message_queue->number_of_pending_messages != 0 )
    (void) _CORE_message_queue_Flush_support( the_message_queue );
  else
    _Thread_queue_Flush(
      &the_message_queue->Wait_queue,
      remote_extract_callout,
      status
    );

  (void) _Workspace_Free( the_message_queue->message_buffers );

}

