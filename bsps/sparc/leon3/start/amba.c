/*
 *  AMBA Plug & Play Bus Driver
 *
 *  This driver hook performs bus scanning.
 *
 *  COPYRIGHT (c) 2011.
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/fatal.h>
#include <leon.h>
#include <grlib/ambapp.h>
#include <rtems/sysinit.h>

unsigned int leon3_timer_prescaler __attribute__((weak)) = 0;
int leon3_timer_core_index __attribute__((weak)) = 0;

/* AMBA Plug&Play information description.
 *
 * After software has scanned AMBA PnP it builds a tree to make
 * it easier for drivers to work with the bus architecture.
 */
struct ambapp_bus ambapp_plb;

/* If RTEMS_DRVMGR_STARTUP is defined extra code is added that
 * registers the GRLIB AMBA PnP bus driver as root driver.
 */
#ifdef RTEMS_DRVMGR_STARTUP
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus_grlib.h>

extern void gptimer_register_drv (void);
extern void apbuart_cons_register_drv(void);
/* All drivers included by BSP, this is overridden by the user by including
 * the drvmgr_confdefs.h. By default the Timer and UART driver are included.
 */
drvmgr_drv_reg_func drvmgr_drivers[] __attribute__((weak)) =
{
  gptimer_register_drv,
  apbuart_cons_register_drv,
  NULL /* End array with NULL */
};

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
struct grlib_config grlib_bus_config =
{
  &ambapp_plb,              /* AMBAPP bus setup */
  &grlib_drv_resources,     /* Driver configuration */
};
#endif

rtems_interrupt_lock LEON3_IrqCtrl_Lock =
  RTEMS_INTERRUPT_LOCK_INITIALIZER("LEON3 IrqCtrl");

/* Pointers to Interrupt Controller configuration registers */
volatile struct irqmp_regs *LEON3_IrqCtrl_Regs;
struct ambapp_dev *LEON3_IrqCtrl_Adev;
volatile struct gptimer_regs *LEON3_Timer_Regs;
struct ambapp_dev *LEON3_Timer_Adev;

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
  int icsel;
  struct ambapp_dev *adev;

  /* Scan AMBA Plug&Play read-only information. The routine builds a PnP
   * tree into ambapp_plb in RAM, after this we never access the PnP
   * information in hardware directly any more.
   * Since on Processor Local Bus (PLB) memory mapping is 1:1
   */
  ambapp_scan(&ambapp_plb, LEON3_IO_AREA, NULL, NULL);

  /* Find LEON3 Interrupt controller */
  adev = (void *)ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                 VENDOR_GAISLER, GAISLER_IRQMP,
                                 ambapp_find_by_idx, NULL);
  if (adev == NULL) {
    /* PANIC IRQ controller not found!
     *
     *  What else can we do but stop ...
     */
    bsp_fatal(LEON3_FATAL_NO_IRQMP_CONTROLLER);
  }

  LEON3_IrqCtrl_Regs = (volatile struct irqmp_regs *)DEV_TO_APB(adev)->start;
  LEON3_IrqCtrl_Adev = adev;
  if ((LEON3_IrqCtrl_Regs->ampctrl >> 28) > 0) {
    /* IRQ Controller has support for multiple IRQ Controllers, each
     * CPU can be routed to different Controllers, we find out which
     * controller by looking at the IRQCTRL Select Register for this CPU.
     * Each Controller is located at a 4KByte offset.
     */
    icsel = LEON3_IrqCtrl_Regs->icsel[LEON3_Cpu_Index/8];
    icsel = (icsel >> ((7 - (LEON3_Cpu_Index & 0x7)) * 4)) & 0xf;
    LEON3_IrqCtrl_Regs += icsel;
  }
  LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = 0;
  LEON3_IrqCtrl_Regs->force[LEON3_Cpu_Index] = 0;
  LEON3_IrqCtrl_Regs->iclear = 0xffffffff;

  /* Init Extended IRQ controller if available */
  leon3_ext_irq_init();

  /* find GP Timer */
  adev = (void *)ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                 VENDOR_GAISLER, GAISLER_GPTIMER,
                                 ambapp_find_by_idx, &leon3_timer_core_index);
  if (adev) {
    LEON3_Timer_Regs = (volatile struct gptimer_regs *)DEV_TO_APB(adev)->start;
    LEON3_Timer_Adev = adev;

    /* Register AMBA Bus Frequency */
    ambapp_freq_init(
      &ambapp_plb,
      LEON3_Timer_Adev,
      (LEON3_Timer_Regs->scaler_reload + 1)
        * LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER
    );
    /* Set user prescaler configuration. Use this to increase accuracy of timer
     * and accociated services like cpucounter.
     * Note that minimum value is the number of timer instances present in
     * GRTIMER/GPTIMER hardware. See HW manual.
     */
    if (leon3_timer_prescaler)
      LEON3_Timer_Regs->scaler_reload = leon3_timer_prescaler;
  }

#ifdef RTEMS_DRVMGR_STARTUP
  /* Register Root bus, Use GRLIB AMBA PnP bus as root bus for LEON3 */
  ambapp_grlib_root_register(&grlib_bus_config);
#endif
}

RTEMS_SYSINIT_ITEM(
  amba_initialize,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_SECOND
);
