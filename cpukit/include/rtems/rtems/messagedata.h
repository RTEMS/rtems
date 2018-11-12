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

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
