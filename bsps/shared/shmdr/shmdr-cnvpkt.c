/* SPDX-License-Identifier: BSD-2-Clause */

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
