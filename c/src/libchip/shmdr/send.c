/*  Shm_Send_packet
 *
 *  This routine is the shared memory driver locked queue write
 *  MPCI driver routine.  This routine sends the specified packet
 *  to the destination specified by "node".  A "node" value of
 *  zero designates that this packet is to be broadcasted.
 *
 *  Input parameters:
 *    node          - destination of this packet (0 = broadcast)
 *    packet        - address of packet
 *
 *  Output parameters: NONE
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

#include <rtems.h>
#include "shm_driver.h"

struct pkt_cpy {
  rtems_unsigned32 packet[MAX_PACKET_SIZE/4];
};

rtems_mpci_entry Shm_Send_packet(
  rtems_unsigned32 node,
  rtems_packet_prefix *packet
)
{
  Shm_Envelope_control *ecb, *tmp_ecb;
  rtems_unsigned32 nnum;

  ecb = Shm_Packet_prefix_to_envelope_control_pointer( packet );
  if ( node ) {
    Shm_Build_preamble( ecb, node );
    Shm_Build_postamble( ecb );
    Shm_Append_to_receive_queue( node, ecb );
    (*Shm_Configuration->cause_intr)( node );
  }
  else {
    for( nnum = SHM_FIRST_NODE ; nnum <= Shm_Maximum_nodes ; nnum++ )
      if ( Shm_Local_node != nnum ) {
        tmp_ecb = Shm_Allocate_envelope();
        if ( !tmp_ecb )
          rtems_fatal_error_occurred( SHM_NO_FREE_PKTS );
        Shm_Build_preamble( tmp_ecb, nnum );
        *((struct pkt_cpy *)tmp_ecb->packet) = *((struct pkt_cpy *)packet);
        Shm_Build_postamble( tmp_ecb );
        Shm_Append_to_receive_queue( nnum, tmp_ecb );
        (*Shm_Configuration->cause_intr)( nnum );
      }
    Shm_Free_envelope( ecb );
  }
}
