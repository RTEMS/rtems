/*  void Shm_Convert_packet( &packet )
 *
 *  This routine is the shared memory locked queue MPCI driver routine
 *  used to convert the RTEMS's information in a packet from non-native
 *  format to processor native format.
 *
 *  Input parameters:
 *    packet  - pointer to a packet
 *
 *  Output parameters:
 *    *packet - packet in native format
 *
 *  NOTE: Message buffers are not manipulated.
 *        Endian conversion is currently the only conversion.
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

void Shm_Convert_packet(
  rtems_packet_prefix *packet
)
{
  rtems_unsigned32 *pkt, i;

  pkt = (rtems_unsigned32 *) packet;
  for ( i=RTEMS_MINIMUN_HETERO_CONVERSION ; i ; i--, pkt++ )
    *pkt = CPU_swap_u32( *pkt );

  for ( i=packet->to_convert ; i ; i--, pkt++ )
    *pkt = CPU_swap_u32( *pkt );
}
