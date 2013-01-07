/**
 * @file
 * 
 * @brief POSIX Message Queues Private Private Support
 *
 * This include file contains all the private support information for
 * POSIX Message Queues.
 * 
 * The structure of the routines is identical to that of POSIX
 * Message_queues to leave the option of having unnamed message
 * queues at a future date.  They are currently not part of the
 * POSIX standard but unnamed message_queues are.  This is also
 * the reason for the apparently unnecessary tracking of
 * the process_shared attribute.  [In addition to the fact that
 * it would be trivial to add pshared to the mq_attr structure
 * and have process private message queues.]
 *
 * This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open
 * time.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_MQUEUE_H
#define _RTEMS_POSIX_MQUEUE_H

#include <signal.h>
#include <mqueue.h> /* struct mq_attr */
#include <rtems/score/coremsg.h>
#include <rtems/score/object.h>
#include <rtems/posix/posixapi.h>

/**
 *  @defgroup POSIX_MQUEUE_P Message Queues Private Support
 *
 *  @ingroup POSIX_MQUEUE
 * 
 * @{
 */
#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Data Structure used to manage a POSIX message queue
 */

typedef struct {
   Objects_Control             Object;
   int                         process_shared;
   bool                        named;
   bool                        linked;
   uint32_t                    open_count;
   CORE_message_queue_Control  Message_queue;
   struct sigevent             notification;
}  POSIX_Message_queue_Control;

typedef struct {
   Objects_Control              Object;
   POSIX_Message_queue_Control *Queue;
   int                          oflag;
} POSIX_Message_queue_Control_fd;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.  The second item is used to manage the set
 *  of "file descriptors" associated with the message queues.
 */

POSIX_EXTERN Objects_Information  _POSIX_Message_queue_Information;
POSIX_EXTERN Objects_Information  _POSIX_Message_queue_Information_fds;

/**
 * @brief Initialize message_queue manager related data structures.
 *
 * DESCRIPTION:
 *
 * This routine performs the initialization necessary for this manager.
 *
 * NOTE:  The structure of the routines is identical to that of POSIX
 *        Message_queues to leave the option of having unnamed message
 *        queues at a future date.  They are currently not part of the
 *        POSIX standard but unnamed message_queues are.  This is also
 *        the reason for the apparently unnecessary tracking of
 *        the process_shared attribute.  [In addition to the fact that
 *        it would be trivial to add pshared to the mq_attr structure
 *        and have process private message queues.]
 *
 *        This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open
 *        time.
 *
 */

void _POSIX_Message_queue_Manager_initialization(void);

/*
 *
 *  _POSIX_Message_queue_Create_support
 *
 *  DESCRIPTION:
 *
 *  This routine performs the creation of a message queue utilizing the
 *  core message queue.
 */

int _POSIX_Message_queue_Create_support(
  const char                    *name,
  size_t                         name_len,
  int                            pshared,
  struct mq_attr                *attr,
  POSIX_Message_queue_Control  **message_queue
);

/**
 * @brief Delete a POSIX message queue.
 *
 * DESCRIPTION:
 *
 * This routine supports the mq_unlink and mq_close routines by
 * doing most of the work involved with removing a message queue.
 */
void _POSIX_Message_queue_Delete(
  POSIX_Message_queue_Control *the_mq
);

/*
 *  @brief POSIX Message Queue Receive Support
 *
 *  DESCRIPTION:
 *
 *  This routine supports the various flavors of receiving a message.
 *
 *  NOTE:  The structure of the routines is identical to that of POSIX
 *         Message_queues to leave the option of having unnamed message
 *         queues at a future date.  They are currently not part of the
 *         POSIX standard but unnamed message_queues are.  This is also
 *         the reason for the apparently unnecessary tracking of
 *         the process_shared attribute.  [In addition to the fact that
 *         it would be trivial to add pshared to the mq_attr structure
 *         and have process private message queues.]
 *
 *         This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open
 *         time.
 */

ssize_t _POSIX_Message_queue_Receive_support(
  mqd_t               mqdes,
  char               *msg_ptr,
  size_t              msg_len,
  unsigned int       *msg_prio,
  bool                wait,
  Watchdog_Interval   timeout
);

/*
 *  _POSIX_Message_queue_Send_support
 *
 *  DESCRIPTION:
 *
 *  This routine posts a message to a specified message queue.
 */

int _POSIX_Message_queue_Send_support(
  mqd_t               mqdes,
  const char         *msg_ptr,
  size_t              msg_len,
  unsigned int        msg_prio,
  bool                wait,
  Watchdog_Interval   timeout
);

/*
 *  _POSIX_Message_queue_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a message queue control block from
 *  the inactive chain of free message queue control blocks.
 */

RTEMS_INLINE_ROUTINE POSIX_Message_queue_Control *_POSIX_Message_queue_Allocate( void );

/*
 *  _POSIX_Message_queue_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a message queue control block to the
 *  inactive chain of free message queue control blocks.
 */

RTEMS_INLINE_ROUTINE void _POSIX_Message_queue_Free (
  POSIX_Message_queue_Control *the_mq
);

/*
 *  _POSIX_Message_queue_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps message queue IDs to message queue control blocks.
 *  If ID corresponds to a local message queue, then it returns
 *  the_mq control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the message queue ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_message queue is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_mq is undefined.
 */

RTEMS_INLINE_ROUTINE POSIX_Message_queue_Control *_POSIX_Message_queue_Get (
  Objects_Id         id,
  Objects_Locations *location
);

/*
 *  _POSIX_Message_queue_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_message_queue is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE bool    _POSIX_Message_queue_Is_null (
  POSIX_Message_queue_Control *the_mq
);

/*
 *  _POSIX_Message_queue_Priority_to_core
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

RTEMS_INLINE_ROUTINE CORE_message_queue_Submit_types _POSIX_Message_queue_Priority_to_core(
  unsigned int priority
);

/*
 *  _POSIX_Message_queue_Priority_from_core
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

RTEMS_INLINE_ROUTINE unsigned int _POSIX_Message_queue_Priority_from_core(
  CORE_message_queue_Submit_types priority
);

/*
 *  _POSIX_Message_queue_Translate_core_message_queue_return_code
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

int _POSIX_Message_queue_Translate_core_message_queue_return_code(
  uint32_t   the_message_queue_status
);


#include <rtems/posix/mqueue.inl>

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
