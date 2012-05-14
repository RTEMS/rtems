/*  networkconfig.c
 *
 *  This file contains the network driver attach function and configuration
 *  for the bf537Stamp.
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *            written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */



#include <bsp.h>
#include <rtems/rtems_bsdnet.h>
#include <libcpu/interrupt.h>
#include <libcpu/ethernet.h>


static bfin_ethernet_configuration_t ethConfig = {
  SCLK,
  (void *) EMAC_BASE_ADDRESS,
  (void *) DMA1_BASE_ADDRESS, /* ethernet receive */
  (void *) DMA2_BASE_ADDRESS, /* ethernet transmit */
  16, /* receive descriptors */
  16, /* transmit descriptors */
  mii, /* phy type */
  1 /* phy address */
};

static bfin_isr_t ethISRs[] = {
  /* careful - shared with a bunch of things */
/*
  {SIC_MAC_ERROR_VECTOR,
   bfin_ethernet_mac_isr,
   0,
   0,
   NULL},
*/
  /* careful - shared with porth irqa */
  {SIC_DMA1_MAC_RX_VECTOR,
   bfin_ethernet_rxdma_isr,
   0,
   0,
   NULL},
  /* careful - shared with porth irqb */
  {SIC_DMA2_MAC_TX_VECTOR,
   bfin_ethernet_txdma_isr,
   0,
   0,
   NULL}
};

int bf537Stamp_network_driver_attach(struct rtems_bsdnet_ifconfig *config,
                                     int attaching) {
  int result;
  int i;

  result = bfin_ethernet_driver_attach(config, attaching, &ethConfig);
  for (i = 0; i < sizeof(ethISRs) / sizeof(ethISRs[0]); i++) {
    bfin_interrupt_register(&ethISRs[i]);
    bfin_interrupt_enable(&ethISRs[i], TRUE);
  }

  return result;
}

