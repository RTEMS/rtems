/*
 *  CORE Message Queue Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects.
 *
 *  COPYRIGHT (c) 1989-2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

/*
 *  _CORE_message_queue_Insert_message
 *
 *  This kernel routine inserts the specified message into the
 *  message queue.  It is assumed that the message has been filled
 *  in before this routine is called.
 *
 *  Input parameters:
 *    the_message_queue - pointer to message queue
 *    the_message       - message to insert
 *    priority          - insert indication
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    insert
 */

void _CORE_message_queue_Insert_message(
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message,
  CORE_message_queue_Submit_types    submit_type
)
{
  ISR_Level  level;
  #if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
    bool    notify = false;
    #define SET_NOTIFY() \
      do { \
        if ( the_message_queue->number_of_pending_messages == 0 ) \
          notify = true; \
      } while (0)
  #else
    #define SET_NOTIFY()
  #endif

  _CORE_message_queue_Set_message_priority( the_message, submit_type );

  #if !defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
    _ISR_Disable( level );
      SET_NOTIFY();
      the_message_queue->number_of_pending_messages++;
      if ( submit_type == CORE_MESSAGE_QUEUE_SEND_REQUEST )
        _CORE_message_queue_Append_unprotected(the_message_queue, the_message);
      else
        _CORE_message_queue_Prepend_unprotected(the_message_queue, the_message);
    _ISR_Enable( level );
  #else
    if ( submit_type == CORE_MESSAGE_QUEUE_SEND_REQUEST ) {
      _ISR_Disable( level );
        SET_NOTIFY();
        the_message_queue->number_of_pending_messages++;
        _CORE_message_queue_Append_unprotected(the_message_queue, the_message);
      _ISR_Enable( level );
    } else if ( submit_type == CORE_MESSAGE_QUEUE_URGENT_REQUEST ) {
      _ISR_Disable( level );
        SET_NOTIFY();
        the_message_queue->number_of_pending_messages++;
        _CORE_message_queue_Prepend_unprotected(the_message_queue, the_message);
      _ISR_Enable( level );
    } else {
      CORE_message_queue_Buffer_control *this_message;
      Chain_Node                        *the_node;
      Chain_Control                     *the_header;
      int                                the_priority;

      the_priority = _CORE_message_queue_Get_message_priority(the_message);
      the_header = &the_message_queue->Pending_messages;
      the_node = _Chain_First( the_header );
      while ( !_Chain_Is_tail( the_header, the_node ) ) {
        int this_priority;

        this_message = (CORE_message_queue_Buffer_control *) the_node;

        this_priority = _CORE_message_queue_Get_message_priority(this_message);

        if ( this_priority <= the_priority ) {
          the_node = the_node->next;
          continue;
        }
        break;
      }
      _ISR_Disable( level );
        SET_NOTIFY();
        the_message_queue->number_of_pending_messages++;
        _Chain_Insert_unprotected( the_node->previous, &the_message->Node );
      _ISR_Enable( level );
    }
  #endif

  #if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
    /*
     *  According to POSIX, does this happen before or after the message
     *  is actually enqueued.  It is logical to think afterwards, because
     *  the message is actually in the queue at this point.
     */
    if ( notify && the_message_queue->notify_handler )
      (*the_message_queue->notify_handler)(the_message_queue->notify_argument);
  #endif
}
