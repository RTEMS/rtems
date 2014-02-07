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
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <ambapp.h>

/* AMBA Plug&Play information description.
 *
 * After software has scanned AMBA PnP it builds a tree to make
 * it easier for drivers to work with the bus architecture.
 */
struct ambapp_bus ambapp_plb;

/* GRLIB extended IRQ controller register */
extern void leon3_ext_irq_init(void);

/* Pointers to Interrupt Controller configuration registers */
volatile struct irqmp_regs *LEON3_IrqCtrl_Regs;

/*
 *  amba_initialize
 *
 *  Must be called just before drivers are initialized.
 *  Used to scan system bus. Probes for AHB masters, AHB slaves and
 *  APB slaves. Addresses to configuration areas of the AHB masters,
 *  AHB slaves, APB slaves and APB master are storeds in
 *  amba_ahb_masters, amba_ahb_slaves and amba.
 */

void amba_initialize(void)
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
    asm volatile( "mov 1, %g1; ta 0x0" );
  }

  LEON3_IrqCtrl_Regs = (volatile struct irqmp_regs *)DEV_TO_APB(adev)->start;
  if ((LEON3_IrqCtrl_Regs->ampctrl >> 28) > 0) {
    /* IRQ Controller has support for multiple IRQ Controllers, each
     * CPU can be routed to different Controllers, we find out which
     * controller by looking at the IRQCTRL Select Register for this CPU.
     * Each Controller is located at a 4KByte offset.
     */
    icsel = LEON3_IrqCtrl_Regs->icsel[LEON3_Cpu_Index/8];
    icsel = (icsel >> ((7 - (LEON3_Cpu_Index & 0x7)) * 4)) & 0xf;
    LEON3_IrqCtrl_Regs += icsel;
    LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = 0;
    LEON3_IrqCtrl_Regs->force[LEON3_Cpu_Index] = 0;
    LEON3_IrqCtrl_Regs->iclear = 0xffffffff;
  }

  /* Init Extended IRQ controller if available */
  leon3_ext_irq_init();

  /* find GP Timer */
  adev = (void *)ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                 VENDOR_GAISLER, GAISLER_GPTIMER,
                                 ambapp_find_by_idx, NULL);
  if (adev) {
    LEON3_Timer_Regs = (volatile struct gptimer_regs *)DEV_TO_APB(adev)->start;

    /* Register AMBA Bus Frequency */
    ambapp_freq_init(&ambapp_plb, adev,
                     (LEON3_Timer_Regs->scaler_reload + 1) * 1000000);
  }
}
