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

#include <rtems/score/coremsg.h>
#include <rtems/score/objectdata.h>

#include <limits.h>
#include <mqueue.h>
#include <signal.h>

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
   CORE_message_queue_Control  Message_queue;
   bool                        linked;
   uint32_t                    open_count;
   struct sigevent             notification;
   int                         oflag;
}  POSIX_Message_queue_Control;

/**
 * @brief The POSIX Message Queue objects information.
 */
extern Objects_Information _POSIX_Message_queue_Information;

/**
 * @brief Macro to define the objects information for the POSIX Message Queue
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define POSIX_MESSAGE_QUEUE_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _POSIX_Message_queue, \
    OBJECTS_POSIX_API, \
    OBJECTS_POSIX_MESSAGE_QUEUES, \
    POSIX_Message_queue_Control, \
    max, \
    _POSIX_PATH_MAX, \
    NULL \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
