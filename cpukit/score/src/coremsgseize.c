/*
 *  CORE Message Queue Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
 *  _CORE_message_queue_Seize
 *
 *  This kernel routine dequeues a message, copies the message buffer to
 *  a given destination buffer, and frees the message buffer to the
 *  inactive message pool.  The thread will be blocked if wait is TRUE,
 *  otherwise an error will be given to the thread if no messages are available.
 *
 *  Input parameters:
 *    the_message_queue - pointer to message queue
 *    id                - id of object we are waitig on
 *    buffer            - pointer to message buffer to be filled
 *    size              - pointer to the size of buffer to be filled
 *    wait              - TRUE if wait is allowed, FALSE otherwise
 *    timeout           - time to wait for a message
 *
 *  Output parameters:  NONE
 *
 *  NOTE: Dependent on BUFFER_LENGTH
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 */

void _CORE_message_queue_Seize(
  CORE_message_queue_Control *the_message_queue,
  Objects_Id                  id,
  void                       *buffer,
  unsigned32                 *size,
  boolean                     wait,
  Watchdog_Interval           timeout
)
{
  ISR_Level                          level;
  CORE_message_queue_Buffer_control *the_message;
  Thread_Control                    *executing;

  executing = _Thread_Executing;
  executing->Wait.return_code = CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
  _ISR_Disable( level );
  if ( the_message_queue->number_of_pending_messages != 0 ) {
    the_message_queue->number_of_pending_messages -= 1;

    the_message = _CORE_message_queue_Get_pending_message( the_message_queue );
    _ISR_Enable( level );
    *size = the_message->Contents.size;
    _CORE_message_queue_Copy_buffer(the_message->Contents.buffer,buffer,*size );
    _CORE_message_queue_Free_message_buffer(the_message_queue, the_message );
    return;
  }

  if ( !wait ) {
    _ISR_Enable( level );
    executing->Wait.return_code = CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  _Thread_queue_Enter_critical_section( &the_message_queue->Wait_queue );
  executing->Wait.queue              = &the_message_queue->Wait_queue;
  executing->Wait.id                 = id;
  executing->Wait.return_argument    = (void *)buffer;
  executing->Wait.return_argument_1  = (void *)size;
  _ISR_Enable( level );

  _Thread_queue_Enqueue( &the_message_queue->Wait_queue, timeout );
}

