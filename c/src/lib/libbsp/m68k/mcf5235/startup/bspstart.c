/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

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
