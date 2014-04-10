/**
 * @file
 *
 * @brief DWMAC 10/100/1000 Network Interface Controllers Core Handling
 *
 * DWMAC 10/100/1000 on-chip Synopsys IP Ethernet controllers.
 * Driver core handling.
 * This header file is NOT part of the driver API.
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef DWMAC_CORE_H_
#define DWMAC_CORE_H_

#include <stdint.h>
#include "dwmac-common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void dwmac_core_set_mac_addr(
  const uint8_t      addr[6],
  volatile uint32_t *reg_high,
  volatile uint32_t *reg_low );

void dwmac_core_set_mac(
  dwmac_common_context *self,
  const bool            enable );

void dwmac_core_dma_start_tx( dwmac_common_context *self );

void dwmac_core_dma_stop_tx( dwmac_common_context *self );

void dwmac_core_dma_start_rx( dwmac_common_context *self );

void dwmac_core_dma_stop_rx( dwmac_common_context *self );

void dwmac_core_dma_restart_rx( dwmac_common_context *self );

void dwmac_core_dma_restart_tx( dwmac_common_context *self );

void dwmac_core_enable_dma_irq_rx( dwmac_common_context *self );

void dwmac_core_enable_dma_irq_tx_default( dwmac_common_context *self );

void dwmac_core_enable_dma_irq_tx_transmitted( dwmac_common_context *self );

void dwmac_core_disable_dma_irq_tx_all( dwmac_common_context *self );

void dwmac_core_disable_dma_irq_tx_transmitted( dwmac_common_context *self );

void dwmac_core_disable_dma_irq_rx( dwmac_common_context *self );

void dwmac_core_reset_dma_irq_status_tx( dwmac_common_context *self );

void dwmac_core_reset_dma_irq_status_rx( dwmac_common_context *self );

void dwmac_core_dma_interrupt( void *arg );

void dwmac_core_dma_flush_tx_fifo( dwmac_common_context *self );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DWMAC_CORE_H_ */
