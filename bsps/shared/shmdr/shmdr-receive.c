/* SPDX-License-Identifier: BSD-2-Clause */

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
