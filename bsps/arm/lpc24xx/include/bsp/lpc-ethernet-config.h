/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 *
 * @brief Ethernet driver configuration.
 */

/*
 * Copyright (C) 2009, 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
      #ifdef LPC24XX_PIN_ETHERNET_POWER_DOWN
        LPC24XX_PIN_ETHERNET_POWER_DOWN,
      #endif
      LPC24XX_PIN_ETHERNET_RMII_0,
      LPC24XX_PIN_ETHERNET_RMII_1,
      LPC24XX_PIN_ETHERNET_RMII_2,
      LPC24XX_PIN_ETHERNET_RMII_3,
      LPC24XX_PIN_TERMINAL
    };

    lpc24xx_module_enable(LPC24XX_MODULE_ETHERNET, LPC24XX_MODULE_PCLK_DEFAULT);
    lpc24xx_pin_config(&pins [0], LPC24XX_PIN_SET_FUNCTION);

    #ifdef LPC24XX_PIN_ETHERNET_POWER_DOWN
      {
        unsigned pin = lpc24xx_pin_get_first_index(&pins[0]);

        lpc24xx_gpio_config(pin, LPC24XX_GPIO_OUTPUT);
        lpc24xx_gpio_set(pin);
      }
    #endif
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

static void lpc_eth_config_module_disable(void)
{
  lpc24xx_module_disable(LPC24XX_MODULE_ETHERNET);
}

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
