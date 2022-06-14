/* SPDX-License-Identifier: BSD-2-Clause */

/*  void Shm_Cause_interrupt( node )
 *
 *  This routine is the shared memory driver routine which
 *  generates interrupts to other CPUs.
 *
 *  It uses the information placed in the node status control
 *  block by each node.  For example, when used with the Motorola
 *  MVME136 board, the MPCSR is used.
 *
 *  Input parameters:
 *    node          - destination of this packet (0 = broadcast)
 *
 *  Output parameters: NONE
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

void Shm_Cause_interrupt(
  uint32_t   node
)
{
  Shm_Interrupt_information *intr;
  uint8_t    *u8;
  uint16_t   *u16;
  uint32_t   *u32;
  uint32_t    value;

  intr = &Shm_Interrupt_table[node];
  value = intr->value;

  switch ( intr->length ) {
    case NO_INTERRUPT:
       break;
    case BYTE:
      u8   = (uint8_t*)intr->address;
      *u8  = (uint8_t) value;
      break;
    case WORD:
      u16   = (uint16_t*)intr->address;
      *u16  = (uint16_t) value;
      break;
    case LONG:
      u32   = (uint32_t*)intr->address;
      *u32  = (uint32_t) value;
      break;
  }
}
