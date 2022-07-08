/* SPDX-License-Identifier: BSD-2-Clause */

/*  void Shm_Get_configuration( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the MVME147.
 *
 *  INPUT PARAMETERS:
 *    localnode - local node number
 *    shmcfg    - address of pointer to SHM Config Table
 *
 *  OUTPUT PARAMETERS:
 *    *shmcfg   - pointer to SHM Config Table
 *
 *  NOTES:  The SIGLP interrupt on the MVME147 is used as an interprocessor
 *          interrupt.
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
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  June 1996
 */

#include <bsp.h>
#include <rtems.h>
#include "shm_driver.h"

#define INTERRUPT 1                   /* MVME147 target supports both */
#define POLLING   0                   /* polling and interrupt modes  */

shm_config_table BSP_shm_cfgtbl;

static uint32_t *BSP_int_address(void)
{
  uint32_t         id, offset;

  id      = (uint32_t) vme_lcsr->gcsr_base_address;
  offset  = (id << 4) & 0xF0;
  offset |= 0xffff0003; /* points to GCSR global 1 */
  return( (uint32_t         * ) offset );
}

void Shm_Get_configuration(
  uint32_t           localnode,
  shm_config_table **shmcfg
)
{
  /* A shared mem space has bee left between RAM_END and DRAM_END
   on the first node*/
  if (localnode == 1)
    BSP_shm_cfgtbl.base       = (vol_u32 *) RAM_END;
  else
    BSP_shm_cfgtbl.base       = (vol_u32 *) (DRAM_END + RAM_END);

  BSP_shm_cfgtbl.length       = DRAM_END - RAM_END;
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
  BSP_shm_cfgtbl.Intr.address = BSP_int_address(); /* GCSR global 1 */
  BSP_shm_cfgtbl.Intr.value   = 0x01; /* SIGLP */
  BSP_shm_cfgtbl.Intr.length  = BYTE;
#endif

  *shmcfg = &BSP_shm_cfgtbl;

}
