/* SPDX-License-Identifier: BSD-2-Clause */

/*  Shm_Envelope_control *Shm_Locked_queue_Get( lq_cb )
 *
 *  This routine returns an envelope control block from a shared
 *  memory queue.
 *
 *  Input parameters:
 *    lq_cb - pointer to a locked queue control block
 *
 *  Output parameters:
 *    returns - pointer to an envelope control block
 *            - NULL if no envelopes on specified queue
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
#include <shm_driver.h>

Shm_Envelope_control *Shm_Locked_queue_Get(
  Shm_Locked_queue_Control *lq_cb
)
{
  Shm_Envelope_control *tmp_ecb;
  uint32_t   tmpfront;

  tmp_ecb = NULL;
  Shm_Lock( lq_cb );

    tmpfront = Shm_Convert(lq_cb->front);
    if ( tmpfront != Shm_Locked_queue_End_of_list ) {
      tmp_ecb = &Shm_Envelopes[ tmpfront ];
      lq_cb->front = tmp_ecb->next;
      if ( tmp_ecb->next == Shm_Locked_queue_End_of_list )
        lq_cb->rear = Shm_Locked_queue_End_of_list;
      tmp_ecb->next = Shm_Locked_queue_Not_on_list;
    }

  Shm_Unlock( lq_cb );
  return( tmp_ecb );
}
