/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 */

/*
 *  COPYRIGHT (c) 1989-1999, 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems.h>
#include "shm_driver.h"

struct pkt_cpy {
  uint32_t   packet[MAX_PACKET_SIZE/4];
};

/**
 * This routine is the shared memory driver locked queue write
 * MPCI driver routine.  This routine sends the specified packet
 * to the destination specified by "node".  A "node" value of
 * zero designates that this packet is to be broadcasted.
 *
 * @param node is the destination of this packet (0 = broadcast)
 * @param packet is the address of packet
 *
 * @return NONE
 */
rtems_mpci_entry Shm_Send_packet(
  uint32_t   node,
  rtems_packet_prefix *packet
)
{
  Shm_Envelope_control *ecb, *tmp_ecb;
  uint32_t   nnum;

  ecb = Shm_Packet_prefix_to_envelope_control_pointer( packet );
  if ( node ) {
    Shm_Build_preamble( ecb, node );
    Shm_Build_postamble( ecb );
    Shm_Append_to_receive_queue( node, ecb );
    (*Shm_Configuration->cause_intr)( node );
  }
  else {
    for( nnum = SHM_FIRST_NODE ; nnum <= SHM_MAXIMUM_NODES ; nnum++ )
      if ( rtems_object_get_local_node() != nnum ) {
        struct pkt_cpy *pkt;

        tmp_ecb = Shm_Allocate_envelope();
        if ( !tmp_ecb )
          rtems_fatal_error_occurred( SHM_NO_FREE_PKTS );
        Shm_Build_preamble( tmp_ecb, nnum );
        pkt = (struct pkt_cpy *)tmp_ecb->packet;
        *pkt = *((struct pkt_cpy *)packet);
        Shm_Build_postamble( tmp_ecb );
        Shm_Append_to_receive_queue( nnum, tmp_ecb );
        (*Shm_Configuration->cause_intr)( nnum );
      }
    Shm_Free_envelope( ecb );
  }
}
