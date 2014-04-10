/**
 * @file
 *
 * @brief DWMAC 1000 on-chip Ethernet controllers DMA handling
 *
 * Functions and data which are specific to the DWMAC 1000 DMA Handling.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#include <errno.h>
#include "dwmac-core.h"
#include "dwmac-common.h"

typedef enum {
  DWMAC100_OPERATION_MODE_TTC_CONTROL_64  = 0x00000000,
  DWMAC100_OPERATION_MODE_TTC_CONTROL_128 = 0x00004000,
  DWMAC100_OPERATION_MODE_TTC_CONTROL_192 = 0x00008000,
  DWMAC100_OPERATION_MODE_TTC_CONTROL_256 = 0x0000c000,
  DWMAC100_OPERATION_MODE_TTC_CONTROL_40  = 0x00010000,
  DWMAC100_OPERATION_MODE_TTC_CONTROL_32  = 0x00014000,
  DWMAC100_OPERATION_MODE_TTC_CONTROL_24  = 0x00018000,
  DWMAC100_OPERATION_MODE_TTC_CONTROL_16  = 0x0001c000,
} dwmac1000_operation_mode_ttc_control;

typedef enum {
  DWMAC100_OPERATION_MODE_RTC_CONTROL_64  = 0x00000000,
  DWMAC100_OPERATION_MODE_RTC_CONTROL_32  = 0x00000008,
  DWMAC100_OPERATION_MODE_RTC_CONTROL_96  = 0x00000010,
  DWMAC100_OPERATION_MODE_RTC_CONTROL_128 = 0x00000018,
} dwmac1000_operation_mode_rtc_control;

static int dwmac1000_dma_init(
  dwmac_common_context *self,
  const uint32_t        pbl,
  const uint32_t        fb,
  const uint32_t        mb,
  const bool            use_enhanced_desc,
  const uint32_t        burst_len_4_support,
  const uint32_t        burst_len_8_support,
  const uint32_t        burst_len_16_support,
  const uint32_t        burst_boundary,
  volatile dwmac_desc  *dma_tx,
  volatile dwmac_desc  *dma_rx )
{
  int      eno   = 0;
  uint32_t value = self->dmagrp->bus_mode;
  int      limit = 10;


  /* DMA SW reset */
  value                 |= DMAGRP_BUS_MODE_SWR;
  self->dmagrp->bus_mode = value;

  while ( limit-- ) {
    if ( !( self->dmagrp->bus_mode & DMAGRP_BUS_MODE_SWR ) )
      break;

    rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 100 );
  }

  if ( limit < 0 ) {
    eno = EBUSY;
  } else {
    /*
     * Set the DMA PBL (Programmable Burst Length) mode
     */
    if ( pbl >= DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_8 ) {
      value =
        DMAGRP_BUS_MODE_PBL( ( pbl + 1 ) / 8 ) | DMAGRP_BUS_MODE_RPBL(
          ( pbl + 1 ) / 8 ) | DMAGRP_BUS_MODE_EIGHTXPBL;
    } else {
      value = DMAGRP_BUS_MODE_PBL( pbl + 1 ) | DMAGRP_BUS_MODE_RPBL( pbl + 1 );
    }

    /* Set the Fixed burst mode */
    if ( fb ) {
      value |= DMAGRP_BUS_MODE_FB;
    }

    /* Mixed Burst has no effect when fb is set */
    if ( mb ) {
      value |= DMAGRP_BUS_MODE_MB;
    }

    if ( use_enhanced_desc ) {
      value |= DMAGRP_BUS_MODE_ATDS;
    }

    self->dmagrp->bus_mode = value;

    /* In case of GMAC AXI configuration, program the DMA_AXI_BUS_MODE
     * for supported bursts.
     *
     * Note: This is applicable only for revision GMACv3.61a. For
     * older version this register is reserved and shall have no
     * effect.
     *
     * Note:
     *  For Fixed Burst Mode: if we directly write 0xFF to this
     *  register using the configurations pass from platform code,
     *  this would ensure that all bursts supported by core are set
     *  and those which are not supported would remain ineffective.
     *
     *  For Non Fixed Burst Mode: provide the maximum value of the
     *  burst length. Any burst equal or below the provided burst
     *  length would be allowed to perform. */
    value = self->dmagrp->axi_bus_mode;

    if ( burst_len_4_support ) {
      value |= DMAGRP_AXI_BUS_MODE_BLEND4;
    } else {
      value &= ~DMAGRP_AXI_BUS_MODE_BLEND4;
    }

    if ( burst_len_8_support ) {
      value |= DMAGRP_AXI_BUS_MODE_BLEND8;
    } else {
      value &= ~DMAGRP_AXI_BUS_MODE_BLEND8;
    }

    if ( burst_len_16_support ) {
      value |= DMAGRP_AXI_BUS_MODE_BLEND16;
    } else {
      value &= ~DMAGRP_AXI_BUS_MODE_BLEND16;
    }

    if ( burst_boundary ) {
      value |= DMAGRP_AXI_BUS_MODE_ONEKBBE;
    } else {
      value &= ~DMAGRP_AXI_BUS_MODE_ONEKBBE;
    }

    self->dmagrp->axi_bus_mode = value;

    /* Mask interrupts by writing to CSR7 */
    dwmac_core_enable_dma_irq_rx( self );
    dwmac_core_enable_dma_irq_tx_default( self );

    /* The base address of the RX/TX descriptor lists must be written into
     * DMA CSR3 and CSR4, respectively. */
    self->dmagrp->transmit_descr_list_addr = (uintptr_t) &dma_tx[0];
    self->dmagrp->receive_descr_list_addr  = (uintptr_t) &dma_rx[0];
  }

  return eno;
}

static void dwmac1000_dma_operation_mode(
  dwmac_common_context *self,
  const unsigned int    txmode,
  const unsigned int    rxmode )
{
  uint32_t value = self->dmagrp->operation_mode;


  if ( txmode == DWMAC_COMMON_DMA_MODE_STORE_AND_FORWARD ) {
    /* Transmit COE type 2 cannot be done in cut-through mode. */
    value |= DMAGRP_OPERATION_MODE_TSF;

    /* Operating on second frame increase the performance
     * especially when transmit store-and-forward is used.*/
    value |= DMAGRP_OPERATION_MODE_OSF;
  } else {
    value &= ~DMAGRP_OPERATION_MODE_TSF;
    value &= ~DMAGRP_OPERATION_MODE_TTC_GET( value );

    /* Set the transmit threshold */
    if ( txmode <= 32 ) {
      value |= DMAGRP_OPERATION_MODE_TTC_SET(
        value,
        DWMAC100_OPERATION_MODE_TTC_CONTROL_32
        );
    } else if ( txmode <= 64 ) {
      value = DMAGRP_OPERATION_MODE_TTC_SET(
        value,
        DWMAC100_OPERATION_MODE_TTC_CONTROL_64
        );
    } else if ( txmode <= 128 ) {
      value = DMAGRP_OPERATION_MODE_TTC_SET(
        value,
        DWMAC100_OPERATION_MODE_TTC_CONTROL_128
        );
    } else if ( txmode <= 192 ) {
      value = DMAGRP_OPERATION_MODE_TTC_SET(
        value,
        DWMAC100_OPERATION_MODE_TTC_CONTROL_192
        );
    } else {
      value = DMAGRP_OPERATION_MODE_TTC_SET(
        value,
        DWMAC100_OPERATION_MODE_TTC_CONTROL_256
        );
    }
  }

  if ( rxmode == DWMAC_COMMON_DMA_MODE_STORE_AND_FORWARD ) {
    value |= DMAGRP_OPERATION_MODE_RSF;
  } else {
    value &= ~DMAGRP_OPERATION_MODE_RSF;
    value &= DMAGRP_OPERATION_MODE_RTC_GET( value );

    if ( rxmode <= 32 ) {
      value = DMAGRP_OPERATION_MODE_RTC_SET(
        value,
        DWMAC100_OPERATION_MODE_RTC_CONTROL_32
        );
    } else if ( rxmode <= 64 ) {
      value = DMAGRP_OPERATION_MODE_RTC_SET(
        value,
        DWMAC100_OPERATION_MODE_RTC_CONTROL_64
        );
    } else if ( rxmode <= 96 ) {
      value = DMAGRP_OPERATION_MODE_RTC_SET(
        value,
        DWMAC100_OPERATION_MODE_RTC_CONTROL_96
        );
    } else {
      value = DMAGRP_OPERATION_MODE_RTC_SET(
        value,
        DWMAC100_OPERATION_MODE_RTC_CONTROL_128
        );
    }
  }

  self->dmagrp->operation_mode = value;
}

const dwmac_common_dma_ops dwmac_dma_ops_1000 = {
  .init     = dwmac1000_dma_init,
  .dma_mode = dwmac1000_dma_operation_mode,
};
