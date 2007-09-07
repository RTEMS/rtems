/*
 *  $Id$
 */

#include <bsp.h>
#include <libchip/smc91111exp.h>
#include <rtems/bspIo.h>


#define SMC91111_BASE_ADDR (void*)0x20000300
#define SMC91111_BASE_IRQ  4
#define SMC91111_BASE_PIO  4

scmv91111_configuration_t leon_scmv91111_configuration = {
  SMC91111_BASE_ADDR,                 /* base address */ 
    LEON_TRAP_TYPE (SMC91111_BASE_IRQ),	/* vector number */
  SMC91111_BASE_PIO,                  /* PIO */ 
    100,			/* 100b */
  1,                                  /* fulldx */
  1                                   /* autoneg */
};

int _rtems_smc91111_driver_attach (struct rtems_bsdnet_ifconfig *config,
				   scmv91111_configuration_t * scm_config);

/*
 * Attach an SMC91111 driver to the system
 */
int
rtems_smc91111_driver_attach_leon3 (struct rtems_bsdnet_ifconfig *config,
				    int attach)
{
  unsigned long addr_mctrl = 0;
  LEON3_IOPORT_Regs_Map *io;

  amba_apb_device apbpio;
  amba_ahb_device apbmctrl;
  
  if ( amba_find_apbslv(&amba_conf,VENDOR_GAISLER,GAISLER_PIOPORT,&apbpio) != 1 ){
    printk("SMC9111_leon3: didn't find PIO\n");
    return 0;
  }
  
  /* Find LEON2 memory controller */
  if ( amba_find_ahbslv(&amba_conf,VENDOR_ESA,ESA_MCTRL,&apbmctrl) != 1 ){
    /* LEON2 memory controller not found, search for fault tolerant memory controller */
    if ( amba_find_ahbslv(&amba_conf,VENDOR_GAISLER,GAISLER_FTMCTRL,&apbmctrl) != 1 ) {
      printk("SMC9111_leon3: didn't find any memory controller\n");
      return 0;
    }
  }
  
  /* Get  controller address */
  addr_mctrl = (unsigned long) apbmctrl.start;
  io = (LEON3_IOPORT_Regs_Map *) apbpio.start;
  
  printk(
        "Activating Leon3 io port for smsc_lan91cxx (pio:%x mctrl:%x)\n",
        (unsigned int)io,
        (unsigned int)addr_mctrl);
  
  /* Setup PIO IRQ */
  io->irqmask |= (1 << leon_scmv91111_configuration.pio);
  io->irqpol |= (1 << leon_scmv91111_configuration.pio);
  io->irqedge |= (1 << leon_scmv91111_configuration.pio);
  io->iodir &= ~(1 << leon_scmv91111_configuration.pio);

  /* Setup memory controller I/O waitstates */
  *((volatile unsigned int *) addr_mctrl) |= 0x10f80000;	/* enable I/O area access */

    return _rtems_smc91111_driver_attach (config,
            &leon_scmv91111_configuration);
};
