/*  void MPCI_Fatal( error )
 *
 *  This routine is the shared memory driver fatal error handler.
 *
 *  Input parameters:
 *    error - fatal error code
 *
 *  Output parameters:  NEVER RETURNS
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include "shm_driver.h"

void MPCI_Fatal(
  Internal_errors_Source  source,
  bool                    is_internal,
  uint32_t                error
)
{
   /*   Eventually need to attempt to broadcast a K_FATAL message
    *   without checking for all possible errors (do not want to
    *   recurse).
    *
    *   Also need to avoid using Shm_Node_statuses if the driver has not been
    *   initialized.
    */

   Shm_Local_node_status->error = Shm_Convert(error);
}
