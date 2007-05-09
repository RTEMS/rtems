/*
 *  $Id$
 */

#include <bsp.h>
#include <libchip/smc91111exp.h>
#include <stdio.h>


#define SMC91111_BASE_ADDR (void*)0x20000300
#define SMC91111_BASE_IRQ  4
#define SMC91111_BASE_PIO  4

scmv91111_configuration_t leon_scmv91111_configuration = {
  SMC91111_BASE_ADDR,                 /* base address */ 
  LEON_TRAP_TYPE(SMC91111_BASE_IRQ),  /* vector number */ 
  SMC91111_BASE_PIO,                  /* PIO */ 
  10,                                 /* 10b */
  1,                                  /* fulldx */
  1                                   /* autoneg */
};

int _rtems_smc91111_driver_attach(
  struct rtems_bsdnet_ifconfig *config,
  scmv91111_configuration_t    *scm_config
);

/*
 * Attach an SMC91111 driver to the system
 */
int rtems_smc91111_driver_attach_leon3 (
  struct rtems_bsdnet_ifconfig *config,
  int                           attach
)
{
  unsigned int iobar, conf,i;

  {
    unsigned long irq_pio, irq_mctrl, addr_pio = 0;
    unsigned long addr_mctrl = 0, addr_timer = 0;

    i = 0;
    while (i < amba_conf.apbslv.devnr) 
      {
        conf = amba_get_confword(amba_conf.apbslv, i, 0);
        if ((amba_vendor(conf) == VENDOR_GAISLER) &&
            (amba_device(conf) == GAISLER_PIOPORT))
    {
      irq_pio = amba_irq(conf);
      iobar = amba_apb_get_membar(amba_conf.apbslv, i);      
      addr_pio = (unsigned long) amba_iobar_start(amba_conf.apbmst, iobar);
    }
        else if ((amba_vendor(conf) == VENDOR_ESA) &&
                 (amba_device(conf) == ESA_MCTRL))
    {
      irq_mctrl = amba_irq(conf);
      iobar = amba_apb_get_membar(amba_conf.apbslv, i);      
      addr_mctrl = (unsigned long) amba_iobar_start(amba_conf.apbmst, iobar);
    }
        else if ((amba_vendor(conf) == VENDOR_GAISLER) &&
                 (amba_device(conf) == GAISLER_GPTIMER))
    {
      iobar = amba_apb_get_membar(amba_conf.apbslv, i);      
      addr_timer = (unsigned long) amba_iobar_start(amba_conf.apbmst, iobar);
    }
        i++;
      }

    if (addr_timer) {
      LEON3_Timer_Regs_Map *timer = (LEON3_Timer_Regs_Map *)addr_timer;
      if (timer->scaler_reload >= 49)
        leon_scmv91111_configuration.ctl_rspeed = 100;
    }
    
    if (addr_pio && addr_mctrl) {
      
      LEON3_IOPORT_Regs_Map *io = (LEON3_IOPORT_Regs_Map *) addr_pio;
      printk(
        "Activating Leon3 io port for smsc_lan91cxx (pio:%x mctrl:%x)\n",
        (unsigned int)addr_pio,
        (unsigned int)addr_mctrl);
      
      *((volatile unsigned int *)addr_mctrl) |= 0x10f80000;  /*mctrl ctrl 1 */
      io->irqmask |= (1 << leon_scmv91111_configuration.pio);
      io->irqpol |= (1 << leon_scmv91111_configuration.pio);
      io->irqedge |= (1 << leon_scmv91111_configuration.pio);
      io->iodir &= ~(1 << leon_scmv91111_configuration.pio);
    } else {
      return 0;
    }
  }
  
  return _rtems_smc91111_driver_attach(config,&leon_scmv91111_configuration);
};

