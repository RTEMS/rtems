/**
 *  @file  rtems/score/coremsg.h
 *
 *  @brief Constants and Structures Associated with the Message Queue Handler.
 *
 *  This include file contains all the constants and structures associated
 *  with the Message queue Handler.
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

#include <rtems/score/chain.h>
#include <rtems/score/threadq.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreMessageQueue Message Queue Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Message Queue services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#if defined(RTEMS_POSIX_API)
  /**
   *  This macro is defined when an API is enabled that requires that the
   *  Message Queue Handler include support for priority based enqueuing
   *  of messages.
   */
  #define RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY
#endif

#if defined(RTEMS_POSIX_API)
  /**
   *  This macro is defined when an API is enabled that requires that the
   *  Message Queue Handler include support for notification of enqueuing
   *  a message.
   */
  #define RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION
#endif

#if defined(RTEMS_POSIX_API)
  /**
   *  This macro is defined when an API is enabled that requires the
   *  Message Queue Handler include support for blocking send operations.
   */
  #define RTEMS_SCORE_COREMSG_ENABLE_BLOCKING_SEND
#endif

/**
 *  @brief Data types needed to manipulate the contents of message buffers.
 *
 *  The following defines the data types needed to manipulate
 *  the contents of message buffers.
 *
 *  @note  The buffer field is normally longer than a single uint32_t
 *         but since messages are variable length we just make a ptr to 1.
 */
typedef struct {
  /** This field is the size of this message. */
  size_t      size;
  /** This field contains the actual message. */
  uint32_t    buffer[1];
} CORE_message_queue_Buffer;

/**
 *  @brief The organization of a message buffer.
 *
 *  The following records define the organization of a message
 *  buffer.
 */
typedef struct {
  /** This element allows this structure to be placed on chains. */
  Chain_Node                 Node;
  #if defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
    /** This field is the priority of this message. */
    int                        priority;
  #endif
  /** This field points to the contents of the message. */
  CORE_message_queue_Buffer  Contents;
}   CORE_message_queue_Buffer_control;

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

/**
 *  @brief Control block used to manage the attributes of each message queue.
 *
 *  The following defines the control block used to manage the
 *  attributes of each message queue.
 */
typedef struct {
  /** This field specifies the order in which blocking tasks will be ordered. */
  CORE_message_queue_Disciplines  discipline;
}   CORE_message_queue_Attributes;

#if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
  /**
   *  @brief Type for a notification handler.
   *
   *  The following defines the type for a Notification handler.  A
   *  notification handler is invoked when the message queue makes a
   *  0->1 transition on pending messages.
   */
  typedef void (*CORE_message_queue_Notify_Handler)( void * );
#endif

/**
 *  @brief Control block used to manage each message queue.
 *
 *  The following defines the control block used to manage each
 *  Message Queue.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting to receive a message from this queue.
   */
  Thread_queue_Control               Wait_queue;
  /** This element is the set of attributes which define this instance's
   *  behavior.
   */
  CORE_message_queue_Attributes      Attributes;
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
  #if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
    /** This is the routine invoked when the message queue transitions
     *  from zero (0) messages pending to one (1) message pending.
     */
    CORE_message_queue_Notify_Handler  notify_handler;
    /** This field is the argument passed to the @ref notify_argument. */
    void                              *notify_argument;
  #endif
  /** This chain is the set of inactive messages.  A message is inactive
   *  when it does not contain a pending message.
   */
  Chain_Control                      Inactive_messages;
}   CORE_message_queue_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
