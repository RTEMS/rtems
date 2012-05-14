/*  Shm_Receive_packet
 *
 *  This routine is the shared memory locked queue MPCI driver routine
 *  used to obtain a packet containing a message from this node's
 *  receive queue.
 *
 *  Input parameters:
 *    packet         - address of a pointer to a packet
 *
 *  Output parameters:
 *    *(rpb->packet) - pointer to packet
 *                     NULL if no packet currently available
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

rtems_mpci_entry Shm_Receive_packet(
  rtems_packet_prefix **packet
)
{
  Shm_Envelope_control *ecb;

  ecb = Shm_Locked_queue_Get( Shm_Local_receive_queue );
  if ( ecb ) {
    *(packet) = Shm_Envelope_control_to_packet_prefix_pointer( ecb );
    if ( ecb->Preamble.endian != Shm_Configuration->format )
      Shm_Convert_packet( *packet );
    Shm_Receive_message_count++;
  } else {
    *(packet) = NULL;
    Shm_Null_message_count++;
  }
}
