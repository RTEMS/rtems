/*
 *  LEON3 Opencores Ethernet MAC Configuration Information
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  leon_open_eth.c,v 1.1.2.1 2005/10/05 19:26:00 joel Exp
 */


#include <bsp.h>
#include <libchip/greth.h>
/*#if (GRETH_DEBUG & GRETH_DEBUG_PRINT_REGISTERS)*/
#include <stdio.h>
/*#endif*/

/*
 * Default sizes of transmit and receive descriptor areas
 */
#define RDA_COUNT     32
#define TDA_COUNT     32

greth_configuration_t leon_greth_configuration; 

int rtems_leon_greth_driver_attach(
  struct rtems_bsdnet_ifconfig *config,
  int attach
)
{
  int device_found = 0;
  int i;
  unsigned int conf, iobar;
  unsigned int base_addr = 0; /* avoid warnings */
  unsigned int eth_irq = 0;   /* avoid warnings */

  /* Scan for MAC AHB slave interface */
  for (i = 0; i < amba_conf.apbslv.devnr; i++)
  {
    conf = amba_get_confword(amba_conf.apbslv, i, 0);
    if ((amba_vendor(conf) == VENDOR_GAISLER) &&
        (amba_device(conf) == GAISLER_ETHMAC))
    {
      iobar = amba_apb_get_membar(amba_conf.apbslv, i);
      base_addr = amba_iobar_start(amba_conf.apbmst, iobar);
      eth_irq = amba_irq(conf) + 0x10;
      device_found = 1;
      break;
    }
  }

  if (device_found) 
  {
    /* clear control register and reset NIC */
    *(volatile int *) base_addr = 0;
    *(volatile int *) base_addr = GRETH_CTRL_RST;
    *(volatile int *) base_addr = 0;
    leon_greth_configuration.base_address = base_addr;
    leon_greth_configuration.vector = eth_irq;
    leon_greth_configuration.txd_count = TDA_COUNT;
    leon_greth_configuration.rxd_count = RDA_COUNT;
    if (rtems_greth_driver_attach( config, &leon_greth_configuration )) {
      LEON_Clear_interrupt(leon_greth_configuration.vector);
      LEON_Unmask_interrupt(leon_greth_configuration.vector);
    } 
  }
  return 0;
}
