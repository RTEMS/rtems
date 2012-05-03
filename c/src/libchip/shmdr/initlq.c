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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include "shm_driver.h"

void Shm_Locked_queue_Initialize(
  Shm_Locked_queue_Control *lq_cb,
  uint32_t   owner
)
{
  Shm_Initialize_lock( lq_cb );
  lq_cb->front = Shm_Locked_queue_End_of_list;
  lq_cb->rear  = Shm_Locked_queue_End_of_list;
  lq_cb->owner = Shm_Convert(owner);
}
