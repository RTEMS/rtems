/*
 *  Message Queue Manager
 *
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

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/rtems/status.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>

/*PAGE
 *
 *  rtems_message_queue_ident
 *
 *  This directive returns the system ID associated with
 *  the message queue name.
 *
 *  Input parameters:
 *    name - user defined message queue name
 *    node - node(s) to be searched
 *    id   - pointer to message queue id
 *
 *  Output parameters:
 *    *id               - message queue id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_message_queue_ident(
  rtems_name    name,
  unsigned32    node,
  Objects_Id   *id
)
{
  Objects_Name_to_id_errors  status;

  status = _Objects_Name_to_id(
    &_Message_queue_Information,
    name,
    node,
    id 
  );

  return _Status_Object_name_errors_to_status[ status ];
}
