/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This routine does the bulk of the system initialisation.
 */

/*
 *  Author:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <string.h>

/*
 * Cacheable areas
 */
#define SDRAM_BASE      0
#define SDRAM_SIZE      (16*1024*1024)
#define FLASH_BASE      0xFF800000
#define FLASH_SIZE      (8*1024*1024)

void bsp_start( void )
{
  /*
   * Invalidate the cache and disable it
   */
  m68k_set_acr0(0);
  m68k_set_acr1(0);
  m68k_set_cacr(MCF5XXX_CACR_CINV);

  /*
   * Cache SDRAM and FLASH
   */
  m68k_set_acr0(
    MCF5XXX_ACR_AB(SDRAM_BASE)    |
    MCF5XXX_ACR_AM(SDRAM_SIZE-1)  |
    MCF5XXX_ACR_EN                |
    MCF5XXX_ACR_BWE               |
    MCF5XXX_ACR_SM_IGNORE
  );

  /*
   * Enable the cache
   */
  mcf5xxx_initialize_cacr(
    MCF5XXX_CACR_CENB |
    MCF5XXX_CACR_DBWE |
    MCF5XXX_CACR_DCM
  );
}

extern char _CPUClockSpeed[];

uint32_t get_CPU_clock_speed(void)
{
  return( (uint32_t)_CPUClockSpeed);
}
