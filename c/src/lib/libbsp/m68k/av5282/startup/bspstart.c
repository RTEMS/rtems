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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
