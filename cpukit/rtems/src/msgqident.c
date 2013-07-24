/**
 *  @file
 *
 *  @brief RTEMS Message Queue Name to Id
 *  @ingroup ClassicMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>

rtems_status_code rtems_message_queue_ident(
  rtems_name  name,
  uint32_t    node,
  rtems_id   *id
)
{
  Objects_Name_or_id_lookup_errors  status;

  status = _Objects_Name_to_id_u32(
    &_Message_queue_Information,
    name,
    node,
    id
  );

  return _Status_Object_name_errors_to_status[ status ];
}
