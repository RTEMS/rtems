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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include "shm_driver.h"

void Shm_Convert_packet(
  rtems_packet_prefix *packet
)
{
  uint32_t   *pkt, i;

  pkt = (uint32_t*) packet;
  for ( i=RTEMS_MINIMUN_HETERO_CONVERSION ; i ; i--, pkt++ )
    *pkt = CPU_swap_u32( *pkt );

  for ( i=packet->to_convert ; i ; i--, pkt++ )
    *pkt = CPU_swap_u32( *pkt );
}
