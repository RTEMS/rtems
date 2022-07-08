/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This routine does the bulk of the system initialization.
 */

/*
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

#include <bsp.h>
#include <bsp/bootcard.h>

/*
 * Read/write copy of common cache
 *   Split I/D cache
 *   Allow CPUSHL to invalidate a cache line
 *   Enable buffered writes
 *   No burst transfers on non-cacheable accesses
 *   Default cache mode is *disabled* (cache only ACRx areas)
 */
uint32_t cacr_mode = MCF5XXX_CACR_CENB | MCF5XXX_CACR_DBWE | MCF5XXX_CACR_DCM;

/*
 * Cacheable areas
 */
extern char RamBase[];
extern char RamSize[];

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialisation.
 */
void bsp_start( void )
{
  /*
   * Invalidate the cache and disable it
   */
  m68k_set_acr0(0);
  m68k_set_acr1(0);
  m68k_set_cacr(MCF5XXX_CACR_CINV);

  /*
   * Cache SDRAM
   */
  m68k_set_acr0(MCF5XXX_ACR_AB((uintptr_t)RamBase)   |
                MCF5XXX_ACR_AM((uintptr_t)RamSize-1) |
                MCF5XXX_ACR_EN                       |
                MCF5XXX_ACR_BWE                      |
                MCF5XXX_ACR_SM_IGNORE);

  /*
   * Enable the cache
   */
  m68k_set_cacr(cacr_mode);
}
