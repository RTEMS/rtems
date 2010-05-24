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

/* Pointers to Interrupt Controller configuration registers */
volatile LEON3_IrqCtrl_Regs_Map *LEON3_IrqCtrl_Regs;

int LEON3_Cpu_Index = 0;

/*
 *  amba_initialize
 *
 *  Must be called just before drivers are initialized.
 *  Used to scan system bus. Probes for AHB masters, AHB slaves and
 *  APB slaves. Addresses to configuration areas of the AHB masters,
 *  AHB slaves, APB slaves and APB master are storeds in
 *  amba_ahb_masters, amba_ahb_slaves and amba.
 */

unsigned int getasr17(void);

asm(" .text  \n"
    "getasr17:   \n"
    "retl \n"
    "mov %asr17, %o0\n"
);


extern rtems_configuration_table Configuration;
extern int scan_uarts(void);

void amba_initialize(void)
{
  int i;
  amba_apb_device dev;

  /* Scan the AMBA Plug&Play info at the default LEON3 area */
  amba_scan(&amba_conf,LEON3_IO_AREA,NULL);

  /* Find LEON3 Interrupt controler */
  i = amba_find_apbslv(&amba_conf,VENDOR_GAISLER,GAISLER_IRQMP,&dev);
  if ( i > 0 ){
    /* Found APB IRQ_MP Interrupt Controller */
    LEON3_IrqCtrl_Regs = (volatile LEON3_IrqCtrl_Regs_Map *) dev.start;
#if defined(RTEMS_MULTIPROCESSING)
      if (rtems_configuration_get_user_multiprocessing_table() != NULL) {
        unsigned int tmp = getasr17();
        LEON3_Cpu_Index = (tmp >> 28) & 3;
      }
#endif
  }

  /* find GP Timer */
  i = amba_find_apbslv(&amba_conf,VENDOR_GAISLER,GAISLER_GPTIMER,&dev);
  if ( i > 0 ){
    LEON3_Timer_Regs = (volatile LEON3_Timer_Regs_Map *) dev.start;
  }

  /* find UARTS */
  scan_uarts();
}
