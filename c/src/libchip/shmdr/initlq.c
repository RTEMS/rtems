/*  void Shm_Locked_queue_Initialize( lq_cb, owner )
 *
 *  This routine initializes a shared memory locked queue.
 *
 *  Input parameters:
 *    lq_cb - pointer to the control block of the queue
 *            to be initialized
 *    owner - unique idenitifier of who owns this queue.
 *
 *  Output parameters: NONE
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

void Shm_Locked_queue_Initialize(
  Shm_Locked_queue_Control *lq_cb,
  rtems_unsigned32 owner
)
{
  Shm_Initialize_lock( lq_cb );
  lq_cb->front = Shm_Locked_queue_End_of_list;
  lq_cb->rear  = Shm_Locked_queue_End_of_list;
  lq_cb->owner = Shm_Convert(owner);
}
