/**
 * @file
 *
 * @brief DWMAC 10/100/1000 Network Interface Controllers Core Handling
 *
 * DWMAC 10/100/1000 on-chip Synopsys IP Ethernet controllers.
 * Driver core handling.
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

#include "dwmac-core.h"
#include "dwmac-common.h"
#include "dwmac-regs.h"

#undef DWMAC_CORE_DEBUG
#ifdef DWMAC_CORE_DEBUG
#define DWMAC_CORE_PRINT_DBG( fmt, args ... )  printk( fmt, ## args )
#else
#define DWMAC_CORE_PRINT_DBG( fmt, args ... )  do { } while ( 0 )
#endif

/* DMA default interrupt masks */
#define DWMAC_CORE_INTR_ENABLE_DEFAULT_MASK_RX \
  ( \
    DMAGRP_INTERRUPT_ENABLE_NIE \
    | DMAGRP_INTERRUPT_ENABLE_RIE \
  )
#define DWMAC_CORE_INTR_ENABLE_DEFAULT_MASK_TX \
  ( \
    DMAGRP_INTERRUPT_ENABLE_NIE \
    | DMAGRP_INTERRUPT_ENABLE_FBE \
    | DMAGRP_INTERRUPT_ENABLE_UNE \
    | DMAGRP_INTERRUPT_ENABLE_AIE \
  )

#define DWMAC_CORE_INTR_ENABLE_ALL_MASK_TX \
  ( \
    DWMAC_CORE_INTR_ENABLE_DEFAULT_MASK_TX \
    | DMAGRP_INTERRUPT_ENABLE_TIE \
  )

#define DWMAC_CORE_INTR_STATUS_DEFAULT_MASK_RX \
  ( \
    DMAGRP_STATUS_NIS \
    | DMAGRP_STATUS_RI \
  )
#define DWMAC_CORE_INTR_STATUS_DEFAULT_MASK_TX \
  ( \
    DMAGRP_STATUS_NIS \
    | DMAGRP_STATUS_TI \
    | DMAGRP_STATUS_FBI \
    | DMAGRP_STATUS_UNF \
    | DMAGRP_STATUS_AIS \
  )

/* CSR1 enables the transmit DMA to check for new descriptor */
void dwmac_core_dma_restart_tx( dwmac_common_context *self )
{
  self->dmagrp->transmit_poll_demand = 1;
}

void dwmac_core_enable_dma_irq_tx_default( dwmac_common_context *self )
{
  self->dmagrp->interrupt_enable |= DWMAC_CORE_INTR_ENABLE_DEFAULT_MASK_TX;
}

void dwmac_core_enable_dma_irq_tx_transmitted( dwmac_common_context *self )
{
  self->dmagrp->interrupt_enable |= DMAGRP_INTERRUPT_ENABLE_TIE;
}

void dwmac_core_enable_dma_irq_rx( dwmac_common_context *self )
{
  self->dmagrp->interrupt_enable |= DWMAC_CORE_INTR_ENABLE_DEFAULT_MASK_RX;
}

void dwmac_core_disable_dma_irq_tx_all( dwmac_common_context *self )
{
  self->dmagrp->interrupt_enable &= ~DWMAC_CORE_INTR_ENABLE_ALL_MASK_TX;
}

void dwmac_core_disable_dma_irq_tx_transmitted( dwmac_common_context *self )
{
  self->dmagrp->interrupt_enable &= ~DMAGRP_INTERRUPT_ENABLE_TIE;
}

void dwmac_core_reset_dma_irq_status_tx( dwmac_common_context *self )
{
  self->dmagrp->status = DWMAC_CORE_INTR_STATUS_DEFAULT_MASK_TX;
}

void dwmac_core_reset_dma_irq_status_rx( dwmac_common_context *self )
{
  self->dmagrp->status = DWMAC_CORE_INTR_STATUS_DEFAULT_MASK_RX;
}

void dwmac_core_disable_dma_irq_rx( dwmac_common_context *self )
{
  self->dmagrp->interrupt_enable &= ~DWMAC_CORE_INTR_ENABLE_DEFAULT_MASK_RX;
}

void dwmac_core_dma_start_tx( dwmac_common_context *self )
{
  self->dmagrp->operation_mode |= DMAGRP_OPERATION_MODE_ST;
}

void dwmac_core_dma_stop_tx( dwmac_common_context *self )
{
  self->dmagrp->operation_mode &= ~DMAGRP_OPERATION_MODE_ST;
}

void dwmac_core_dma_start_rx( dwmac_common_context *self )
{
  self->dmagrp->operation_mode |= DMAGRP_OPERATION_MODE_SR;
}

void dwmac_core_dma_stop_rx( dwmac_common_context *self )
{
  self->dmagrp->operation_mode &= ~DMAGRP_OPERATION_MODE_SR;
}

void dwmac_core_dma_restart_rx( dwmac_common_context *self )
{
  self->dmagrp->receive_poll_demand = 1;
}

#ifdef DWMAC_CORE_DEBUG
static void show_tx_process_state( const uint32_t status )
{
  const uint32_t STATE = DMAGRP_STATUS_TS_GET( status );


  switch ( STATE ) {
    case 0:
      DWMAC_CORE_PRINT_DBG( "- TX (Stopped): Reset or Stop command\n" );
      break;
    case 1:
      DWMAC_CORE_PRINT_DBG( "- TX (Running):Fetching the Tx desc\n" );
      break;
    case 2:
      DWMAC_CORE_PRINT_DBG( "- TX (Running): Waiting for end of tx\n" );
      break;
    case 3:
      DWMAC_CORE_PRINT_DBG( "- TX (Running): Reading the data "
                            "and queuing the data into the Tx buf\n" );
      break;
    case 6:
      DWMAC_CORE_PRINT_DBG( "- TX (Suspended): Tx Buff Underflow "
                            "or an unavailable Transmit descriptor\n" );
      break;
    case 7:
      DWMAC_CORE_PRINT_DBG( "- TX (Running): Closing Tx descriptor\n" );
      break;
    default:
      break;
  }
}

static void show_rx_process_state( const uint32_t status )
{
  const uint32_t STATE = DMAGRP_STATUS_RS_GET( status );


  switch ( STATE ) {
    case 0:
      DWMAC_CORE_PRINT_DBG( "- RX (Stopped): Reset or Stop command\n" );
      break;
    case 1:
      DWMAC_CORE_PRINT_DBG( "- RX (Running): Fetching the Rx desc\n" );
      break;
    case 2:
      DWMAC_CORE_PRINT_DBG( "- RX (Running):Checking for end of pkt\n" );
      break;
    case 3:
      DWMAC_CORE_PRINT_DBG( "- RX (Running): Waiting for Rx pkt\n" );
      break;
    case 4:
      DWMAC_CORE_PRINT_DBG( "- RX (Suspended): Unavailable Rx buf\n" );
      break;
    case 5:
      DWMAC_CORE_PRINT_DBG( "- RX (Running): Closing Rx descriptor\n" );
      break;
    case 6:
      DWMAC_CORE_PRINT_DBG( "- RX(Running): Flushing the current frame"
                            " from the Rx buf\n" );
      break;
    case 7:
      DWMAC_CORE_PRINT_DBG( "- RX (Running): Queuing the Rx frame"
                            " from the Rx buf into memory\n" );
      break;
    default:
      break;
  }
}

#else /*  DWMAC_CORE_DEBUG */
  #define show_tx_process_state( status )
  #define show_rx_process_state( status )
#endif /* DWMAC_CORE_DEBUG */

void dwmac_core_dma_interrupt( void *arg )
{
  dwmac_common_context        *self            = (dwmac_common_context *) arg;
  dwmac_common_dma_irq_counts *count           = &self->stats.dma_irq_counts;
  rtems_event_set              events_receive  = 0;
  rtems_event_set              events_transmit = 0;

  /* Get interrupt status */
  uint32_t irq_status  = self->dmagrp->status & self->dmagrp->interrupt_enable;
  uint32_t irq_handled = 0;
  uint32_t irq_disable = 0;


  DWMAC_CORE_PRINT_DBG( "%s: [CSR5: 0x%08x]\n", __func__, irq_status );

  /* It displays the DMA process states (CSR5 register) if DWMAC_CORE_DEBUG is #defined */
  show_tx_process_state( self->dmagrp->status );
  show_rx_process_state( self->dmagrp->status );

  /* Is there any abnormal interrupt? */
  if ( irq_status & DMAGRP_STATUS_AIS ) {
    DWMAC_CORE_PRINT_DBG( "CSR5[15] DMA ABNORMAL IRQ: " );

    if ( irq_status & DMAGRP_STATUS_UNF ) {
      DWMAC_CORE_PRINT_DBG( "transmit underflow\n" );
      events_transmit |= DWMAC_COMMON_EVENT_TX_BUMP_UP_DMA_THRESHOLD;
      irq_handled     |= DMAGRP_STATUS_UNF;
      irq_disable     |= DMAGRP_INTERRUPT_ENABLE_UNE;
      ++count->tx_underflow;
    }

    if ( irq_status & DMAGRP_STATUS_TJT ) {
      DWMAC_CORE_PRINT_DBG( "transmit jabber\n" );
      irq_handled |= DMAGRP_STATUS_TJT;
      ++count->tx_jabber;
    }

    if ( irq_status & DMAGRP_STATUS_OVF ) {
      DWMAC_CORE_PRINT_DBG( "recv overflow\n" );
      irq_handled |= DMAGRP_STATUS_OVF;
      ++count->rx_overflow;
    }

    if ( irq_status & DMAGRP_STATUS_TU ) {
      DWMAC_CORE_PRINT_DBG( "transmit buffer unavailable\n" );
      irq_handled |= DMAGRP_STATUS_TU;
      ++count->tx_buf_unav;
    }

    if ( irq_status & DMAGRP_STATUS_RU ) {
      DWMAC_CORE_PRINT_DBG( "receive buffer unavailable\n" );
      irq_handled |= DMAGRP_STATUS_RU;
      ++count->rx_buf_unav;
    }

    if ( irq_status & DMAGRP_STATUS_RPS ) {
      DWMAC_CORE_PRINT_DBG( "receive process stopped\n" );
      irq_handled |= DMAGRP_STATUS_RPS;
      ++count->rx_process_stopped;
    }

    if ( irq_status & DMAGRP_STATUS_RWT ) {
      DWMAC_CORE_PRINT_DBG( "receive watchdog\n" );
      irq_handled |= DMAGRP_STATUS_RWT;
      ++count->rx_watchdog;
    }

    if ( irq_status & DMAGRP_STATUS_ETI ) {
      DWMAC_CORE_PRINT_DBG( "transmit early interrupt\n" );
      irq_handled |= DMAGRP_STATUS_ETI;
      ++count->tx_early;
    }

    if ( irq_status & DMAGRP_STATUS_ERI ) {
      DWMAC_CORE_PRINT_DBG( "receive early interrupt\n" );
      irq_handled |= DMAGRP_STATUS_ERI;
      ++count->rx_early;
    }

    if ( irq_status & DMAGRP_STATUS_TPS ) {
      DWMAC_CORE_PRINT_DBG( "transmit process stopped\n" );
      events_transmit |= DWMAC_COMMON_EVENT_TASK_INIT;
      irq_handled     |= DMAGRP_STATUS_TPS;
      irq_disable     |= DMAGRP_INTERRUPT_ENABLE_TSE;
      ++count->tx_process_stopped;
    }

    if ( irq_status & DMAGRP_STATUS_FBI ) {
      DWMAC_CORE_PRINT_DBG( "fatal bus error\n" );
      events_transmit |= DWMAC_COMMON_EVENT_TASK_INIT;
      irq_handled     |= DMAGRP_STATUS_FBI;
      irq_disable     |= DMAGRP_INTERRUPT_ENABLE_FBE;
      ++count->fatal_bus_error;
    }

    irq_handled |= DMAGRP_STATUS_AIS;
  }

  /* Is there any normal interrupt? */
  if ( irq_status & DMAGRP_STATUS_NIS ) {
    /* Transmit interrupt */
    if ( irq_status & DMAGRP_STATUS_TI ) {
      events_transmit |= DWMAC_COMMON_EVENT_TX_FRAME_TRANSMITTED;
      irq_handled     |= DMAGRP_STATUS_TI;
      irq_disable     |= DMAGRP_INTERRUPT_ENABLE_TIE;
      ++count->transmit;
    }

    /* Receive interrupt */
    if ( irq_status & DMAGRP_STATUS_RI ) {
      events_receive |= DWMAC_COMMON_EVENT_RX_FRAME_RECEIVED;
      irq_handled    |= DMAGRP_STATUS_RI;
      irq_disable    |= DMAGRP_INTERRUPT_ENABLE_RIE;
      ++count->receive;
    }

    irq_handled |= DMAGRP_STATUS_NIS;
  }

  /* Optional hardware blocks, interrupts should be disabled */
  if ( irq_status
       & ( DMAGRP_STATUS_GMI | DMAGRP_STATUS_GLI ) ) {
    DWMAC_CORE_PRINT_DBG( "%s: unexpected status %08x\n", __func__,
                          irq_status );

    if ( irq_status & DMAGRP_STATUS_GMI ) {
      irq_handled |= DMAGRP_STATUS_GMI;
      ++count->unhandled;
    }

    if ( irq_status & DMAGRP_STATUS_GLI ) {
      irq_handled |= DMAGRP_STATUS_GLI;
      ++count->unhandled;
    }
  }

  /* Count remaining unhandled interrupts (there should not be any)  */
  if ( ( irq_status & 0x1FFCF ) != irq_handled ) {
    ++count->unhandled;
  }

  /* Disable interrupts which need further handling by tasks.
   * The tasks will re-enable them. */
  self->dmagrp->interrupt_enable &= ~irq_disable;

  /* Clear interrupts */
  self->dmagrp->status = irq_handled;

  /* Send events to receive task */
  if ( events_receive != 0 ) {
    (void) rtems_bsdnet_event_send( self->task_id_rx, events_receive );
  }

  /* Send events to transmit task */
  if ( events_transmit != 0 ) {
    (void) rtems_bsdnet_event_send( self->task_id_tx, events_transmit );
  }

  DWMAC_CORE_PRINT_DBG( "\n\n" );
}

void dwmac_core_dma_flush_tx_fifo( dwmac_common_context *self )
{
  self->dmagrp->operation_mode |= DMAGRP_OPERATION_MODE_FTF;

  do {
  } while ( ( self->dmagrp->operation_mode & DMAGRP_OPERATION_MODE_FTF ) != 0 );
}

void dwmac_core_set_mac_addr(
  const uint8_t      addr[6],
  volatile uint32_t *reg_high,
  volatile uint32_t *reg_low )
{
  uint32_t data = MAC_HIGH_ADDRHI( ( addr[5] << 8 ) | addr[4] );


  /* For MAC Addr registers se have to set the Address Enable (AE)
   * bit that has no effect on the High Reg 0 where the bit 31 (MO)
   * is RO.
   */
  data     |= MAC_HIGH_AE;
  *reg_high = data;

  data      =
    ( (uint32_t) addr[3] << 24 )
    | ( (uint32_t) addr[2] << 16 )
    | ( (uint32_t) addr[1] << 8 )
    | addr[0];
  *reg_low = data;
}

/* Enable disable MAC RX/TX */
void dwmac_core_set_mac(
  dwmac_common_context *self,
  const bool            enable )
{
  uint32_t value = self->macgrp->mac_configuration;


  if ( enable ) {
    value |= MACGRP_MAC_CONFIGURATION_RE | MACGRP_MAC_CONFIGURATION_TE;
  } else {
    value &= ~( MACGRP_MAC_CONFIGURATION_RE | MACGRP_MAC_CONFIGURATION_TE );
  }

  self->macgrp->mac_configuration = value;
}
