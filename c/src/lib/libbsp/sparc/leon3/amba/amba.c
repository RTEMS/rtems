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
static int apb_init = 0;

/*
 *  bsp_leon3_predriver_hook
 *
 *  BSP predriver hook.  Called just before drivers are initialized.
 *  Used to scan system bus. Probes for AHB masters, AHB slaves and 
 *  APB slaves. Addresses to configuration areas of the AHB masters,
 *  AHB slaves, APB slaves and APB master are storeds in 
 *  amba_ahb_masters, amba_ahb_slaves and amba.
 */

unsigned int getasr17();

asm(" .text  \n"
    "getasr17:   \n"
    "retl \n"
    "mov %asr17, %o0\n"
);
    
      
extern rtems_configuration_table Configuration;

void bsp_leon3_predriver_hook(void)
{
  unsigned int iobar, conf;
  int i;
  unsigned int tmp;
	
	/* Scan the AMBA Plug&Play info at the default LEON3 area */
	amba_scan(&amba_conf,LEON3_IO_AREA,NULL);

  /* Find LEON3 Interrupt controler */
  i = 0;
  while (i < amba_conf.apbslv.devnr) 
  {
    conf = amba_get_confword(amba_conf.apbslv, i, 0);
    if ((amba_vendor(conf) == VENDOR_GAISLER) && (amba_device(conf) == GAISLER_IRQMP))
    {
      iobar = amba_apb_get_membar(amba_conf.apbslv, i);
      LEON3_IrqCtrl_Regs = (volatile LEON3_IrqCtrl_Regs_Map *) amba_iobar_start(amba_conf.apbslv.apbmst[i], iobar);
      /* asm("mov %%asr17, %0": : "r" (tmp)); */
      if (Configuration.User_multiprocessing_table != NULL)
      {	
	      tmp = getasr17();
       	LEON3_Cpu_Index = (tmp >> 28) & 3;
      }
      break;
    }
    i++;
  }
  /* find GP Timer */
  i = 0;
 while (i < amba_conf.apbslv.devnr) 
  {
    conf = amba_get_confword(amba_conf.apbslv, i, 0);
    if ((amba_vendor(conf) == VENDOR_GAISLER) &&
       (amba_device(conf) == GAISLER_GPTIMER)) {
      iobar = amba_apb_get_membar(amba_conf.apbslv, i);      
      LEON3_Timer_Regs = (volatile LEON3_Timer_Regs_Map *)
      amba_iobar_start(amba_conf.apbslv.apbmst[i], iobar);
      break;
    }
    i++;
  }

}
