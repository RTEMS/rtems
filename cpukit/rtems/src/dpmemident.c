/*
 *  Dual Port Memory Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/address.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/dpmem.h>

/*PAGE
 *
 *  rtems_port_ident
 *
 *  This directive returns the system ID associated with
 *  the port name.
 *
 *  Input parameters:
 *    name - user defined port name
 *    id   - pointer to port id
 *
 *  Output parameters:
 *    *id      - port id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_port_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  Objects_Name_to_id_errors  status;

  status = _Objects_Name_to_id(
    &_Dual_ported_memory_Information,
    (Objects_Name) name,
    OBJECTS_SEARCH_ALL_NODES,
    id
  );

  return _Status_Object_name_errors_to_status[ status ];
}
