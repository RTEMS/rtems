/*
 *  RTEMS network driver for Blackfin embedded ethernet controller
 *
 *  COPYRIGHT (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *            written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _ethernet_h_
#define _ethernet_h_


#define BFIN_ETHERNET_DEBUG_NONE                0x0000
#define BFIN_ETHERNET_DEBUG_ALL                 0xFFFF

#define BFIN_ETHERNET_DEBUG  (BFIN_ETHERNET_DEBUG_NONE)


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
  uint32_t sclk;
  void *ethBaseAddress;
  void *rxdmaBaseAddress;
  void *txdmaBaseAddress;
  int rxDescCount;
  int txDescCount;
  enum {rmii, mii} phyType;
  int phyAddr;
} bfin_ethernet_configuration_t;


void bfin_ethernet_rxdma_isr(int vector);
void bfin_ethernet_txdma_isr(int vector);
void bfin_ethernet_mac_isr(int vector);

int bfin_ethernet_driver_attach(struct rtems_bsdnet_ifconfig *config,
                                int attaching,
                                bfin_ethernet_configuration_t *chip);


#ifdef __cplusplus
}
#endif


#endif /* _ethernet_h_ */

