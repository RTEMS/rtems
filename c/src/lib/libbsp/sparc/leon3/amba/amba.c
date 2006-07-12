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

#include <leon.h>

#define amba_insert_device(tab, address) \
{ \
  if (*(address)) \
  { \
    (tab)->addr[(tab)->devnr] = (address); \
    (tab)->devnr ++; \
  } \
} while(0)


/* Structure containing address to devices found on the Amba Plug&Play bus */
amba_confarea_type amba_conf;

/* Pointers to Interrupt Controller configuration registers */
volatile LEON3_IrqCtrl_Regs_Map *LEON3_IrqCtrl_Regs;

/*
 *  bsp_leon3_predriver_hook
 *
 *  BSP predriver hook.  Called just before drivers are initialized.
 *  Used to scan system bus. Probes for AHB masters, AHB slaves and 
 *  APB slaves. Addresses to configuration areas of the AHB masters,
 *  AHB slaves, APB slaves and APB master are storeds in 
 *  amba_ahb_masters, amba_ahb_slaves and amba.
 */


void bsp_leon3_predriver_hook(void)
{
  unsigned int *cfg_area;  /* address to configuration area */
  unsigned int mbar, iobar, conf;
  int i, j;

  amba_conf.ahbmst.devnr = 0; amba_conf.ahbslv.devnr = 0; amba_conf.apbslv.devnr = 0;
  cfg_area = (unsigned int *) (LEON3_IO_AREA | LEON3_CONF_AREA);

  for (i = 0; i < LEON3_AHB_MASTERS; i++) 
  {
    amba_insert_device(&amba_conf.ahbmst, cfg_area);
    cfg_area += LEON3_AHB_CONF_WORDS;
  }

  cfg_area = (unsigned int *) (LEON3_IO_AREA | LEON3_CONF_AREA | LEON3_AHB_SLAVE_CONF_AREA);
  for (i = 0; i < LEON3_AHB_SLAVES; i++) 
  {
    amba_insert_device(&amba_conf.ahbslv, cfg_area);
    cfg_area += LEON3_AHB_CONF_WORDS;
  }  

  for (i = 0; i < amba_conf.ahbslv.devnr; i ++) 
  {
    conf = amba_get_confword(amba_conf.ahbslv, i, 0);
    mbar = amba_ahb_get_membar(amba_conf.ahbslv, i, 0);
    if ((amba_vendor(conf) == VENDOR_GAISLER) && (amba_device(conf) == GAISLER_APBMST))
    {
      amba_conf.apbmst = amba_membar_start(mbar);
      cfg_area = (unsigned int *) (amba_conf.apbmst | LEON3_CONF_AREA);
      for (j = amba_conf.apbslv.devnr; j < LEON3_APB_SLAVES; j++)
      {
	amba_insert_device(&amba_conf.apbslv, cfg_area);
	cfg_area += LEON3_APB_CONF_WORDS;
      }
    }
  }    

  /* Find LEON3 Interrupt controler */
  i = 0;
  while (i < amba_conf.apbslv.devnr) 
  {
    conf = amba_get_confword(amba_conf.apbslv, i, 0);
    if ((amba_vendor(conf) == VENDOR_GAISLER) && (amba_device(conf) == GAISLER_IRQMP))
    {
      iobar = amba_apb_get_membar(amba_conf.apbslv, i);
      LEON3_IrqCtrl_Regs = (volatile LEON3_IrqCtrl_Regs_Map *) amba_iobar_start(amba_conf.apbmst, iobar);
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
        amba_iobar_start(amba_conf.apbmst, iobar);
      break;
    }
    i++;
  }

}


