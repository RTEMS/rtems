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

rtems_mpci_entry Shm_Return_packet(
  rtems_packet_prefix *packet
)
{
  Shm_Free_envelope( Shm_Packet_prefix_to_envelope_control_pointer(packet) );
}

