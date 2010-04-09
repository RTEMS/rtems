/**
 * @file
 *
 * @ingroup lpc_eth
 *
 * @brief Ethernet driver configuration.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC32XX_LPC_ETHERNET_CONFIG_H
#define LIBBSP_ARM_LPC32XX_LPC_ETHERNET_CONFIG_H

#include <stdlib.h>
#include <limits.h>

#include <rtems.h>
#include <rtems/malloc.h>

#include <bsp.h>
#include <bsp/lpc32xx.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc_eth Ethernet Support
 *
 * @ingroup lpc
 *
 * @brief Ethernet support.
 *
 * @{
 */

#define LPC_ETH_CONFIG_INTERRUPT LPC32XX_IRQ_ETHERNET

#define LPC_ETH_CONFIG_REG_BASE LPC32XX_BASE_ETHERNET

#define LPC_ETH_CONFIG_RX_UNIT_COUNT_DEFAULT 16
#define LPC_ETH_CONFIG_RX_UNIT_COUNT_MAX INT_MAX

#define LPC_ETH_CONFIG_TX_UNIT_COUNT_DEFAULT 32
#define LPC_ETH_CONFIG_TX_UNIT_COUNT_MAX INT_MAX

#define LPC_ETH_CONFIG_UNIT_MULTIPLE 8U

#ifdef LPC32XX_ETHERNET_RMII
  #define LPC_ETH_CONFIG_RMII

  static void lpc_eth_config_module_enable(void)
  {
    LPC32XX_MAC_CLK_CTRL = 0x1f;
  }
#else
  static void lpc_eth_config_module_enable(void)
  {
    LPC32XX_MAC_CLK_CTRL = 0x0f;
  }
#endif

#define LPC_ETH_CONFIG_USE_TRANSMIT_DMA

static char *lpc_eth_config_alloc_table_area(size_t size)
{
  return rtems_heap_allocate_aligned_with_boundary(size, 32, 0);
}

static void lpc_eth_config_free_table_area(char *table_area)
{
  /* FIXME: Type */
  free(table_area, (int) 0xdeadbeef);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_LPC_ETHERNET_CONFIG_H */
