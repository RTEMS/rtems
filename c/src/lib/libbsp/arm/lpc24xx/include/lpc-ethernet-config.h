/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Ethernet driver configuration.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC24XX_LPC_ETHERNET_CONFIG_H
#define LIBBSP_ARM_LPC24XX_LPC_ETHERNET_CONFIG_H

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/lpc24xx.h>

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LPC_ETH_CONFIG_INTERRUPT LPC24XX_IRQ_ETHERNET

#define LPC_ETH_CONFIG_REG_BASE MAC_BASE_ADDR

#ifdef ARM_MULTILIB_ARCH_V4
  #define LPC_ETH_CONFIG_RX_UNIT_COUNT_DEFAULT 16
  #define LPC_ETH_CONFIG_RX_UNIT_COUNT_MAX 54

  #define LPC_ETH_CONFIG_TX_UNIT_COUNT_DEFAULT 10
  #define LPC_ETH_CONFIG_TX_UNIT_COUNT_MAX 10

  #define LPC_ETH_CONFIG_UNIT_MULTIPLE 1U

  #define LPC24XX_ETH_RAM_BEGIN 0x7fe00000U
  #define LPC24XX_ETH_RAM_SIZE (16U * 1024U)
#else
  #define LPC_ETH_CONFIG_RX_UNIT_COUNT_DEFAULT 16
  #define LPC_ETH_CONFIG_RX_UNIT_COUNT_MAX INT_MAX

  #define LPC_ETH_CONFIG_TX_UNIT_COUNT_DEFAULT 32
  #define LPC_ETH_CONFIG_TX_UNIT_COUNT_MAX INT_MAX

  #define LPC_ETH_CONFIG_UNIT_MULTIPLE 8U

  #define LPC_ETH_CONFIG_USE_TRANSMIT_DMA

  #define LPC24XX_ETH_RAM_BEGIN 0x20000000U
  #define LPC24XX_ETH_RAM_SIZE (32U * 1024U)
#endif

#ifdef LPC24XX_ETHERNET_RMII
  #define LPC_ETH_CONFIG_RMII

  static void lpc_eth_config_module_enable(void)
  {
    static const lpc24xx_pin_range pins [] = {
      LPC24XX_PIN_ETHERNET_RMII_0,
      LPC24XX_PIN_ETHERNET_RMII_1,
      LPC24XX_PIN_ETHERNET_RMII_2,
      LPC24XX_PIN_ETHERNET_RMII_3,
      LPC24XX_PIN_TERMINAL
    };

    lpc24xx_module_enable(LPC24XX_MODULE_ETHERNET, LPC24XX_MODULE_PCLK_DEFAULT);
    lpc24xx_pin_config(&pins [0], LPC24XX_PIN_SET_FUNCTION);
  }
#else
  static void lpc_eth_config_module_enable(void)
  {
    static const lpc24xx_pin_range pins [] = {
      LPC24XX_PIN_ETHERNET_MII,
      LPC24XX_PIN_TERMINAL
    };

    lpc24xx_module_enable(LPC24XX_MODULE_ETHERNET, LPC24XX_MODULE_PCLK_DEFAULT);
    lpc24xx_pin_config(&pins [0], LPC24XX_PIN_SET_FUNCTION);
  }
#endif

static char *lpc_eth_config_alloc_table_area(size_t size)
{
  if (size < LPC24XX_ETH_RAM_SIZE) {
    return (char *) LPC24XX_ETH_RAM_BEGIN;
  } else {
    return NULL;
  }
}

static void lpc_eth_config_free_table_area(char *table_area)
{
  /* Do nothing */
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_LPC_ETHERNET_CONFIG_H */
