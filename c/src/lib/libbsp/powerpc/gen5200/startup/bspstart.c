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
/*   found in found in the file LICENSE in this distribution or at     */
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

#warning The interrupt disable mask is now stored in SPRG0, please verify that this is compatible to this BSP (see also bootcard.c).

#include <string.h>

#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/score/thread.h>

#include <libcpu/powerpc-utility.h>
#include <libcpu/raw_exception.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/ppc_exc_bspsupp.h>

#include <bsp/irq.h>

#if defined(HAS_UBOOT)
bd_t *uboot_bdinfo_ptr = (bd_t *)1; /* will be overwritten from startup code */
bd_t uboot_bdinfo_copy;             /* will be overwritten with copy of bdinfo */
#endif

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;

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

void bsp_get_work_area(
  void   **work_area_start,
  size_t  *work_area_size,
  void   **heap_start,
  size_t  *heap_size)
{
#ifdef HAS_UBOOT
  char *ram_end = (char *) uboot_bdinfo_ptr->bi_memstart +
                                 uboot_bdinfo_ptr->bi_memsize;
#else /* HAS_UBOOT */
  char *ram_end = bsp_ram_end;
#endif /* HAS_UBOOT */

  *work_area_start = bsp_work_area_start;
  *work_area_size = ram_end - bsp_work_area_start;
  *heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}

void bsp_predriver_hook(void)
  {
#if 0
  init_RTC();

  init_PCI();
  initialize_universe();
  initialize_PCI_bridge ();

#if (HAS_PMC_PSC8)
  initialize_PMC();
#endif

 /*
  * Initialize Bsp General purpose vector table.
  */
 initialize_external_exception_vector();

#if (0)
  /*
   * XXX - Modify this to write a 48000000 (loop to self) command
   *       to each interrupt location.  This is better for debug.
   */
 bsp_spurious_initialize();
#endif

#endif
}

void bsp_start(void)
{
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables so that it can be used
   * later...
   */
  myCpu         = get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  #if defined(HAS_UBOOT)
    uboot_bdinfo_copy = *uboot_bdinfo_ptr;
    uboot_bdinfo_ptr = &uboot_bdinfo_copy;
  #endif  

  #if defined(HAS_UBOOT) && defined(SHOW_MORE_INIT_SETTINGS)
    {
      void dumpUBootBDInfo( bd_t * );
      dumpUBootBDInfo( uboot_bdinfo_ptr );
    }
  #endif

  cpu_init();

 bsp_clicks_per_usec    = (IPB_CLOCK/1000000);

  /*
   * Enable instruction and data caches. Do not force writethrough mode.
   */
  #if INSTRUCTION_CACHE_ENABLE
    rtems_cache_enable_instruction();
  #endif
  #if DATA_CACHE_ENABLE
    rtems_cache_enable_data();
  #endif

  /* Initialize exception handler */
  ppc_exc_cache_wb_check = 0;
  ppc_exc_initialize(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    (uint32_t) bsp_interrupt_stack_start,
    (uint32_t) bsp_interrupt_stack_size
  );

  /* Initalize interrupt support */
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    BSP_panic( "Cannot intitialize interrupt support\n");
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

/*
 *
 *  _Thread_Idle_body
 *
 *  Replaces the one in c/src/exec/score/src/threadidlebody.c
 *  The MSR[POW] bit is set to put the CPU into the low power mode
 *  defined in HID0.  HID0 is set during starup in start.S.
 *
 */
Thread _Thread_Idle_body(uint32_t ignored)
{
  for(;;) {
    asm volatile(
      "mfmsr 3; oris 3,3,4; sync; mtmsr 3; isync; ori 3,3,0; ori 3,3,0"
     );
  }
  return 0;
}

