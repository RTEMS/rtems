/**
 * @file
 *
 * @ingroup RTEMSScoreMessageQueue
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreMessageQueue which are only used by the implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREMSG_H
#define _RTEMS_SCORE_COREMSG_H

#include <rtems/score/coremsgbuffer.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/threadq.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreMessageQueue Message Queue Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Message Queue Handler implementation.
 *
 * This handler encapsulates functionality which provides the foundation
 * Message Queue services used in all of the APIs supported by RTEMS.
 *
 * @{
 */

#if defined(RTEMS_POSIX_API)
  /**
   *  This macro is defined when an API is enabled that requires that the
   *  Message Queue Handler include support for notification of enqueuing
   *  a message.
   */
  #define RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION
#endif

/**
 *  This macro is defined when an API is enabled that requires the
 *  Message Queue Handler include support for blocking send operations.
 */
#define RTEMS_SCORE_COREMSG_ENABLE_BLOCKING_SEND

typedef struct CORE_message_queue_Control CORE_message_queue_Control;

/**
 *  @brief The possible blocking disciplines for a message queue.
 *
 *  This enumerated types defines the possible blocking disciplines
 *  for a message queue.
 */
typedef enum {
  /** This value indicates that blocking tasks are in FIFO order. */
  CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO,
  /** This value indicates that blocking tasks are in priority order. */
  CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY
}   CORE_message_queue_Disciplines;

#if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
  /**
   *  @brief Type for a notification handler.
   *
   *  The following defines the type for a Notification handler.  A
   *  notification handler is invoked when the message queue makes a
   *  0->1 transition on pending messages.
   */
  typedef void (*CORE_message_queue_Notify_Handler)(
    CORE_message_queue_Control *,
    Thread_queue_Context *
  );
#endif

/**
 *  @brief Control block used to manage each message queue.
 *
 *  The following defines the control block used to manage each
 *  Message Queue.
 */
struct CORE_message_queue_Control {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting to receive a message from this queue.
   */
  Thread_queue_Control               Wait_queue;

  /**
   * @brief The thread queue operations according to the blocking discipline.
   */
  const Thread_queue_Operations     *operations;

  /** This element is maximum number of messages which may be pending
   *  at any given time.
   */
  uint32_t                           maximum_pending_messages;
  /** This element is the number of messages which are currently pending.
   */
  uint32_t                           number_of_pending_messages;
  /** This is the size in bytes of the largest message which may be
   *  sent via this queue.
   */
  size_t                             maximum_message_size;
  /** This chain is the set of pending messages.  It may be ordered by
   *  message priority or in FIFO order.
   */
  Chain_Control                      Pending_messages;
  /** This is the address of the memory allocated for message buffers.
   *  It is allocated are part of message queue initialization and freed
   *  as part of destroying it.
   */
  CORE_message_queue_Buffer         *message_buffers;

  /**
   * @brief This member contains the optional message buffer storage area free
   * handler.
   *
   * It may be NULL.  In this case no action is performed to free the message
   * buffer storage area.
   */
  void                            ( *free_message_buffers )( void * );

  #if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
    /** This is the routine invoked when the message queue transitions
     *  from zero (0) messages pending to one (1) message pending.
     */
    CORE_message_queue_Notify_Handler  notify_handler;
  #endif
  /** This chain is the set of inactive messages.  A message is inactive
   *  when it does not contain a pending message.
   */
  Chain_Control                      Inactive_messages;
};

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
