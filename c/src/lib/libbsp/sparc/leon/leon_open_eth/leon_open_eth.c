/*
 *  LEON2 Opencores Ethernet MAC Configuration Information
 *
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

/*
 * Default location of device registers
 */
#define OPEN_ETH_BASE_ADDRESS 0xb0000000

/*
 * Default interrupt vector
 */
#define OPEN_ETH_VECTOR 0x1C

open_eth_configuration_t leon_open_eth_configuration = {
  OPEN_ETH_BASE_ADDRESS,        /* base address */ 
  OPEN_ETH_VECTOR,              /* vector number */ 
  TDA_COUNT,                 /* number of transmit descriptors */
  RDA_COUNT                  /* number of receive descriptors */
};

int rtems_leon_open_eth_driver_attach(struct rtems_bsdnet_ifconfig *config)
{

  *(volatile int *) OPEN_ETH_BASE_ADDRESS = 0;
  *(volatile int *) OPEN_ETH_BASE_ADDRESS = 0x800;
  *(volatile int *) OPEN_ETH_BASE_ADDRESS = 0;
  if (rtems_open_eth_driver_attach( config, &leon_open_eth_configuration )) {
    LEON_REG.Interrupt_Clear = (1 << (OPEN_ETH_VECTOR - 0x10));
    LEON_REG.Interrupt_Mask  |= (1 << (OPEN_ETH_VECTOR - 0x10));
    return 1;
  }
  return 0;
}
