/*  Shm_Return_packet
 *
 *  This routine is the shared memory locked queue MPCI driver
 *  routine used to return a message packet to a free envelope
 *  pool accessible by this node.
 *
 *  Input parameters:
 *    packet             - address of pointer to packet
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

rtems_mpci_entry Shm_Return_packet(
  rtems_packet_prefix *packet
)
{
  Shm_Free_envelope( Shm_Packet_prefix_to_envelope_control_pointer(packet) );
}
