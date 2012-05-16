/*  LEON3 BSP SMC91111 registration and low-level initialization
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <libchip/smc91111exp.h>
#include <rtems/bspIo.h>
#include <ambapp.h>

#define SMC91111_BASE_ADDR (void*)0x20000300
#define SMC91111_BASE_IRQ  4
#define SMC91111_BASE_PIO  4

scmv91111_configuration_t leon_scmv91111_configuration = {
  SMC91111_BASE_ADDR,                 /* base address */
  SMC91111_BASE_IRQ,                  /* IRQ number (on LEON vector is irq) */
  SMC91111_BASE_PIO,                  /* PIO */
  100,                                /* 100b */
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
  struct grgpio_regs *io;
  struct ambapp_apb_info apbpio;
  struct ambapp_apb_info apbmctrl;

  if (ambapp_find_apbslv(&ambapp_plb, VENDOR_GAISLER, GAISLER_GPIO, &apbpio)
      != 1) {
    printk("SMC9111_leon3: didn't find PIO\n");
    return 0;
  }

  /* In order to access the SMC controller the memory controller must have
   * I/O bus enabled. Find first memory controller.
   */
  if (ambapp_find_apbslv(&ambapp_plb, VENDOR_ESA, ESA_MCTRL, &apbmctrl) != 1) {
    if (ambapp_find_apbslv(&ambapp_plb, VENDOR_GAISLER, GAISLER_FTMCTRL,
                           &apbmctrl) != 1) {
      if (ambapp_find_apbslv(&ambapp_plb, VENDOR_GAISLER, GAISLER_FTSRCTRL,
                             &apbmctrl) != 1) {
        if (ambapp_find_apbslv(&ambapp_plb, VENDOR_GAISLER, GAISLER_FTSRCTRL8,
                               &apbmctrl) != 1) {
          printk("SMC9111_leon3: didn't find any memory controller\n");
          return 0;
        }
      }
    }
  }

  /* Get  controller address */
  addr_mctrl = (unsigned long) apbmctrl.start;
  io = (struct grgpio_regs *) apbpio.start;

  printk(
        "Activating Leon3 io port for smsc_lan91cxx (pio:%x mctrl:%x)\n",
        (unsigned int)io,
        (unsigned int)addr_mctrl);

  /* Setup PIO IRQ */
  io->imask |= (1 << leon_scmv91111_configuration.pio);
  io->ipol |= (1 << leon_scmv91111_configuration.pio);
  io->iedge |= (1 << leon_scmv91111_configuration.pio);
  io->dir &= ~(1 << leon_scmv91111_configuration.pio);

  /* Setup memory controller I/O waitstates */
  *((volatile unsigned int *) addr_mctrl) |= 0x10f80000;	/* enable I/O area access */

  return _rtems_smc91111_driver_attach(config, &leon_scmv91111_configuration);
};
