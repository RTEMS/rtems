/* SPDX-License-Identifier: BSD-2-Clause */

/*  void Shm_Locked_queue_Add( lq_cb, ecb )
 *
 *  This routine adds an envelope control block to a shared memory queue.
 *
 *  Input parameters:
 *    lq_cb - pointer to a locked queue control block
 *    ecb   - pointer to an envelope control block
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

void Shm_Locked_queue_Add(
  Shm_Locked_queue_Control *lq_cb,
  Shm_Envelope_control     *ecb
)
{
  uint32_t   index;

  ecb->next  = Shm_Locked_queue_End_of_list;
  ecb->queue = lq_cb->owner;
  index      = ecb->index;

  Shm_Lock( lq_cb );
    if ( Shm_Convert(lq_cb->front) != Shm_Locked_queue_End_of_list )
      Shm_Envelopes[ Shm_Convert(lq_cb->rear) ].next = index;
    else
      lq_cb->front = index;
    lq_cb->rear  = index;
  Shm_Unlock( lq_cb );
}
