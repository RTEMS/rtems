/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the BSP initialization code                  |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       bspstart.c                                          */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 C level startup code                  */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  This routine starts the application. It includes    */
/*                 application, board, and monitor specific            */
/*                 initialization and configuration. The generic CPU   */
/*                 dependent initialization has been performed before  */
/*                 this routine is invoked.                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   MPC8260ads C level startup code                     */
/*   Module:       bspstart.c                                          */
/*   Project:      RTEMS 4.6.0pre1 / MCF8260ads BSP                    */
/*   Version       1.2                                                 */
/*   Date:         04/17/2002                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   The MPC860 specific stuff was written by Jay Monkman              */
/*   (jmonkman@frasca.com)                                             */
/*                                                                     */
/*   Modified for the MPC8260ADS board by Andy Dachs                   */
/*   <a.dachs@sstl.co.uk>                                              */
/*   Surrey Satellite Technology Limited, 2001                         */
/*   A 40MHz system clock is assumed.                                  */
/*   The PON. RST.CONF. Dip switches (DS1) are                         */
/*   1 - Off                                                           */
/*   2 - On                                                            */
/*   3 - Off                                                           */
/*   4 - On                                                            */
/*   5 - Off                                                           */
/*   6 - Off                                                           */
/*   7 - Off                                                           */
/*   8 - Off                                                           */
/*   Dip switches on DS2 and DS3 are all set to ON                     */
/*   The LEDs on the board are used to signal panic and fatal_error    */
/*   conditions.                                                       */
/*   The mmu is unused at this time.                                   */
/*                                                                     */
/*   COPYRIGHT (c) 1989-2007.                                          */
/*   On-Line Applications Research Corporation (OAR).                  */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in the file LICENSE in this distribution or at     */
/*   http://www.rtems.com/license/LICENSE.                             */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#include <rtems.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/mpc5200.h>

/* Configuration parameter for clock driver */
uint32_t bsp_time_base_frequency;

/* Legacy */
uint32_t bsp_clicks_per_usec;

void BSP_panic(char *s)
{
  printk("%s PANIC %s\n",_RTEMS_version, s);
  __asm__ __volatile ("sc");
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("%s PANIC ERROR %x\n",_RTEMS_version, v);
  __asm__ __volatile ("sc");
}

void bsp_start(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables so that it can be used
   * later...
   */
  myCpu         = get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  #if defined(HAS_UBOOT) && defined(SHOW_MORE_INIT_SETTINGS)
    {
      void dumpUBootBDInfo( bd_t * );
      dumpUBootBDInfo( &bsp_uboot_board_info );
    }
  #endif

  cpu_init();

  if(get_ppc_cpu_revision() >= 0x2014) {
    /* Special settings for MPC5200B (B variant) */
    uint32_t xlb_cfg = mpc5200.config;

    /* XXX: The Freescale documentation for BSDIS seems to be wrong */
    xlb_cfg |= XLB_CFG_BSDIS;

    xlb_cfg &= ~XLB_CFG_PLDIS;

    mpc5200.config = xlb_cfg;
  }

  bsp_time_base_frequency = XLB_CLOCK / 4;
  bsp_clicks_per_usec    = (XLB_CLOCK/4000000);

  /*
   * Enable instruction and data caches. Do not force writethrough mode.
   */
  #if BSP_INSTRUCTION_CACHE_ENABLED
    rtems_cache_enable_instruction();
  #endif
  #if BSP_DATA_CACHE_ENABLED
    rtems_cache_enable_data();
  #endif

  /* Initialize exception handler */
  ppc_exc_cache_wb_check = 0;
  sc = ppc_exc_initialize(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    (uintptr_t) bsp_interrupt_stack_start,
    (uintptr_t) bsp_interrupt_stack_size
  );
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot initialize exceptions");
  }
  ppc_exc_set_handler(ASM_ALIGN_VECTOR, ppc_exc_alignment_handler);

  /* Initalize interrupt support */
  sc = bsp_interrupt_initialize();
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot intitialize interrupts");
  }

  /*
   *  If the BSP was built with IRQ benchmarking enabled,
   *  then intialize it.
   */
  #if (BENCHMARK_IRQ_PROCESSING == 1)
    BSP_IRQ_Benchmarking_Reset();
  #endif

  #ifdef SHOW_MORE_INIT_SETTINGS
    printk("Exit from bspstart\n");
  #endif
}
