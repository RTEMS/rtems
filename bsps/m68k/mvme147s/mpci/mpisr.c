/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 *  NOTE: This routine is not used when in polling mode.  Either
 *        this routine OR Shm_clockisr is used in a particular system.
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
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  June 1996
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>

static rtems_isr Shm_isr_mvme147(rtems_vector_number vector)
{
  (void) vector;
  Shm_Interrupt_count += 1;
  rtems_multiprocessing_announce();
  vme_gcsr->global_1 = 1; /* clear SIGLP intr */
}

/*
 *  This driver routine sets the SHM interrupt vector to point to the
 *  driver's SHM interrupt service routine.
 */
void Shm_setvec(void)
{
  /* may need to disable intr */
  set_vector( Shm_isr_mvme147, VME_SIGLP_VECTOR, 1 );
}
