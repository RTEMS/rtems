/*
 *  AMBA Plag & Play Bus Driver
 *
 *  This driver hook performs bus scanning.
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>

/* Structure containing address to devices found on the Amba Plug&Play bus */
amba_confarea_type amba_conf;

/* GRLIB extended IRQ controller register */
extern void leon3_ext_irq_init(void);

/* Pointers to Interrupt Controller configuration registers */
volatile LEON3_IrqCtrl_Regs_Map *LEON3_IrqCtrl_Regs;

/*
 *  amba_initialize
 *
 *  Must be called just before drivers are initialized.
 *  Used to scan system bus. Probes for AHB masters, AHB slaves and
 *  APB slaves. Addresses to configuration areas of the AHB masters,
 *  AHB slaves, APB slaves and APB master are storeds in
 *  amba_ahb_masters, amba_ahb_slaves and amba.
 */

extern int scan_uarts(void);

void amba_initialize(void)
{
  int i;
  amba_apb_device dev;

  /* Scan the AMBA Plug&Play info at the default LEON3 area */
  amba_scan(&amba_conf,LEON3_IO_AREA,NULL);

  /* Find LEON3 Interrupt controller */
  i = amba_find_apbslv(&amba_conf,VENDOR_GAISLER,GAISLER_IRQMP,&dev);
  if (i <= 0){
    /* PANIC IRQ controller not found!
     *
     *  What else can we do but stop ...
     */
    asm volatile( "mov 1, %g1; ta 0x0" );
  }

  LEON3_IrqCtrl_Regs = (volatile LEON3_IrqCtrl_Regs_Map *) dev.start;

  /* Init Extended IRQ controller if available */
  leon3_ext_irq_init();

  /* find GP Timer */
  i = amba_find_apbslv(&amba_conf,VENDOR_GAISLER,GAISLER_GPTIMER,&dev);
  if ( i > 0 ){
    LEON3_Timer_Regs = (volatile LEON3_Timer_Regs_Map *) dev.start;
  }

  /* find UARTS */
  scan_uarts();
}
