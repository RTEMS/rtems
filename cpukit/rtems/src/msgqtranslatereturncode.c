/**
 *  @file
 *
 *  @brief Message queue Translate Core Message Queue Return Code
 *  @ingroup ClassicMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremsgimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>

rtems_status_code _Message_queue_Translate_core_return_code_[] = {
  RTEMS_SUCCESSFUL,         /* CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL */
  RTEMS_INVALID_SIZE,       /* CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE */
  RTEMS_TOO_MANY,           /* CORE_MESSAGE_QUEUE_STATUS_TOO_MANY */
  RTEMS_UNSATISFIED,        /* CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED */
  RTEMS_UNSATISFIED,        /* CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT */
  RTEMS_OBJECT_WAS_DELETED, /* CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED */
  RTEMS_TIMEOUT             /* CORE_MESSAGE_QUEUE_STATUS_TIMEOUT */
};

rtems_status_code _Message_queue_Translate_core_message_queue_return_code (
  uint32_t   status
)
{
  /*
   *  Check for proxy blocking first since it is out of range
   *  from the external status codes.
   */
  #if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_Is_proxy_blocking(status) )
      return RTEMS_PROXY_BLOCKING;
  #endif

  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( status > CORE_MESSAGE_QUEUE_STATUS_TIMEOUT )
      return RTEMS_INTERNAL_ERROR;
  #endif

  return _Message_queue_Translate_core_return_code_[status];
}
