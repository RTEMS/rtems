/**
 * @file
 *
 * @ingroup ClassicMessageQueue
 *
 * @brief Classic Message Queue Manager API
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_MESSAGEDATA_H
#define _RTEMS_RTEMS_MESSAGEDATA_H

#include <rtems/rtems/message.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/objectdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicMessageQueueImpl
 *
 * @{
 */

/**
 *  The following records define the control block used to manage
 *  each message queue.
 */
typedef struct {
  /** This field is the inherited object characteristics. */
  Objects_Control             Object;
  /** This field is the instance of the SuperCore Message Queue. */
  CORE_message_queue_Control  message_queue;
  /** This field is the attribute set as defined by the API. */
  rtems_attribute             attribute_set;
}   Message_queue_Control;

/**
 * @brief The Classic Message Queue objects information.
 */
extern Objects_Information _Message_queue_Information;

#if defined(RTEMS_MULTIPROCESSING)
/**
 *  @brief _Message_queue_MP_Send_extract_proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */
void _Message_queue_MP_Send_extract_proxy (
  Thread_Control *the_thread,
  Objects_Id      id
);
#endif

/**
 * @brief Macro to define the objects information for the Classic Message Queue
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define MESSAGE_QUEUE_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Message_queue, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_MESSAGE_QUEUES, \
    Message_queue_Control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    _Message_queue_MP_Send_extract_proxy \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
