/* SPDX-License-Identifier: BSD-2-Clause */

/*  Shared Memory Lock Routines
 *
 *  This shared memory locked queue support routine need to be
 *  able to lock the specified locked queue.  Interrupts are
 *  disabled while the queue is locked to prevent preemption
 *  and deadlock when two tasks poll for the same lock.
 *  previous level.
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
#include <bsp.h>
#include <shm_driver.h>

/*
 *  Shm_Initialize_lock
 *
 *  Initialize the lock for the specified locked queue.
 */

void Shm_Initialize_lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  lq_cb->lock = LQ_UNLOCKED;
}

/*  void _Shm_Lock( &lq_cb )
 *
 *  This shared memory locked queue support routine locks the
 *  specified locked queue.  It disables interrupts to prevent
 *  a deadlock condition.
 */

void Shm_Lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  uint32_t         isr_level;
  uint32_t         *lockptr = (uint32_t*)&lq_cb->lock;

  rtems_interrupt_disable( isr_level );
    Shm_isrstat = isr_level;
    __asm__ volatile( "lockit:"    : : );
    __asm__ volatile( "tas %0@"    : "=a" (lockptr) : "0" (lockptr) );
    __asm__ volatile( "bne lockit" : : );
/* should delay */
}

/*
 *  Shm_Unlock
 *
 *  Unlock the lock for the specified locked queue.
 */

void Shm_Unlock(
  Shm_Locked_queue_Control *lq_cb
)
{
  uint32_t         isr_level;

  lq_cb->lock = SHM_UNLOCK_VALUE;
  isr_level = Shm_isrstat;
  rtems_interrupt_enable( isr_level );
}
