/* SPDX-License-Identifier: BSD-2-Clause */

/*  void Shm_Get_configuration( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the PowerPC PSIM.
 *
 *  INPUT PARAMETERS:
 *    localnode - local node number
 *    shmcfg    - address of pointer to SHM Config Table
 *
 *  OUTPUT PARAMETERS:
 *    *shmcfg   - pointer to SHM Config Table
 *
 *  NOTES:  No interrupt support.
 *
 *  COPYRIGHT (c) 1989-2008.
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
#include <psim.h>
#include "shm_driver.h"

#define INTERRUPT 0                   /* PSIM target supports only */
#define POLLING   1                   /* polling mode. */

shm_config_table BSP_shm_cfgtbl;

void Shm_Get_configuration(
  uint32_t           localnode,
  shm_config_table **shmcfg
)
{
   BSP_shm_cfgtbl.base         = (uint32_t*)PSIM.SharedMemory;
   BSP_shm_cfgtbl.length       = sizeof(PSIM.SharedMemory);
   BSP_shm_cfgtbl.format       = SHM_BIG;

   BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt;

#ifdef NEUTRAL_BIG
   BSP_shm_cfgtbl.convert      = NULL_CONVERT;
#else
   BSP_shm_cfgtbl.convert      = CPU_swap_u32;
#endif

#if (POLLING==1)
   BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
   BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;
#else
   BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
   BSP_shm_cfgtbl.Intr.address = 0;
   BSP_shm_cfgtbl.Intr.value   = 0;
   BSP_shm_cfgtbl.Intr.length  = BYTE;
#endif

   *shmcfg = &BSP_shm_cfgtbl;

}
