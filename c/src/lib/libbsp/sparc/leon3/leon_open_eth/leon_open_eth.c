/*
 *  LEON3 Opencores Ethernet MAC Configuration Information
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <bsp.h>
#include <libchip/open_eth.h>
#if (OPEN_ETH_DEBUG & OPEN_ETH_DEBUG_PRINT_REGISTERS)
#include <stdio.h>
#endif

/*
 * Default sizes of transmit and receive descriptor areas
 */
#define RDA_COUNT     16
#define TDA_COUNT     16

open_eth_configuration_t leon_open_eth_configuration;

int rtems_leon_open_eth_driver_attach(
  struct rtems_bsdnet_ifconfig *config,
  int attach
)
{
  unsigned int base_addr = 0; /* avoid warnings */
  unsigned int eth_irq = 0;   /* avoid warnings */
  struct ambapp_dev *adev;
  struct ambapp_ahb_info *ahb;

  /* Scan for MAC AHB slave interface */
  adev = (void *)ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_AHB_SLVS),
                                 VENDOR_OPENCORES, OPENCORES_ETHMAC,
                                 ambapp_find_by_idx, NULL);
  if (!adev) {
    adev = (void *)ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_AHB_SLVS),
                                   VENDOR_GAISLER, GAISLER_ETHAHB,
                                   ambapp_find_by_idx, NULL);
  }

  if (adev)
  {
    ahb = DEV_TO_AHB(adev);
    base_addr = ahb->start[0];
    eth_irq = ahb->irq;

    /* clear control register and reset NIC */
    *(volatile int *) base_addr = 0;
    *(volatile int *) base_addr = 0x800;
    *(volatile int *) base_addr = 0;
    leon_open_eth_configuration.base_address = base_addr;
    leon_open_eth_configuration.vector = eth_irq + 0x10;
    leon_open_eth_configuration.txd_count = TDA_COUNT;
    leon_open_eth_configuration.rxd_count = RDA_COUNT;
    /* enable 100 MHz operation only if cpu frequency >= 50 MHz */
    if (LEON3_Timer_Regs->scaler_reload >= 49)
      leon_open_eth_configuration.en100MHz = 1;
    if (rtems_open_eth_driver_attach( config, &leon_open_eth_configuration )) {
      LEON_Clear_interrupt(eth_irq);
      LEON_Unmask_interrupt(eth_irq);
    }
  }
  return 0;
}
