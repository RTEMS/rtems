/*  void MPCI_Fatal( error )
 *
 *  This routine is the shared memory driver fatal error handler.
 *
 *  Input parameters:
 *    error - fatal error code
 *
 *  Output parameters:  NEVER RETURNS
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems.h>
#include "shm.h"

void MPCI_Fatal(
  rtems_unsigned32 error
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
