/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  AMBA Plug & Play Bus Driver
 *
 *  This driver hook performs bus scanning.
 *
 *  COPYRIGHT (c) 2011.
 *  Aeroflex Gaisler
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
#include <bsp/fatal.h>
#include <leon.h>
#include <grlib/ambapp.h>
#include <rtems/score/memory.h>
#include <rtems/sysinit.h>

#include <string.h>

#if !defined(LEON3_GPTIMER_BASE)
unsigned int leon3_timer_prescaler __attribute__((weak)) = 0;
int leon3_timer_core_index __attribute__((weak)) = 0;
#endif

/* AMBA Plug&Play information description.
 *
 * After software has scanned AMBA PnP it builds a tree to make
 * it easier for drivers to work with the bus architecture.
 */
static struct ambapp_bus ambapp_plb_instance;

static void *ambapp_plb_alloc( size_t size )
{
  return _Memory_Allocate( _Memory_Get(), size, CPU_HEAP_ALIGNMENT );
}

struct ambapp_bus *ambapp_plb( void )
{
  struct ambapp_bus *plb;

  plb = &ambapp_plb_instance;

  if ( plb->root == NULL ) {
    struct ambapp_context ctx;

    ctx.copy_from_device = (ambapp_memcpy_t) memcpy;
    ctx.alloc = ambapp_plb_alloc;

    /* Scan AMBA Plug&Play read-only information. The routine builds a PnP
     * tree into ambapp_plb in RAM, after this we never access the PnP
     * information in hardware directly any more.
     * Since on Processor Local Bus (PLB) memory mapping is 1:1
     */
    ambapp_scan( plb, LEON3_IO_AREA, &ctx, NULL );
  }

  return plb;
}

/* If RTEMS_DRVMGR_STARTUP is defined extra code is added that
 * registers the GRLIB AMBA PnP bus driver as root driver.
 */
#ifdef RTEMS_DRVMGR_STARTUP
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus_grlib.h>

/* Driver resources configuration for AMBA root bus. It is declared weak
 * so that the user may override it, if the defualt settings are not
 * enough.
 */
struct drvmgr_bus_res grlib_drv_resources __attribute__((weak)) =
{
  .next = NULL,
  .resource =
  {
    DRVMGR_RES_EMPTY,
  }
};

/* GRLIB AMBA bus configuration (the LEON3 root bus configuration) */
struct grlib_config grlib_bus_config;

static void ambapp_grlib_root_initialize( void )
{
  /* Register Root bus, Use GRLIB AMBA PnP bus as root bus for LEON3 */
  grlib_bus_config.abus = ambapp_plb();
  grlib_bus_config.resources = &grlib_drv_resources;
  ambapp_grlib_root_register(&grlib_bus_config);
}

RTEMS_SYSINIT_ITEM(
  ambapp_grlib_root_initialize,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_SECOND
);
#endif

#if !defined(LEON3_IRQAMP_BASE)
irqamp *LEON3_IrqCtrl_Regs;
struct ambapp_dev *LEON3_IrqCtrl_Adev;
#endif

#if !defined(LEON3_GPTIMER_BASE)
gptimer *LEON3_Timer_Regs;
struct ambapp_dev *LEON3_Timer_Adev;
#endif

/*
 *  amba_initialize
 *
 *  Must be called just before drivers are initialized.
 *  Used to scan system bus. Probes for AHB masters, AHB slaves and
 *  APB slaves. Addresses to configuration areas of the AHB masters,
 *  AHB slaves, APB slaves and APB master are storeds in
 *  amba_ahb_masters, amba_ahb_slaves and amba.
 */

static void amba_initialize(void)
{
  struct ambapp_dev *adev;
  struct ambapp_bus *plb;

  plb = ambapp_plb();
#if defined(LEON3_IRQAMP_BASE) && defined(LEON3_GPTIMER_BASE)
  (void) plb;
  (void) adev;
#endif

#if !defined(LEON3_IRQAMP_BASE)
  /* Find LEON3 Interrupt controller */
  adev = (void *)ambapp_for_each(plb, (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                 VENDOR_GAISLER, GAISLER_IRQMP,
                                 ambapp_find_by_idx, NULL);
  if (adev == NULL) {
    /* PANIC IRQ controller not found!
     *
     *  What else can we do but stop ...
     */
    bsp_fatal(LEON3_FATAL_NO_IRQMP_CONTROLLER);
  }

  LEON3_IrqCtrl_Regs = (irqamp *)DEV_TO_APB(adev)->start;
  LEON3_IrqCtrl_Adev = adev;
  if ((grlib_load_32(&LEON3_IrqCtrl_Regs->asmpctrl) >> 28) > 0) {
    uint32_t icsel;

    /* IRQ Controller has support for multiple IRQ Controllers, each
     * CPU can be routed to different Controllers, we find out which
     * controller by looking at the IRQCTRL Select Register for this CPU.
     * Each Controller is located at a 4KByte offset.
     */
    icsel = grlib_load_32(&LEON3_IrqCtrl_Regs->icselr[LEON3_Cpu_Index/8]);
    icsel = (icsel >> ((7 - (LEON3_Cpu_Index & 0x7)) * 4)) & 0xf;
    LEON3_IrqCtrl_Regs += icsel;
  }
#endif

#if !defined(LEON3_GPTIMER_BASE)
  /* find GP Timer */
  adev = (void *)ambapp_for_each(plb, (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                 VENDOR_GAISLER, GAISLER_GPTIMER,
                                 ambapp_find_by_idx, &leon3_timer_core_index);
  if (adev) {
    LEON3_Timer_Regs = (gptimer *)DEV_TO_APB(adev)->start;
    LEON3_Timer_Adev = adev;

    /* Register AMBA Bus Frequency */
    ambapp_freq_init(
      plb,
      LEON3_Timer_Adev,
      (grlib_load_32(&LEON3_Timer_Regs->sreload) + 1)
        * LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER
    );
    /* Set user prescaler configuration. Use this to increase accuracy of timer
     * and accociated services like cpucounter.
     * Note that minimum value is the number of timer instances present in
     * GRTIMER/GPTIMER hardware. See HW manual.
     */
    if (leon3_timer_prescaler)
      grlib_store_32(&LEON3_Timer_Regs->sreload, leon3_timer_prescaler);
  }
#endif
}

RTEMS_SYSINIT_ITEM(
  amba_initialize,
  RTEMS_SYSINIT_BSP_EARLY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
