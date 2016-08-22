/**
 * @file
 *
 * @brief Inlined Routines in the Core Message Handler
 *
 * This include file contains the static inline implementation of all
 * inlined routines in the Core Message Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREMSGIMPL_H
#define _RTEMS_SCORE_COREMSGIMPL_H

#include <rtems/score/coremsg.h>
#include <rtems/score/status.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadqimpl.h>

#include <limits.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreMessageQueue
 */
/**@{**/

/**
 *  @brief Used when appending messages onto a message queue.
 *
 *  This is the priority constant used when appending messages onto
 *  a message queue.
 */
#define  CORE_MESSAGE_QUEUE_SEND_REQUEST   INT_MAX

/**
 *  @brief Used when prepending messages onto a message queue.
 *
 *  This is the priority constant used when prepending messages onto
 *  a message queue.
 */
#define  CORE_MESSAGE_QUEUE_URGENT_REQUEST INT_MIN

/**
 *  @brief The modes in which a message may be submitted to a message queue.
 *
 *  The following type details the modes in which a message
 *  may be submitted to a message queue.  The message may be posted
 *  in a send or urgent fashion.
 *
 *  @note  All other values are message priorities.  Numerically smaller
 *         priorities indicate higher priority messages.
 */
typedef int CORE_message_queue_Submit_types;

/**
 *  @brief Initialize a message queue.
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects.
 *
 *  This routine initializes @a the_message_queue
 *      based on the parameters passed.
 *
 *  @param[in] the_message_queue points to the message queue to initialize
 *  @param[in] discipline the blocking discipline
 *  @param[in] maximum_pending_messages is the maximum number of messages
 *         that will be allowed to pend at any given time
 *  @param[in] maximum_message_size is the size of largest message that
 *         may be sent to this message queue instance
 *
 *  @retval true if the message queue can be initialized.  In general,
 *         false will only be returned if memory for the pending
 *         messages cannot be allocated.
 */
bool _CORE_message_queue_Initialize(
  CORE_message_queue_Control     *the_message_queue,
  CORE_message_queue_Disciplines  discipline,
  uint32_t                        maximum_pending_messages,
  size_t                          maximum_message_size
);

/**
 *  @brief Close a message queue.
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects
 *
 *  This function closes a message by returning all allocated space and
 *  flushing @a the_message_queue's task wait queue.
 *
 *  @param[in] the_message_queue points to the message queue to close
 *  @param[in] queue_context The thread queue context used for
 *    _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 */
void _CORE_message_queue_Close(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
);

/**
 *  @brief Flush pending messages.
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects.
 *
 *  This function flushes @a the_message_queue's pending message queue.  The
 *  number of messages flushed from the queue is returned.
 *
 *  @param[in] the_message_queue points to the message queue to flush
 *  @param[in] queue_context The thread queue context with interrupts disabled.
 *
 *  @retval This method returns the number of message pending messages flushed.
 */
uint32_t   _CORE_message_queue_Flush(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
);

#if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
/**
 *  @brief Flush waiting threads.
 *
 *  This function flushes the threads which are blocked on
 *  @a the_message_queue's pending message queue.  They are
 *  unblocked whether blocked sending or receiving. It returns
 *  the number of messages flushed from the queue.
 *
 *  @param[in] the_message_queue points to the message queue to flush
 *  @retval number of messages flushed from the queue
 */
  void _CORE_message_queue_Flush_waiting_threads(
    CORE_message_queue_Control *the_message_queue
  );
#endif

/**
 *  @brief Broadcast a message to the message queue.
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects.
 *
 *  This function sends a message for every thread waiting on the queue and
 *  returns the number of threads made ready by the message.
 *
 *  @param[in] the_message_queue points to the message queue
 *  @param[in] buffer is the starting address of the message to broadcast
 *  @param[in] size is the size of the message being broadcast
 *  @param[out] count points to the variable that will contain the
 *         number of tasks that are sent this message
 *  @param[in] queue_context The thread queue context used for
 *    _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 *  @retval @a *count will contain the number of messages sent
 *  @retval indication of the successful completion or reason for failure
 */
Status_Control _CORE_message_queue_Broadcast(
  CORE_message_queue_Control *the_message_queue,
  const void                 *buffer,
  size_t                      size,
  uint32_t                   *count,
  Thread_queue_Context       *queue_context
);

/**
 *  @brief Submit a message to the message queue.
 *
 *  This routine implements the send and urgent message functions. It
 *  processes a message that is to be submitted to the designated
 *  message queue.  The message will either be processed as a
 *  send message which it will be inserted at the rear of the queue
 *  or it will be processed as an urgent message which will be inserted
 *  at the front of the queue.
 *
 *  @param[in] the_message_queue points to the message queue
 *  @param[in] buffer is the starting address of the message to send
 *  @param[in] size is the size of the message being send
 *  @param[in] submit_type determines whether the message is prepended,
 *         appended, or enqueued in priority order.
 *  @param[in] wait indicates whether the calling thread is willing to block
 *         if the message queue is full.
 *  @param[in] queue_context The thread queue context used for
 *    _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 *  @retval indication of the successful completion or reason for failure
 */
Status_Control _CORE_message_queue_Submit(
  CORE_message_queue_Control       *the_message_queue,
  Thread_Control                   *executing,
  const void                       *buffer,
  size_t                            size,
  CORE_message_queue_Submit_types   submit_type,
  bool                              wait,
  Thread_queue_Context             *queue_context
);

/**
 *  @brief Size a message from the message queue.
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects.
 *
 *  This kernel routine dequeues a message, copies the message buffer to
 *  a given destination buffer, and frees the message buffer to the
 *  inactive message pool.  The thread will be blocked if wait is true,
 *  otherwise an error will be given to the thread if no messages are available.
 *
 *  @param[in] the_message_queue points to the message queue
 *  @param[in] buffer is the starting address of the message buffer to
 *         to be filled in with a message
 *  @param[in] size_p is a pointer to the size of the @a buffer and
 *         indicates the maximum size message that the caller can receive.
 *  @param[in] wait indicates whether the calling thread is willing to block
 *         if the message queue is empty.
 *  @param[in] queue_context The thread queue context used for
 *    _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 *
 *  @retval indication of the successful completion or reason for failure.
 *          On success, the location pointed to @a size_p will contain the
 *          size of the received message.
 *
 *  @note Returns message priority via return area in TCB.
 *
 *  - INTERRUPT LATENCY:
 *    + available
 *    + wait
 */
Status_Control _CORE_message_queue_Seize(
  CORE_message_queue_Control *the_message_queue,
  Thread_Control             *executing,
  void                       *buffer,
  size_t                     *size_p,
  bool                        wait,
  Thread_queue_Context       *queue_context
);

/**
 *  @brief Insert a message into the message queue.
 *
 *  Copies the specified content into the message storage space and then
 *  inserts the message into the message queue according to the submit type.
 *
 *  @param[in] the_message_queue points to the message queue
 *  @param[in] the_message is the message to enqueue
 *  @param[in] content_source the message content source
 *  @param[in] content_size the message content size in bytes
 *  @param[in] submit_type determines whether the message is prepended,
 *         appended, or enqueued in priority order.
 */
void _CORE_message_queue_Insert_message(
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message,
  const void                        *content_source,
  size_t                             content_size,
  CORE_message_queue_Submit_types    submit_type
);

RTEMS_INLINE_ROUTINE Status_Control _CORE_message_queue_Send(
  CORE_message_queue_Control       *the_message_queue,
  const void                       *buffer,
  size_t                            size,
  bool                              wait,
  Thread_queue_Context             *queue_context
)
{
  return _CORE_message_queue_Submit(
    the_message_queue,
    _Thread_Executing,
    buffer,
    size,
    CORE_MESSAGE_QUEUE_SEND_REQUEST,
    wait,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE Status_Control _CORE_message_queue_Urgent(
  CORE_message_queue_Control       *the_message_queue,
  const void                       *buffer,
  size_t                            size,
  bool                              wait,
  Thread_queue_Context             *queue_context
)
{
  return _CORE_message_queue_Submit(
    the_message_queue,
    _Thread_Executing,
    buffer,
    size,
    CORE_MESSAGE_QUEUE_URGENT_REQUEST,
    wait,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE void _CORE_message_queue_Acquire(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
)
{
  _Thread_queue_Acquire( &the_message_queue->Wait_queue, queue_context );
}

RTEMS_INLINE_ROUTINE void _CORE_message_queue_Acquire_critical(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
)
{
  _Thread_queue_Acquire_critical( &the_message_queue->Wait_queue, queue_context );
}

RTEMS_INLINE_ROUTINE void _CORE_message_queue_Release(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
)
{
  _Thread_queue_Release( &the_message_queue->Wait_queue, queue_context );
}

/**
 * This routine copies the contents of the source message buffer
 * to the destination message buffer.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Copy_buffer (
  const void *source,
  void       *destination,
  size_t      size
)
{
  memcpy(destination, source, size);
}

/**
 * This function allocates a message buffer from the inactive
 * message buffer chain.
 */
RTEMS_INLINE_ROUTINE CORE_message_queue_Buffer_control *
_CORE_message_queue_Allocate_message_buffer (
    CORE_message_queue_Control *the_message_queue
)
{
   return (CORE_message_queue_Buffer_control *)
     _Chain_Get_unprotected( &the_message_queue->Inactive_messages );
}

/**
 * This routine frees a message buffer to the inactive
 * message buffer chain.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Free_message_buffer (
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message
)
{
  _Chain_Append_unprotected( &the_message_queue->Inactive_messages, &the_message->Node );
}

/**
 * This function returns the priority of @a the_message.
 *
 * @note It encapsulates the optional behavior that message priority is
 *       disabled if no API requires it.
 */
RTEMS_INLINE_ROUTINE int _CORE_message_queue_Get_message_priority (
  const CORE_message_queue_Buffer_control *the_message
)
{
  #if defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
    return the_message->priority;
  #else
    return 0;
  #endif
}

/**
 * This function removes the first message from the_message_queue
 * and returns a pointer to it.
 */
RTEMS_INLINE_ROUTINE
  CORE_message_queue_Buffer_control *_CORE_message_queue_Get_pending_message (
  CORE_message_queue_Control *the_message_queue
)
{
  return (CORE_message_queue_Buffer_control *)
    _Chain_Get_unprotected( &the_message_queue->Pending_messages );
}

#if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
  /**
   * This function returns true if notification is enabled on this message
   * queue and false otherwise.
   */
  RTEMS_INLINE_ROUTINE bool _CORE_message_queue_Is_notify_enabled (
    CORE_message_queue_Control *the_message_queue
  )
  {
    return (the_message_queue->notify_handler != NULL);
  }
#endif

/**
 * This routine initializes the notification information for
 * @a the_message_queue.
 */
#if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
  RTEMS_INLINE_ROUTINE void _CORE_message_queue_Set_notify (
    CORE_message_queue_Control        *the_message_queue,
    CORE_message_queue_Notify_Handler  the_handler
  )
  {
    the_message_queue->notify_handler = the_handler;
  }
#else
  /* turn it into nothing if not enabled */
  #define _CORE_message_queue_Set_notify( the_message_queue, the_handler ) \
    do { } while ( 0 )
#endif

RTEMS_INLINE_ROUTINE Thread_Control *_CORE_message_queue_Dequeue_receiver(
  CORE_message_queue_Control      *the_message_queue,
  const void                      *buffer,
  size_t                           size,
  CORE_message_queue_Submit_types  submit_type,
  Thread_queue_Context            *queue_context
)
{
  Thread_Control *the_thread;

  /*
   *  If there are pending messages, then there can't be threads
   *  waiting for us to send them a message.
   *
   *  NOTE: This check is critical because threads can block on
   *        send and receive and this ensures that we are broadcasting
   *        the message to threads waiting to receive -- not to send.
   */
  if ( the_message_queue->number_of_pending_messages != 0 ) {
    return NULL;
  }

  /*
   *  There must be no pending messages if there is a thread waiting to
   *  receive a message.
   */
  the_thread = _Thread_queue_First_locked(
    &the_message_queue->Wait_queue,
    the_message_queue->operations
  );
  if ( the_thread == NULL ) {
    return NULL;
  }

   *(size_t *) the_thread->Wait.return_argument = size;
   the_thread->Wait.count = (uint32_t) submit_type;

  _CORE_message_queue_Copy_buffer(
    buffer,
    the_thread->Wait.return_argument_second.mutable_object,
    size
  );

  _Thread_queue_Extract_critical(
    &the_message_queue->Wait_queue.Queue,
    the_message_queue->operations,
    the_thread,
    queue_context
  );

  return the_thread;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
