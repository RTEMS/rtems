/**
 * @file
 *
 * @ingroup RTEMSScoreMessageQueue
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
 * @addtogroup RTEMSScoreMessageQueue
 *
 * @{
 */

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
 * @brief Initializes a message queue.
 *
 * This package is the implementation of the CORE Message Queue Handler.
 * This core object provides task synchronization and communication functions
 * via messages passed to queue objects.
 *
 * This routine initializes @a the_message_queue
 *        based on the parameters passed.
 *
 * @param[out] the_message_queue The message queue to initialize.
 * @param discipline The blocking discipline for the message queue.
 * @param maximum_pending_messages The maximum number of messages
 *        that will be allowed to pend at any given time.
 * @param maximum_message_size The size of the largest message that
 *        may be sent to this message queue instance.
 *
 * @retval true The message queue can be initialized.
 * @retval false Memory for the pending messages cannot be allocated.
 */
bool _CORE_message_queue_Initialize(
  CORE_message_queue_Control     *the_message_queue,
  CORE_message_queue_Disciplines  discipline,
  uint32_t                        maximum_pending_messages,
  size_t                          maximum_message_size
);

/**
 * @brief Closes a message queue.
 *
 * This package is the implementation of the CORE Message Queue Handler.
 * This core object provides task synchronization and communication functions
 * via messages passed to queue objects.
 *
 * This function closes a message by returning all allocated space and
 * flushing @a the_message_queue's task wait queue.
 *
 * @param[in, out] the_message_queue The message queue to close.
 * @param[in, out] queue_context The thread queue context used for
 *   _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 */
void _CORE_message_queue_Close(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
);

/**
 * @brief Flushes pending messages.
 *
 * This package is the implementation of the CORE Message Queue Handler.
 * This core object provides task synchronization and communication functions
 * via messages passed to queue objects.
 *
 * This function flushes @a the_message_queue's pending message queue.  The
 * number of messages flushed from the queue is returned.
 *
 * @param[in, out] the_message_queue The message queue to flush.
 * @param queue_context The thread queue context with interrupts disabled.
 *
 * @return This method returns the number of message pending messages flushed.
 */
uint32_t   _CORE_message_queue_Flush(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
);

#if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
/**
 * @brief Flushes waiting threads.
 *
 * This function flushes the threads which are blocked on
 * @a the_message_queue's pending message queue.  They are
 * unblocked whether blocked sending or receiving. It returns
 * the number of messages flushed from the queue.
 *
 * @param[in, out] the_message_queue The message queue to flush.
 *
 * @return This method returns the number of messages flushed from the queue.
 */
  void _CORE_message_queue_Flush_waiting_threads(
    CORE_message_queue_Control *the_message_queue
  );
#endif

/**
 * @brief Broadcasts a message to the message queue.
 *
 * This package is the implementation of the CORE Message Queue Handler.
 * This core object provides task synchronization and communication functions
 * via messages passed to queue objects.
 *
 * This function sends a message for every thread waiting on the queue and
 * returns the number of threads made ready by the message.
 *
 * @param[in, out] the_message_queue The message queue to operate upon.
 * @param buffer The starting address of the message to broadcast.
 * @param size The size of the message being broadcast.
 * @param[out] count The variable that will contain the
 *        number of tasks that are sent this message.
 * @param queue_context The thread queue context used for
 *   _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 *
 * @retval STATUS_SUCCESSFUL The message was successfully broadcast.
 * @retval STATUS_MESSAGE_INVALID_SIZE The message size was too big.
 */
Status_Control _CORE_message_queue_Broadcast(
  CORE_message_queue_Control *the_message_queue,
  const void                 *buffer,
  size_t                      size,
  uint32_t                   *count,
  Thread_queue_Context       *queue_context
);

/**
 * @brief Submits a message to the message queue.
 *
 * This routine implements the send and urgent message functions. It
 * processes a message that is to be submitted to the designated
 * message queue.  The message will either be processed as a
 * send message which it will be inserted at the rear of the queue
 * or it will be processed as an urgent message which will be inserted
 * at the front of the queue.
 *
 * @param[in, out] the_message_queue The message queue to operate upon.
 * @param executing The executing thread.
 * @param buffer The starting address of the message to send.
 * @param size The size of the message being send.
 * @param submit_type Determines whether the message is prepended,
 *        appended, or enqueued in priority order.
 * @param wait Indicates whether the calling thread is willing to block
 *        if the message queue is full.
 * @param queue_context The thread queue context used for
 *   _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 *
 * @retval STATUS_SUCCESSFUL The message was successfully submitted to the message queue.
 * @retval STATUS_MESSAGE_INVALID_SIZE The message size was too big.
 * @retval STATUS_TOO_MANY No message buffers were available.
 * @retval STATUS_MESSAGE_QUEUE_WAIT_IN_ISR The caller is in an ISR, do not block!
 * @retval STATUS_TIMEOUT A timeout occured.
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
 * @brief Seizes a message from the message queue.
 *
 * This package is the implementation of the CORE Message Queue Handler.
 * This core object provides task synchronization and communication functions
 * via messages passed to queue objects.
 *
 * This kernel routine dequeues a message, copies the message buffer to
 * a given destination buffer, and frees the message buffer to the
 * inactive message pool.  The thread will be blocked if wait is true,
 * otherwise an error will be given to the thread if no messages are available.
 *
 * @param[in, out] the_message_queue The message queue to seize a message from.
 * @param executing The executing thread.
 * @param[out] buffer The starting address of the message buffer to
 *        to be filled in with a message.
 * @param[out] size_p The size of the @a buffer,
 *        indicates the maximum size message that the caller can receive.
 * @param wait Indicates whether the calling thread is willing to block
 *        if the message queue is empty.
 * @param queue_context The thread queue context used for
 *   _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 *
 * @retval STATUS_SUCCESSFUL The message was successfully seized from the message queue.
 * @retval STATUS_UNSATISFIED Wait was set to false and there is currently no pending message.
 * @retval STATUS_TIMEOUT A timeout occured.
 *
 * @note Returns message priority via return area in TCB.
 *
 * - INTERRUPT LATENCY:
 *   + available
 *   + wait
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
 * @brief Inserts a message into the message queue.
 *
 * Copies the specified content into the message storage space and then
 * inserts the message into the message queue according to the submit type.
 *
 * @param[in, out] the_message_queue The message queue to insert a message in.
 * @param[in, out] the_message The message to insert in the message queue.
 * @param content_source The message content source.
 * @param content_size The message content size in bytes.
 * @param submit_type Determines whether the message is prepended,
 *        appended, or enqueued in priority order.
 */
void _CORE_message_queue_Insert_message(
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message,
  const void                        *content_source,
  size_t                             content_size,
  CORE_message_queue_Submit_types    submit_type
);

/**
 * @brief Sends a message to the message queue.
 *
 * @param[in, out] the_message_queue The message queue to send a message to.
 * @param buffer The starting address of the message to send.
 * @param sizeis The size of the message being send.
 * @param wait Indicates whether the calling thread is willing to block
 *        if the message queue is full.
 * @param queue_context The thread queue context used for
 *   _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 *
 * @retval STATUS_SUCCESSFUL The message was successfully submitted to the message queue.
 * @retval STATUS_MESSAGE_INVALID_SIZE The message size was too big.
 * @retval STATUS_TOO_MANY No message buffers were available.
 * @retval STATUS_MESSAGE_QUEUE_WAIT_IN_ISR The caller is in an ISR, do not block!
 * @retval STATUS_TIMEOUT A timeout occured.
 */
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

/**
 * @brief Sends an urgent message to the message queue.
 *
 * @param[in, out] the_message_queue The message queue to send an urgent message to.
 * @param buffer The starting address of the message to send.
 * @param sizeis The size of the message being send.
 * @param wait Indicates whether the calling thread is willing to block
 *        if the message queue is full.
 * @param queue_context The thread queue context used for
 *   _CORE_message_queue_Acquire() or _CORE_message_queue_Acquire_critical().
 *
 * @retval STATUS_SUCCESSFUL The message was successfully submitted to the message queue.
 * @retval STATUS_MESSAGE_INVALID_SIZE The message size was too big.
 * @retval STATUS_TOO_MANY No message buffers were available.
 * @retval STATUS_MESSAGE_QUEUE_WAIT_IN_ISR The caller is in an ISR, do not block!
 * @retval STATUS_TIMEOUT A timeout occured.
 */
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

/**
 * @brief Acquires the message queue.
 *
 * @param[in, out] the_message_queue Rhe message queue to acquire.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Acquire(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
)
{
  _Thread_queue_Acquire( &the_message_queue->Wait_queue, queue_context );
}

/**
 * @brief Acquires the message queue critical.
 *
 * @param[in, out] the_message_queue The message queue to acquire critical.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Acquire_critical(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
)
{
  _Thread_queue_Acquire_critical( &the_message_queue->Wait_queue, queue_context );
}

/**
 * @brief Releases the message queue.
 *
 * @param[in, out] the_message_queue The message queue to release.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Release(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
)
{
  _Thread_queue_Release( &the_message_queue->Wait_queue, queue_context );
}

/**
 * @brief Copies the source message buffer to the destination message buffer.
 *
 * This routine copies the contents of the source message buffer
 * to the destination message buffer.
 *
 * @param source The source message buffer to be copied.
 * @param[out] destination The destination messag buffer to copy the source to.
 * @param size The size of the source buffer.
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
 * @brief Allocates a message buffer from the inactive message buffer chain.
 *
 * This function allocates a message buffer from the inactive
 * message buffer chain.
 *
 * @param the_message_queue The message queue to operate upon.
 *
 * @retval pointer The allocated message buffer.
 * @retval NULL The inactive message buffer chain is empty.
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
 * @brief Frees a message buffer to inactive message buffer chain.
 *
 * This routine frees a message buffer to the inactive
 * message buffer chain.
 *
 * @param[in, out] the_message_queue The message queue to free the message buffer to.
 * @param[out] the_message The message to be freed.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Free_message_buffer (
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message
)
{
  _Chain_Append_unprotected( &the_message_queue->Inactive_messages, &the_message->Node );
}

/**
 * @brief Gets message priority.
 *
 * This function returns the priority of @a the_message.
 *
 * @param the_message The message to obtain the priority from.
 *
 * @retval priority The priority of this message.
 * @retval 0 Message priority is disabled.
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
 * @brief Gets first message of message queue and removes it.
 *
 * This function removes the first message from the_message_queue
 * and returns a pointer to it.
 *
 * @param[in, out] the_message_queue The message queue to get the first message from.
 *
 * @retval pointer The first message if the message queue is not empty.
 * @retval NULL The message queue is empty.
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
   * @brief Checks if notification is enabled.
   *
   * This function returns true if notification is enabled on this message
   * queue and false otherwise.
   *
   * @param the_message_queue The message queue to check if the notification is enabled.
   *
   * @retval true Notification is enabled on this message queue.
   * @retval false Notification is not enabled on this message queue.
   */
  RTEMS_INLINE_ROUTINE bool _CORE_message_queue_Is_notify_enabled (
    CORE_message_queue_Control *the_message_queue
  )
  {
    return (the_message_queue->notify_handler != NULL);
  }
#endif

/**
 * @brief Initializes notification information.
 *
 * This routine initializes the notification information for
 * @a the_message_queue.
 *
 * @param[out] the_message_queue The message queue to initialize the notification information.
 * @param[out] the_handler The notification information for the message queue.
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

/**
 * @brief Gets the first locked thread waiting to receive and dequeues it.
 *
 * This method dequeues the first locked thread waiting to receive a message,
 *      dequeues it and returns the corresponding Thread_Control.
 *
 * @param[in, out] the_message_queue The message queue to operate upon.
 * @param buffer The buffer that is copied to the threads mutable_object.
 * @param size The size of the buffer.
 * @param submit_type Indicates whether the thread should be willing to block in the future.
 * @param queue_context The thread queue context.
 *
 * @retval thread The Thread_Control for the first locked thread, if there is a locked thread.
 * @retval NULL There are pending messages or no thread waiting to receive.
 */
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
