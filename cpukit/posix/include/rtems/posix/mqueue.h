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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_MQUEUE_H
#define _RTEMS_POSIX_MQUEUE_H

#include <signal.h>
#include <mqueue.h> /* struct mq_attr */
#include <rtems/score/coremsg.h>
#include <rtems/score/object.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup POSIX_MQUEUE_P Message Queues Private Support
 *
 *  @ingroup POSIXAPI
 * 
 */
/**@{**/

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

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
