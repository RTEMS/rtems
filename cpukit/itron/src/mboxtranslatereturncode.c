/*
 *  ITRON 3.0 Mailbox Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/itron/mbox.h>
#include <rtems/itron/task.h>

/*
 *  _ITRON_Mailbox_Translate_core_message_queue_return_code
 *
 *  This routine translates a core message queue object status
 *  into the appropriate ITRON status code.
 */

ER _ITRON_Mailbox_Translate_core_message_queue_return_code(
  CORE_message_queue_Status status
)
{
  switch (status) {
    case CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL:
      return E_OK;
    case CORE_MESSAGE_QUEUE_STATUS_TOO_MANY:
      return E_TMOUT;
    case CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE:
      return E_PAR;
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT:
      return E_TMOUT;
    case CORE_MESSAGE_QUEUE_STATUS_TIMEOUT:
      return E_TMOUT;
    default:
      return E_ID;
  }
}
