/** 
 *  @file  rtems/score/coremsg.inl
 *
 *  This include file contains the static inline implementation of all
 *  inlined routines in the Core Message Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREMSG_H
# error "Never use <rtems/score/coremsg.inl> directly; include <rtems/score/coremsg.h> instead."
#endif

#ifndef _RTEMS_SCORE_COREMSG_INL
#define _RTEMS_SCORE_COREMSG_INL

/**
 *  @addtogroup ScoreMessageQueue 
 *  @{
 */

#include <string.h>   /* needed for memcpy */
 
/**
 *  This routine sends a message to the end of the specified message queue.
 */
RTEMS_INLINE_ROUTINE CORE_message_queue_Status _CORE_message_queue_Send(
  CORE_message_queue_Control                *the_message_queue,
  const void                                *buffer,
  size_t                                     size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  bool                                    wait,
  Watchdog_Interval                          timeout
)
{
  return _CORE_message_queue_Submit(
    the_message_queue,
    buffer,
    size,
    id,
    api_message_queue_mp_support,
    CORE_MESSAGE_QUEUE_SEND_REQUEST,
    wait,     /* sender may block */
    timeout   /* timeout interval */
  );
}
 
/**
 *  This routine sends a message to the front of the specified message queue.
 */
RTEMS_INLINE_ROUTINE CORE_message_queue_Status _CORE_message_queue_Urgent(
  CORE_message_queue_Control                *the_message_queue,
  const void                                *buffer,
  size_t                                     size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  bool                                    wait,
  Watchdog_Interval                          timeout
)
{
  return _CORE_message_queue_Submit(
    the_message_queue,
    buffer,
    size,
    id,
    api_message_queue_mp_support,
    CORE_MESSAGE_QUEUE_URGENT_REQUEST,
    wait,     /* sender may block */
    timeout   /* timeout interval */
 );
}

/**
 *  This routine copies the contents of the source message buffer
 *  to the destination message buffer.
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
 *  This function allocates a message buffer from the inactive
 *  message buffer chain.
 */
RTEMS_INLINE_ROUTINE CORE_message_queue_Buffer_control *
_CORE_message_queue_Allocate_message_buffer (
    CORE_message_queue_Control *the_message_queue
)
{
   return (CORE_message_queue_Buffer_control *)
     _Chain_Get( &the_message_queue->Inactive_messages );
}

/**
 *  This routine frees a message buffer to the inactive
 *  message buffer chain.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Free_message_buffer (
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message
)
{
  _Chain_Append( &the_message_queue->Inactive_messages, &the_message->Node );
}

/**
 *  This function returns the priority of @a the_message.
 *
 *  NOTE: It encapsulates the optional behavior that message priority is
 *        disabled if no API requires it.
 */
RTEMS_INLINE_ROUTINE int _CORE_message_queue_Get_message_priority (
  CORE_message_queue_Buffer_control *the_message
)
{
  #if defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
    return the_message->priority;
  #else
    return 0;
  #endif
}

/**
 *  This function sets the priority of @a the_message.
 *
 *  NOTE: It encapsulates the optional behavior that message priority is
 *        disabled if no API requires it.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Set_message_priority (
  CORE_message_queue_Buffer_control *the_message,
  int                                priority  
)
{
  #if defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
    the_message->priority = priority;
  #endif
}

/**
 *  This function removes the first message from the_message_queue
 *  and returns a pointer to it.
 */
RTEMS_INLINE_ROUTINE
  CORE_message_queue_Buffer_control *_CORE_message_queue_Get_pending_message (
  CORE_message_queue_Control *the_message_queue
)
{
  return (CORE_message_queue_Buffer_control *)
    _Chain_Get_unprotected( &the_message_queue->Pending_messages );
}

/**
 *  This function returns true if the priority attribute is
 *  enabled in the attribute_set and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _CORE_message_queue_Is_priority(
  CORE_message_queue_Attributes *the_attribute
)
{
  return
    (the_attribute->discipline == CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY);
}

/**
 *  This routine places the_message at the rear of the outstanding
 *  messages on the_message_queue.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Append_unprotected (
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message
)
{
  _Chain_Append_unprotected(
    &the_message_queue->Pending_messages,
    &the_message->Node
  );
}

/**
 *  This routine places the_message at the front of the outstanding
 *  messages on the_message_queue.
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Prepend_unprotected (
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message
)
{
  _Chain_Prepend_unprotected(
    &the_message_queue->Pending_messages,
    &the_message->Node
  );
}

/**
 *  This function returns true if the_message_queue is true and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _CORE_message_queue_Is_null (
  CORE_message_queue_Control *the_message_queue
)
{
  return ( the_message_queue == NULL  );
}

#if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
  /**
   *  This function returns true if notification is enabled on this message
   *  queue and false otherwise.
   */
  RTEMS_INLINE_ROUTINE bool _CORE_message_queue_Is_notify_enabled (
    CORE_message_queue_Control *the_message_queue
  )
  {
    return (the_message_queue->notify_handler != NULL);
  }
#endif

/**
 *  This routine initializes the notification information for
 *  @a the_message_queue.
 */
#if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
  RTEMS_INLINE_ROUTINE void _CORE_message_queue_Set_notify (
    CORE_message_queue_Control        *the_message_queue,
    CORE_message_queue_Notify_Handler  the_handler,
    void                              *the_argument
  )
  {
    the_message_queue->notify_handler  = the_handler;
    the_message_queue->notify_argument = the_argument;
  }
#else
  /* turn it into nothing if not enabled */
  #define _CORE_message_queue_Set_notify( \
           the_message_queue, the_handler, the_argument )
#endif

/**@}*/

#endif
/* end of include file */
