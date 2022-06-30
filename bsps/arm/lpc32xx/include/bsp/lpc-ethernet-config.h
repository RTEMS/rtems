/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup lpc_eth
 *
 * @brief Ethernet driver configuration.
 */

/*
 * Copyright (c) 2009 embedded brains GmbH & Co. KG
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
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief Ethernet support.
 *
 * @{
 */

#define LPC_ETH_CONFIG_INTERRUPT LPC32XX_IRQ_ETHERNET

#define LPC_ETH_CONFIG_REG_BASE LPC32XX_BASE_ETHERNET

#define LPC_ETH_CONFIG_RX_UNIT_COUNT_DEFAULT 16
#define LPC_ETH_CONFIG_RX_UNIT_COUNT_MAX INT_MAX

#define LPC_ETH_CONFIG_TX_UNIT_COUNT_DEFAULT 128
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

static void lpc_eth_config_module_disable(void)
{
  LPC32XX_MAC_CLK_CTRL = 0;
}

#define LPC_ETH_CONFIG_USE_TRANSMIT_DMA

static char *lpc_eth_config_alloc_table_area(size_t size)
{
  return rtems_heap_allocate_aligned_with_boundary(size, 32, 0);
}

static void lpc_eth_config_free_table_area(char *table_area)
{
  /* FIXME: Type */
  free(table_area, NULL);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_LPC_ETHERNET_CONFIG_H */
