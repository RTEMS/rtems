/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup SJA1000
 *
 * @brief This source file contains the implementation of SJA1000
 *   controller.
 */

/*
 * Copyright (C) 2025-2026 Michal Lenc <michallenc@seznam.cz> self-funded
 * Copyright (C) 2025-2026 Pavel Pisa <pisa@cmp.felk.cvut.cz> self-funded
 * Implementation is based on original LinCAN SJA1000 driver
 * Copyright (C) 2002-2009 DCE FEE CTU Prague <http://dce.felk.cvut.cz>
 * Copyright (C) 2002-2009 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 * Copyright (C) 2004-2005 Tomasz Motylewski (BFAD GmbH)
 * Funded by OCERA and FRESCOR IST projects
 * Based on CAN driver code by Arnaud Westenberg <arnaud@wanadoo.nl>
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

#include <stdatomic.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <stdbool.h>
#include <endian.h>
#include <stdlib.h>
#include <string.h>

/* FIXME find or implement portable I/O mapped devices access routines */
#if 0 && defined(__i386__)
#define RTEMS_CAN_SJA1000_ARCH_WITH_IO_PORTS 1
#include "i386_io.h"
#endif

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/score/basedefs.h>

#include <dev/can/can-devcommon.h>
#include <dev/can/can-helpers.h>
#include <dev/can/can.h>

#include <dev/can/sja1000.h>

#include "sja1000_regs.h"

#ifdef RTEMS_DEBUG
# define SJA1000_DEBUG 1
#else
# undef SJA1000_DEBUG
#endif

#define CAN_SJA1000_TYPE "sja1000"

/**
 * @brief This represents one HW transmission buffer.
 */
struct sja1000_txb_info {
  /**
   * @brief This holds the pointer to the slot the buffer acquired.
   *
   * The stack does not free the slot immediately after it is assigned to
   * the buffer, but waits for successful transmission (or error/abort). Hence
   * the buffer has to keep the information about the slot it acquired to
   * provide correct error reporting or to schedule it for later processing.
   */
  struct rtems_can_queue_slot *slot;
  /**
   * @brief This holds the pointer to the edge from which the slot was
   *  acquired.
   *
   * This information is useful for future filtering of echo frames back to
   * sending instances.
   */
  struct rtems_can_queue_edge *edge;
  int prio;
};

/**
 * @brief This represents internal SJA1000 structure.
 *
 * This structure is SJA1000 controller dependent and provides
 * access to non generic parts that do not belong to generic
 * @ref rtems_can_chip structure.
 */
struct sja1000_internal {
  /**
   * @brief Base memory address.
   */
  uintptr_t base;
  /**
   * @brief This member holds the enabled interrupts.
   */
  uint8_t int_ena;
  /**
   * @brief This member holds the interrupt status register value.
   */
  atomic_uint isr;
  /*
   * @brief This member holds the register mapping hardware options given
   * during the chip initialization..
   */
  unsigned int hw_options;
  /**
   * @brief This member holds the  @ref sja1000_txb_info structures.
   */
  struct sja1000_txb_info txb_info;
  /**
   * @brief This member holds one  @ref can_frame .
   */
  struct can_frame frame;
  /**
   * @brief This member indicates whether frame member has a frame to
   * be passed to the upper layer in worker thread.
   */
  atomic_bool frame_to_pass;
  /**
   * @brief This member holds the number of interrupt loops to monitor
   * IRQ stuck condition.
   */
  atomic_int irq_loops;
  /**
   * @brief This member indicates whether the interrupt was temporarily
   * disabled because of IRQ stuck condition.
   */
  atomic_bool irq_emergency_disable;
  /**
   * @brief This member provides the worker thread with the information
   * chip stop is required.
   */
  atomic_bool do_stop;
};

static const struct rtems_can_bittiming_const sja1000_bittiming_const = {
  .name = "sja1000",
  .tseg1_min = 1,
  .tseg1_max = 16,
  .tseg2_min = 1,
  .tseg2_max = 8,
  .sjw_max = 4,
  .brp_min = 2,
  .brp_max = 128,
  .brp_inc = 2,
};

static inline void sja1000_write_reg(
  struct sja1000_internal * restrict internal,
  enum sja1000_regs reg,
  uint8_t val
)
{
  reg <<= internal->hw_options & RTEMS_SJA1000_HW_REG_SPAN_MASK;
#ifdef RTEMS_CAN_SJA1000_ARCH_WITH_IO_PORTS
  if ( internal->hw_options & RTEMS_SJA1000_HW_REG_IO_PORT ) {
    outb( val, internal->base + reg );
  } else
#endif
  {
    *( volatile uint8_t * )( internal->base + reg ) = val;
  }
}

static inline uint8_t sja1000_read_reg(
  struct sja1000_internal * restrict internal,
  enum sja1000_regs reg
)
{
  reg <<= internal->hw_options & RTEMS_SJA1000_HW_REG_SPAN_MASK;
#ifdef RTEMS_CAN_SJA1000_ARCH_WITH_IO_PORTS
  if ( internal->hw_options & RTEMS_SJA1000_HW_REG_IO_PORT ) {
    return inb( internal->base + reg );
  } else
#endif
  {
    return *( volatile uint8_t * )( internal->base + reg );
  }
}

/**
 * @brief This function enables the configuration mode.
 *
 * This operation brings the controller to configuration mode
 * and stops run mode.
 *
 * @param  internal Pointer to internal SJA1000 structure
 *
 * @return 0 on success, negatted ETIMEDOUT otherwise,
 */
static int sja1000_enable_configuration( struct sja1000_internal *internal )
{
  uint8_t regval;
  int i = 100;

  regval = sja1000_read_reg( internal, SJA1000_MODE );
  while ( i ) {
    sja1000_write_reg( internal , SJA1000_MODE, regval | REG_MODE_RM );
                      regval = sja1000_read_reg( internal, SJA1000_MODE );
    if ( ( regval & REG_MODE_RM ) ) {
      return 0;
    }

    i--;
  }

  return -ETIMEDOUT;
}

/**
 * @brief This function disables the configuration mode.
 *
 * This operation brings the controller to run mode.
 *
 * @param  internal Pointer to internal SJA1000 structure
 *
 * @return 0 on success, negatted ETIMEDOUT otherwise,
 */
static int sja1000_disable_configuration( struct sja1000_internal *internal )
{
  uint8_t regval;
  int i = 100;

  regval = sja1000_read_reg( internal, SJA1000_MODE );
  while ( i ) {
    sja1000_write_reg( internal , SJA1000_MODE, regval & ~( REG_MODE_RM ) );
    regval = sja1000_read_reg( internal, SJA1000_MODE );
    if ( !( regval & REG_MODE_RM ) ) {
      return 0;
    }

    i--;
  }

  return -ETIMEDOUT;
}

/**
 * @brief This function reads the CAN frame from RX FIFO.
 *
 * This only reads the frame and stores it in a given structure, but
 * the function doesn't redistribute it to the upper layer, because
 * it can be called from both worker thread and interrupt handler.
 *
 * @param  chip  Pointer to CAN chip structure.
 * @param  frame Pointer to CAN frame.
 *
 * @return 1 on success (received frame),
 */
static int sja1000_read_frame(
    struct rtems_can_chip *chip,
    struct can_frame *frame
)
{
  struct sja1000_internal *internal = chip->internal;
  enum sja1000_regs data_reg;
  uint32_t id;
  uint8_t frm;
  int i;

  frm = sja1000_read_reg( internal, SJA1000_FRM );
  if ( FIELD_GET( REG_FRM_FF, frm ) ) {
    /* Extended frame format */
      id = (uint32_t)sja1000_read_reg( internal, SJA1000_ID0 ) << 21;
      id += (uint32_t)sja1000_read_reg( internal, SJA1000_ID1 ) << 13;
      id += (uint32_t)sja1000_read_reg( internal, SJA1000_ID2 ) << 5;
      id += (uint32_t)sja1000_read_reg( internal, SJA1000_ID3 ) >> 3;
                                       data_reg = SJA1000_DATE;
  } else {
    /* Standard frame format */
      id = (uint32_t)sja1000_read_reg( internal, SJA1000_ID0 ) << 3;
      id += (uint32_t)sja1000_read_reg( internal, SJA1000_ID1 ) >> 5;
                                        data_reg = SJA1000_DATS;
  }

  /* Fill the CAN frame structure */
  frame->header.flags = 0;
  frame->header.can_id = id;
  frame->header.timestamp = rtems_can_fill_timestamp();

  /* RTR Flag */
  if ( FIELD_GET( REG_FRM_RTR, frm ) ) {
    frame->header.flags |= CAN_FRAME_RTR;
  }

  frame->header.dlen = FIELD_GET( REG_FRM_DLC, frm );
  for ( i = 0; i < frame->header.dlen; i++ ) {
    frame->data[i] = sja1000_read_reg( internal, data_reg++ );
  }

  return 1;
}


/**
 * @brief  This function handles CAN interrupts.
 *
 * @param  arg Argument passed throught interrupt routine.
 *
 * @return None
 */
static void sja1000_interrupt( void *arg )
{
  struct rtems_can_chip *chip = (struct rtems_can_chip *)arg;
  struct sja1000_internal *internal = chip->internal;
  int running = rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags );
  uint8_t isr;
  uint8_t ier = running? internal->int_ena: 0;
  ier &= ~( REG_INT_RI | REG_INT_TI );

  /* Ideal scenario would be to mask interrupts and process them all
   * in worker thread. SJA1000 driver unfortunately can't do this, it can
   * only disable them, but this resets interrupt register.
   *
   * Instead, read interrupt register here and atomicaly or its
   * value to the variable that will be used by the worker thread. Then
   * disable the interrupts.
   */

  isr = sja1000_read_reg( internal, SJA1000_IR );
  if ( running ) {
    atomic_fetch_or( &internal->isr, isr & REG_INT_WORKER_MASK );
  }
  sja1000_write_reg( internal, SJA1000_IER, ier & ~internal->isr );

  /* RX interrupt keeps coming until release buffer command is sent even if
   * it is disabled. Therefore read the first message here, store in
   * in our internal structure and release the buffer. Following messages
   * are read from worker thread based on RX buffer status bit.
   */
  if ( FIELD_GET( REG_INT_RI, isr ) &&
       ( !internal->frame_to_pass || !running ) ) {
    internal->frame = ( struct can_frame ){};
    sja1000_read_frame( chip, &internal->frame );
    sja1000_write_reg( internal, SJA1000_CMR, REG_CMR_RRB );
    if ( running )
      internal->frame_to_pass = true;
  }

  if ( running ) {
    /* IRQ stuck protection */
    if ( atomic_fetch_add( &internal->irq_loops, 1 ) > 1000 ) {
      rtems_interrupt_vector_disable( chip->irq );
      internal->irq_emergency_disable = true;
    }
    rtems_binary_semaphore_post( &chip->qends_dev->worker_sem );
  }
  return;
}

/**
 * @brief  This function inserts frame to TX buffer
 *
 * @param  chip Pointer to CAN chip structure
 * @param  cf   Pointer to CAN frame.
 *
 * @return True on success, False otherwise.
 */
static bool sja1000_write_frame(
    struct rtems_can_chip *chip,
    const struct can_frame *cf
)
{
  struct sja1000_internal *internal = chip->internal;
  unsigned int i;
  uint8_t frm = 0;
  uint32_t id = 0;
  bool ide = cf->header.flags & CAN_FRAME_IDE;
  enum sja1000_regs data_reg = ide ? SJA1000_DATE : SJA1000_DATS;
  uint8_t cmr;

  if ( cf->header.dlen > CAN_FRAME_STANDARD_DLEN ) {
    return false;
  }

  /* Prepare Frame format */
  if ( cf->header.flags & CAN_FRAME_RTR ) {
    frm |= REG_FRM_RTR;
  }

  if ( ide ) {
    frm |= REG_FRM_FF;
  }

  frm |= rtems_canfd_len2dlc( cf->header.dlen );
  sja1000_write_reg( internal, SJA1000_FRM, frm );

  /* Fill CAN frame identifier */
  id = cf->header.can_id;
  if ( ide ) {
    sja1000_write_reg( internal, SJA1000_ID0, ( id >> 21 ) & 0xf8 );
    sja1000_write_reg( internal, SJA1000_ID1, ( id >> 13 ) & 0xff );
    sja1000_write_reg( internal, SJA1000_ID2, ( id >> 5 ) & 0xff );
    sja1000_write_reg( internal, SJA1000_ID3, ( id << 3 ) & 0xff );
  } else {
    sja1000_write_reg( internal, SJA1000_ID0, ( id >> 3 ) & 0xff );
    sja1000_write_reg( internal, SJA1000_ID1, ( id << 5 ) & 0xff );
  }

  /* Fill the data */
  for ( i = 0; i < cf->header.dlen; i++ ) {
    sja1000_write_reg( internal, data_reg + i, cf->data[i] );
  }

  /* Set correct TX modex */
  if ( chip->ctrlmode & CAN_CTRLMODE_LOOPBACK )
    cmr = REG_CMR_SRR;
  else
    cmr = REG_CMR_TR;

  if ( chip->ctrlmode & CAN_CTRLMODE_ONE_SHOT )
    cmr |= REG_CMR_AT;

  sja1000_write_reg( internal, SJA1000_CMR, cmr );
  return true;
}

/**
 * @brief  This function reads SJA1000s fault confinement state.
 *
 * @param  internal Pointer to internal SJA1000 structure
 *
 * @return Fault confinement state of controller
 */
static enum can_state sja1000_read_fault_state(
  struct sja1000_internal *internal
)
{
  uint8_t ecc;
  uint8_t rxerr;
  uint8_t txerr;
  uint8_t ewl;
  uint8_t sr;

  ecc = sja1000_read_reg( internal, SJA1000_ECC );
  ewl = sja1000_read_reg( internal, SJA1000_EWLR );
  rxerr = sja1000_read_reg( internal, SJA1000_RXERR );
  txerr = sja1000_read_reg( internal, SJA1000_TXERR1 );
  sr = sja1000_read_reg( internal, SJA1000_SR );

  if ( FIELD_GET( REG_ECC_FUNC, ecc ) ==  REG_ECC_ERROR_ACTIVE ) {
    if ( ewl > rxerr && ewl > txerr ) {
      return CAN_STATE_ERROR_ACTIVE;
    } else {
      return CAN_STATE_ERROR_WARNING;
    }
  } else if ( FIELD_GET( REG_ECC_FUNC, ecc ) == REG_ECC_ERROR_PASSIVE ) {
    return CAN_STATE_ERROR_PASSIVE;
  } else if ( FIELD_GET( REG_SR_BS, sr ) ) {
    return CAN_STATE_BUS_OFF;
  }

  return CAN_STATE_ERROR_PASSIVE;
}

/**
 * @brief  This function handles error frame interrupts.
 *
 * @param  chip Pointer to CAN chip structure
 * @param  isr  Interrupt status register value
 *
 * @return None
 */
static void sja1000_err_interrupt( struct rtems_can_chip *chip, uint32_t isr )
{
  struct sja1000_internal *internal = chip->internal;
  struct can_frame err_frame = {};
  enum can_state state;
  uint8_t rxerr;
  uint8_t txerr;
  uint8_t ecc;

  rxerr = sja1000_read_reg( internal, SJA1000_RXERR );
  txerr = sja1000_read_reg( internal, SJA1000_TXERR1 );

  state = sja1000_read_fault_state( internal );
  rtems_can_stats_set_state(&chip->chip_stats, state );
  atomic_fetch_and( &internal->isr, ~( REG_INT_EPI ) );

  if ( FIELD_GET( REG_INT_EI, isr ) ) {
    atomic_fetch_and( &internal->isr, ~( REG_INT_EI ) );
    switch ( state ) {
      case CAN_STATE_BUS_OFF:
        err_frame.header.can_id = CAN_ERR_ID_BUSOFF;
        err_frame.header.flags = CAN_FRAME_ERR;
        break;
      case CAN_STATE_ERROR_PASSIVE:
        err_frame.header.can_id = CAN_ERR_ID_CRTL | CAN_ERR_ID_CNT;
        err_frame.header.flags = CAN_FRAME_ERR;
        err_frame.data[CAN_ERR_DATA_BYTE_TRX_CTRL] = ( rxerr > 127 ) ?
                             CAN_ERR_CRTL_RX_PASSIVE :
                             CAN_ERR_CRTL_TX_PASSIVE;
        err_frame.data[CAN_ERR_DATA_BYTE_CNT_TX] = txerr;
        err_frame.data[CAN_ERR_DATA_BYTE_CNT_RX] = rxerr;
        break;
      case CAN_STATE_ERROR_WARNING:
        err_frame.header.can_id = CAN_ERR_ID_CRTL | CAN_ERR_ID_CNT;
        err_frame.header.flags = CAN_FRAME_ERR;
        err_frame.data[CAN_ERR_DATA_BYTE_TRX_CTRL] = ( txerr > rxerr ) ?
                             CAN_ERR_CRTL_TX_WARNING :
                             CAN_ERR_CRTL_RX_WARNING;
        err_frame.data[CAN_ERR_DATA_BYTE_CNT_TX] = txerr;
        err_frame.data[CAN_ERR_DATA_BYTE_CNT_RX] = rxerr;
        break;
      case CAN_STATE_ERROR_ACTIVE:
        err_frame.header.can_id = CAN_ERR_ID_CRTL;
        err_frame.header.flags = CAN_FRAME_ERR;
        err_frame.data[CAN_ERR_DATA_BYTE_TRX_CTRL] = CAN_ERR_CRTL_ACTIVE;
        err_frame.data[CAN_ERR_DATA_BYTE_CNT_TX] = txerr;
        err_frame.data[CAN_ERR_DATA_BYTE_CNT_RX] = rxerr;
        break;
      default:
        break;
    }
  }

  /* Check for Arbitration Lost interrupt */
  if ( FIELD_GET( REG_INT_ALI, isr ) ) {
    atomic_fetch_and( &internal->isr, ~( REG_INT_ALI ) );
    err_frame.header.can_id |= CAN_ERR_ID_CRTL | CAN_ERR_ID_LOSTARB;
    err_frame.header.flags |= CAN_FRAME_ERR;
    err_frame.data[CAN_ERR_DATA_BYTE_TRX_LOSTARB] = CAN_ERR_LOSTARB_UNSPEC;
  }

  /* Check for Bus Error interrupt */
  if ( FIELD_GET( REG_INT_BEI, isr ) ) {
    atomic_fetch_and( &internal->isr, ~( REG_INT_BEI ) );
    rtems_can_stats_add_rx_error( &chip->chip_stats );
    ecc = sja1000_read_reg( internal, SJA1000_ECC );
    err_frame.header.can_id |= CAN_ERR_ID_CRTL | CAN_ERR_ID_PROT | \
                               CAN_ERR_ID_BUSERROR;
    err_frame.header.flags |= CAN_FRAME_ERR;
    switch ( ecc & REG_ECC_ERCC ) {
      case REG_ECC_ERCC_BIT:
        err_frame.data[CAN_ERR_DATA_BYTE_TRX_PROT] = CAN_ERR_PROT_BIT;
		break;
      case REG_ECC_ERCC_FORM:
        err_frame.data[CAN_ERR_DATA_BYTE_TRX_PROT] = CAN_ERR_PROT_FOR;
		break;
      case REG_ECC_ERCC_STUFF:
        err_frame.data[CAN_ERR_DATA_BYTE_TRX_PROT] = CAN_ERR_PROT_STUFF;
		break;
	  default:
		break;
    }
    err_frame.data[CAN_ERR_DATA_BYTE_TRX_PROT] = CAN_ERR_PROT_UNSPEC;
    err_frame.data[CAN_ERR_DATA_BYTE_TRX_PROT_LOC] = CAN_ERR_PROT_LOC_UNSPEC;
  }

  /* Check for RX overflow interrupt */
  if ( FIELD_GET( REG_INT_DOI, isr ) ) {
    atomic_fetch_and( &internal->isr, ~( REG_INT_DOI ) );
    rtems_can_stats_add_rx_overflows( &chip->chip_stats );
    err_frame.header.can_id |= CAN_ERR_ID_CRTL;
    err_frame.header.flags |= CAN_FRAME_ERR;
    err_frame.data[CAN_ERR_ID_LOSTARB] |= CAN_ERR_CRTL_RX_OVERFLOW;
    sja1000_write_reg( internal, SJA1000_CMR, REG_CMR_CDO );
  }

  if ( err_frame.header.flags != 0 ) {
    rtems_can_queue_filter_frame_to_edges(
      &chip->qends_dev->base,
      NULL,
      &err_frame,
      0
    );
  }
}

/**
 * @brief  This is the part of the worker function that processes interrupts.
 *
 * This is implemented as a while loop that goes on until there are no
 * interrupts to be read.
 *
 * @param  rtems_can_chip      Pointer to chip structure.
 *
 * @return None
 */
static inline void sja1000_process_interrupts(
  struct rtems_can_chip *chip
)
{
  uint8_t isr;
  uint8_t sr;
  bool rx_dispatch;
  struct sja1000_internal *internal = chip->internal;
  struct sja1000_txb_info *txb_info = &internal->txb_info;
  struct rtems_can_queue_ends_dev *qends_dev = chip->qends_dev;
  struct rtems_can_queue_ends *qends = &qends_dev->base;
  do {
    rx_dispatch = internal->frame_to_pass;
    internal->frame_to_pass = false;
    while ( rx_dispatch ) {
      /* Pass frame to edges */
      rtems_can_queue_filter_frame_to_edges( qends, NULL, &internal->frame, 0 );
      /* Update statistics */
      rtems_can_stats_add_rx_done( &chip->chip_stats );
      rtems_can_stats_add_rx_bytes( &chip->chip_stats,
                                    internal->frame.header.dlen );
      rx_dispatch = false;
      sr = sja1000_read_reg( internal, SJA1000_SR );
      if ( sr == 0xff )
        break;
      /* Check receive buffer status and process another message if RX FIFO
       * is not empty
       */
      if ( FIELD_GET( REG_SR_RBS, sr ) ) {
        if ( sja1000_read_frame( chip, &internal->frame ) >= 0) {
          /* Release received buffer -> this will either clear the respective
           * bit in status register or push another RX message to the buffer
           */
          sja1000_write_reg( internal, SJA1000_CMR, REG_CMR_RRB );
          rx_dispatch = true;
        }
      }
    }

    sr = sja1000_read_reg( internal, SJA1000_SR );
    if ( txb_info->slot != NULL ) {
      if ( FIELD_GET( REG_SR_TCS, sr ) ) {
        /* Transmit was finished. Update TX statistics, send local echo
         * and release the slot from SW FIFO queue.
         */
        rtems_can_stats_add_tx_done( &chip->chip_stats );
        rtems_can_stats_add_tx_bytes(
          &chip->chip_stats,
          txb_info->slot->frame.header.dlen
        );
        rtems_can_queue_filter_frame_to_edges(
          &chip->qends_dev->base,
          txb_info->edge,
          &txb_info->slot->frame,
          CAN_FRAME_LOCAL
        );
        rtems_can_queue_free_outslot(
          &chip->qends_dev->base,
          txb_info->edge,
          txb_info->slot
        );
        txb_info->slot = NULL;
        txb_info->edge = NULL;
      } else if ( FIELD_GET( REG_SR_TBS, sr ) ) {
        /* We have reserved slot in TX buffer but TBS field says transmit
         * buffer is empty and transmission was not finished because TCS field
         * is not set -> abort happened.
         * Reschedule aborted frames only if the chip is running, otherwise
         * return them as TX error frames (the chip was stopped with some
         * frames still in the FIFOs/HW buffers).
         */
        if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 1 ) {
          /* Reschedule aborted frames for future processing only if the chip
           * is running. Otherwise handle them as errors.
           */
          rtems_can_queue_push_back_outslot(
            &chip->qends_dev->base,
            txb_info->edge,
            txb_info->slot
          );
        } else {
          rtems_can_queue_filter_frame_to_edges(
            &chip->qends_dev->base,
            txb_info->edge,
            &txb_info->slot->frame,
            CAN_FRAME_TXERR
          );
          rtems_can_queue_free_outslot(
            &chip->qends_dev->base,
            txb_info->edge,
            txb_info->slot
          );
        }
        txb_info->slot = NULL;
        txb_info->edge = NULL;
      }
    }

    isr = atomic_load( &internal->isr );
    if (
        FIELD_GET( REG_INT_EI, isr ) ||
        FIELD_GET( REG_INT_DOI, isr ) ||
        FIELD_GET( REG_INT_EPI, isr ) ||
        FIELD_GET( REG_INT_ALI, isr ) ||
        FIELD_GET( REG_INT_BEI, isr )
    ) {
      /* Error interrupts */
      sja1000_err_interrupt( chip, isr );
    }
  } while ( isr != 0 );
}

/**
 * @brief  This is the main worker for processing of TX and RX messages.
 *
 * The worker is released either from CAN interrupt or from upper layer
 * when there is new frame to be sent. The function handles all interrupts
 * and process new messages to be transfer.
 *
 * @param  arg  RTEMS task argument, this is the pointer to CAN chip structure.
 *
 * @return None
 */
static rtems_task sja1000_worker( rtems_task_argument arg )
{
  struct rtems_can_chip *chip = ( struct rtems_can_chip * )arg;
  struct sja1000_internal *internal = chip->internal;
  struct rtems_can_queue_ends_dev *qends_dev = chip->qends_dev;
  struct rtems_can_queue_ends *qends = &qends_dev->base;
  struct rtems_can_queue_edge *qedge;
  struct rtems_can_queue_slot *slot;
  int ret;

  while ( 1 ) {
    int tx_recheck = 0;
    bool reenable_irq = false;

    if (internal->irq_emergency_disable) {
      internal->irq_emergency_disable = false;
#ifdef SJA1000_DEBUG
      printk("stuck IRQ 0x%02x IR 0x%02x SR 0x%02x F2P %d\n", internal->isr,
             sja1000_read_reg( internal, SJA1000_IR ),
             sja1000_read_reg( internal, SJA1000_SR ),
             internal->frame_to_pass );
#endif
      reenable_irq = true;
    }

    if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) )
      sja1000_process_interrupts( chip );

    if ( atomic_load( &internal->do_stop ) ) {
      if ( internal->txb_info.slot != NULL ) {
        sja1000_write_reg( internal, SJA1000_CMR, REG_CMR_AT );
        tx_recheck = 1;
      }

      /* Clear the FIFOs filled with frames to be sent. */
      while ( rtems_can_queue_test_outslot( qends, &qedge, &slot ) >= 0 ) {
        /* Filter these frames back to the application as TX error frames. */
        rtems_can_queue_filter_frame_to_edges(
          &chip->qends_dev->base,
          qedge,
          &slot->frame,
          CAN_FRAME_TXERR
        );
        rtems_can_queue_free_outslot(
          &chip->qends_dev->base,
          qedge,
          slot
        );
        rtems_can_stats_add_tx_error( &chip->chip_stats );
      }

      if ( sja1000_read_reg( chip->internal, SJA1000_SR ) & REG_SR_TBS &&
          internal->txb_info.slot == NULL
      ) {
        /* Notify the stop function all frames were aborted/sent back */
        atomic_store( &internal->do_stop, false );
        rtems_binary_semaphore_post( &chip->stop_sem );
      }
    } else if (
      ( sja1000_read_reg( chip->internal, SJA1000_SR ) & REG_SR_TBS ) &&
      internal->txb_info.slot == NULL
    ) {
      /* TX buffer is empty, we can write a frame */
      ret = rtems_can_queue_test_outslot( qends, &qedge, &slot );
      if ( ret >= 0 ) {
        if ( sja1000_write_frame( chip, &slot->frame ) ) {
          internal->txb_info.edge = qedge;
          internal->txb_info.slot = slot;

          internal->txb_info.prio = qedge->edge_prio;
          tx_recheck = 1;
        } else {
          rtems_can_queue_push_back_outslot( qends, qedge, slot );
        }
      }
    } else if ( ( internal->txb_info.prio < RTEMS_CAN_QUEUE_PRIO_NR - 1 ) &&
        internal->txb_info.slot != NULL ) {
      /* There is no free space in HW buffers. Check whether pending
       * message has higher priority class then some message in HW buffers.
       */
      int avail_prio = internal->txb_info.prio + 1;
      int pending_prio = rtems_can_queue_pending_outslot_prio(
        qends,
        avail_prio
      );
      if ( pending_prio >= avail_prio ) {
        /* There is a pending message with higher priority class. Abort
         * the frame currently inserted in HW buffer.
         */
        sja1000_write_reg( internal, SJA1000_CMR, REG_CMR_AT );
        tx_recheck = 1;
      }
    }

    sja1000_write_reg( internal, SJA1000_IER, internal->int_ena );
    if (tx_recheck || internal->isr) {
      continue;
    }
    internal->irq_loops = 0;
    if (reenable_irq) {
      rtems_interrupt_vector_enable( chip->irq );
    }
    rtems_binary_semaphore_wait( &qends_dev->worker_sem );
    internal->irq_loops = 0;
  }
}

/**
 * @brief  This function is used to obtain the information about the
 * controller.
 *
 * @param  chip  Pointer to chip structure.
 * @param  what  Integer specifying what info should be retrieved. Refer to
 *   @ref CANChip info defines.
 *
 * @return Obtain information on success, negated errno on error.
 * @retval -EINVAL  Incorrect parameter.
 * @retval -ENOTSUP Given information is not provided/supported.
 */
static int sja1000_get_chip_info( struct rtems_can_chip *chip, int what )
{
  int ret;

  switch( what ) {
    case RTEMS_CAN_CHIP_BITRATE:
      ret = chip->bittiming.bitrate;
      break;
    case RTEMS_CAN_CHIP_DBITRATE:
      ret = -ENOTSUP;
      break;
    case RTEMS_CAN_CHIP_NUSERS:
      ret = atomic_load( &chip->used );
      break;
    case RTEMS_CAN_CHIP_FLAGS:
      ret = atomic_load( &chip->flags );
      break;
    case RTEMS_CAN_CHIP_MODE:
      ret = chip->ctrlmode;
      break;
    case RTEMS_CAN_CHIP_MODE_SUPPORTED:
      ret = chip->ctrlmode_supported;
      break;
    default:
      ret = -EINVAL;
      break;
  }

  return ret;
}

/**
 * @brief  This function calculates and sets bittiming from given bitrate
 *
 * @param  chip  Pointer to chip structure.
 * @param  type  Bittiming type (nominal, FD)
 * @param  bt    Pointer to rtems_can_bittiming structure
 *
 * @return 0 on success, negated errno on error.
 * @retval -EPERM  Chip is already started, cannot change bittiming.
 * @retval -EINVAL Incorrect bit time type.
 */
static int sja1000_calc_and_set_bittiming(
  struct rtems_can_chip *chip,
  int type,
  struct rtems_can_bittiming *bt
)
{
  const struct rtems_can_bittiming_const *btc;
  struct rtems_can_bittiming *chip_bt;
  int ret;

  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) ) {
    /* Chip was already started, cannot change bitrate */
    return -EPERM;
  }

  if ( type != RTEMS_CAN_BITTIME_NOMINAL ) {
    return -EINVAL;
  }

  btc = &sja1000_bittiming_const;
  chip_bt = &chip->bittiming;
  ret = rtems_can_bitrate2bittiming( chip, bt, btc );
  if ( ret < 0 ) {
    return ret;
  }

  *chip_bt = *bt;

  return 0;
}


static int sja1000_set_bittiming( struct rtems_can_chip *chip )
{
  struct sja1000_internal *internal = chip->internal;
  struct rtems_can_bittiming *bt = &chip->bittiming;
  uint8_t btr0, btr1;
  uint8_t brp_raw = ( bt->brp >> 1 ) - 1;

  btr0 = ( ( ( bt->sjw - 1 ) & 0x3 ) << 6 ) | ( brp_raw & 0x3f );
  btr1 = ( ( bt->prop_seg + bt->phase_seg1 - 1 ) & 0xf ) |
           ( ( ( bt->phase_seg2 - 1 ) & 0x7 ) << 4 );
  if ( chip->ctrlmode & CAN_CTRLMODE_3_SAMPLES )
    btr1 |= 0x80;
  sja1000_write_reg( internal, SJA1000_BTR0, btr0 );
  sja1000_write_reg( internal, SJA1000_BTR1, btr1 );

  return 0;
}

/**
 * @brief  This function checks and sets given bittiming
 *
 * @param  chip  Pointer to chip structure.
 * @param  type  Bittiming type (nominal, FD)
 * @param  bt    Pointer to rtems_can_bittiming structure
 *
 * @return 0 on success, negated errno on error.
 * @retval -EPERM  Chip is already started, cannot change bittiming.
 * @retval -EINVAL Incorrect bit time type.
 */
static int sja1000_check_and_set_bittiming(
  struct rtems_can_chip *chip,
  int type,
  struct rtems_can_bittiming *bt
)
{
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) ) {
    /* Chip was already started, cannot change bitrate */
    return -EPERM;
  }

  if ( type != RTEMS_CAN_BITTIME_NOMINAL ) {
    return -EINVAL;
  }

  chip->bittiming = *bt;

  return 0;
}


/**
 * @brief  This function implements chip specific ioctl calls.
 *
 * @param  chip    Pointer to chip structure.
 * @param  command IOCTL command
 * @param  arg     Void pointer to IOCL argument
 *
 * @return Negative value on error.
 */
static int sja1000_chip_ioctl(
  struct rtems_can_chip *chip,
  ioctl_command_t command,
  void *arg
)
{
  (void) chip;
  (void) arg;

  int ret;

  switch( command ) {
    default:
      ret = -EINVAL;
      break;
  }

  return ret;
}

/**
 * @brief  This function stops the chip.
 *
 * @param  chip  Pointer to chip structure.
 *
 * @return 0 on success. -ETIME in case of timeout or -EAGAIN if no timeout
 *         is set.
 */
static int sja1000_stop_chip(
  struct rtems_can_chip *chip,
  struct timespec *ts
)
{
  struct sja1000_internal *internal = chip->internal;
  rtems_interval timeout;
  struct timespec curr;
  struct timespec final;
  struct timespec towait;
  int ret = -1;

  rtems_mutex_lock( &chip->lock );
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 0 ) {
    /* Already stopped, return. */
    rtems_mutex_unlock( &chip->lock );
    return 0;
  }

  /* Set do_stop flag to notify worker we don't want to perform standard
   * TX operation, but that we want to stop the controller and dispatch
   * all pending frames.
   */
  atomic_store( &internal->do_stop, true );
  /* Only TX interrupts will remain enabled */
  internal->int_ena &= REG_INT_TI;
  sja1000_write_reg( internal, SJA1000_IER, internal->int_ena );
  /* Wake up worker thread */
  rtems_binary_semaphore_post( &chip->qends_dev->worker_sem );
  if ( ts != NULL ) {
    /* Get absolute monotonic final time */
    clock_gettime( CLOCK_MONOTONIC, &final );
    rtems_timespec_add_to( &final, ts );

    clock_gettime( CLOCK_MONOTONIC, &curr );
    rtems_timespec_subtract( &curr, &final, &towait );
    if ( towait.tv_sec < 0 ) {
      ret = rtems_binary_semaphore_try_wait( &chip->stop_sem );
    } else {
      timeout = rtems_timespec_to_ticks( &towait );
      ret = rtems_binary_semaphore_wait_timed_ticks(
        &chip->stop_sem,
        timeout
      );
    }
  } else {
    /* There is no timeout, do just try wait */
    ret = rtems_binary_semaphore_try_wait( &chip->stop_sem );
  }

  if (ret < 0) {
    /* The controller didn't managed to clear all frames before timeout,
     * flush the buffers.
     */
    rtems_can_queue_ends_flush_outlist( &chip->qends_dev->base );
  }

  /* Nark the chip as stopped */
  rtems_can_clear_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags );
  /* Disable all interrupts and set the chip in configuration (disable)
   * mode.
   */
  sja1000_write_reg( internal, SJA1000_IER, 0 );
  sja1000_enable_configuration( internal );
  rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_STOPPED );
  rtems_can_bus_notify_chip_stop( chip );
  rtems_mutex_unlock( &chip->lock );

  return 0;
}

/**
 * @brief  This function sets SJA1000 mode
 *
 * @param  internal Pointer to internal SJA1000 structure
 * @param  ctrlmode Mode set bitfield
 *
 * @return None
 */
static void sja1000_set_mode(
  struct sja1000_internal *internal,
  uint32_t ctrlmode
)
{
  uint8_t mode_reg = sja1000_read_reg( internal, SJA1000_MODE );

  mode_reg = ( ctrlmode & CAN_CTRLMODE_LISTENONLY ) ?
             ( mode_reg | REG_MODE_LOM ) :
             ( mode_reg & ~REG_MODE_LOM );

  mode_reg = ( ctrlmode & CAN_CTRLMODE_PRESUME_ACK ) ?
             ( mode_reg | REG_MODE_STM ) :
             ( mode_reg & ~REG_MODE_STM );


  sja1000_write_reg( internal, SJA1000_MODE, mode_reg );
}

/**
 * @brief  This function starts the chip.
 *
 * It sets bit timing values, correct mode, enables interrupts and HW
 * controller.
 *
 * @param  chip  Pointer to chip structure.
 *
 * @return 0 on success, negative value otherwise.
 */
static int sja1000_start_chip( struct rtems_can_chip *chip )
{
  struct sja1000_internal *internal = chip->internal;
  int ret = 0;

  rtems_mutex_lock( &chip->lock );
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) ) {
    /* Chip was already configured, skip */
    rtems_mutex_unlock( &chip->lock );
    return 0;
  }

  ret = sja1000_enable_configuration( internal );
  if ( ret < 0 ) {
    rtems_mutex_unlock( &chip->lock );
    return ret;
  }

  /* Set PeliCAN mode */
  sja1000_write_reg(
      internal,
      SJA1000_CDR,
      REG_CDR_PELICAN | REG_CDR_OFF | REG_CDR_CBP
  );

  /* Reset acceptance filter -> accept all */
  sja1000_write_reg( internal, SJA1000_ACR0, 0 );
  sja1000_write_reg( internal, SJA1000_ACR1, 0 );
  sja1000_write_reg( internal, SJA1000_ACR2, 0 );
  sja1000_write_reg( internal, SJA1000_ACR3, 0 );
  sja1000_write_reg( internal, SJA1000_AMR0, 0xff );
  sja1000_write_reg( internal, SJA1000_AMR1, 0xff );
  sja1000_write_reg( internal, SJA1000_AMR2, 0xff );
  sja1000_write_reg( internal, SJA1000_AMR3, 0xff );

  /* Disable all interrupts */
  sja1000_write_reg( internal, SJA1000_IER, 0 );

  /* Set output mode */
  sja1000_write_reg(
      internal,
      SJA1000_OCR,
      REG_OCR_MODE_NORMAL | REG_OCR_TX0_LH
  );

  /* Configure bit-rate */
  ret = sja1000_set_bittiming( chip );
  if ( ret < 0 ) {
    sja1000_disable_configuration( internal );
    rtems_mutex_unlock( &chip->lock );
    return ret;
  }

  /* Set controller's modes */
  sja1000_set_mode( internal, chip->ctrlmode );

  sja1000_write_reg( internal, SJA1000_TXERR0, 0 );
  sja1000_write_reg( internal, SJA1000_TXERR1, 0 );
  sja1000_read_reg( internal, SJA1000_ECC );

  /* Clear all interrupts */
  sja1000_read_reg( internal, SJA1000_IR );

  /* Enable interrupts */
  internal->int_ena = REG_INT_RI | REG_INT_TI | REG_INT_EI |
                      REG_INT_DOI | REG_INT_WUI | REG_INT_EPI;
  if ( chip->ctrlmode & CAN_CTRLMODE_BERR_REPORTING )
    internal->int_ena |= REG_INT_ALI | REG_INT_BEI;
  sja1000_write_reg( internal, SJA1000_IER, internal->int_ena );

  /* Leave configuration mode -> this returns the chip to normal operation */
  ret = sja1000_disable_configuration( internal );
  if ( ret < 0 ) {
    rtems_mutex_unlock( &chip->lock );
    return ret;
  }

  /* Mark HW as configured */
  rtems_binary_semaphore_try_wait( &chip->stop_sem );
  rtems_can_set_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags );
  rtems_can_stats_reset( &chip->chip_stats );
  rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_ERROR_ACTIVE );

  rtems_mutex_unlock( &chip->lock );
  return ret;
}

/**
 * @brief  This function registers the chip operations.
 *
 * @param  chip_ops  Pointer to rtems_can_chip_ops structure.
 *
 * @return None
 */
static void sja1000_register( struct rtems_can_chip_ops *chip_ops )
{
  chip_ops->start_chip = sja1000_start_chip;
  chip_ops->stop_chip = sja1000_stop_chip;
  chip_ops->chip_ioctl = sja1000_chip_ioctl;
  chip_ops->get_chip_timestamp = NULL;
  chip_ops->get_chip_info = sja1000_get_chip_info;
  chip_ops->calc_bittiming = sja1000_calc_and_set_bittiming;
  chip_ops->check_and_set_bittiming = sja1000_check_and_set_bittiming;
}

struct rtems_can_chip *rtems_can_sja1000_initialize(
  uintptr_t addr,
  unsigned int hw_options,
  rtems_vector_number irq,
  rtems_task_priority worker_priority,
  rtems_option irq_option,
  unsigned long can_clk_rate
)
{
  rtems_id worker_task_id;
  rtems_status_code sc;
  struct rtems_can_chip *chip;
  struct sja1000_internal *internal;
  int ret;

  internal = calloc( 1, sizeof( struct sja1000_internal ) );
  if ( internal == NULL ) {
    return NULL;
  }

  internal->base = addr;
  internal->hw_options = hw_options;

  chip = calloc( 1, sizeof( struct rtems_can_chip ) );
  if ( chip == NULL ) {
    free( internal );
    return NULL;
  }

  chip->qends_dev = calloc( 1, sizeof( struct rtems_can_queue_ends_dev ) );
  if ( chip->qends_dev == NULL ) {
    free( internal );
    free( chip );
    return NULL;
  }

  chip->type = CAN_SJA1000_TYPE;
  chip->irq = irq;
  chip->freq = can_clk_rate;
  chip->ctrlmode_supported = CAN_CTRLMODE_LOOPBACK |
                             CAN_CTRLMODE_LISTENONLY |
                             CAN_CTRLMODE_PRESUME_ACK |
                             CAN_CTRLMODE_3_SAMPLES |
                             CAN_CTRLMODE_BERR_REPORTING |
                             CAN_CTRLMODE_ONE_SHOT;
  chip->bittiming_const = &sja1000_bittiming_const;
  rtems_mutex_init( &chip->lock, "sja1000_lock" );

  sja1000_register( &chip->chip_ops );

  chip->internal = internal;
  ret = sja1000_enable_configuration( internal );
  if ( ret < 0 ) {
    free( internal );
    free( chip->qends_dev );
    free( chip );
    return NULL;
  }

  /* Make sure interrupts are disabled */
  sja1000_write_reg( internal, SJA1000_IER, 0 );

  rtems_binary_semaphore_init(
    &chip->stop_sem,
    "can_sja1000_stop_sem"
  );

  rtems_can_queue_ends_init_chip(
    chip,
    "can_sja1000_worker"
  );

  sc = rtems_interrupt_handler_install(
    chip->irq,
    "sja1000",
    irq_option,
    sja1000_interrupt,
    chip
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    free( internal );
    free( chip->qends_dev );
    free( chip );
    return NULL;
  }

  sc = rtems_task_create(
    rtems_build_name( 'C', 'A', 'N', 'S' ),
    worker_priority,
    RTEMS_MINIMUM_STACK_SIZE + 0x400,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &worker_task_id
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_interrupt_handler_remove(
      chip->irq,
      sja1000_interrupt,
      chip
    );
    free( internal );
    free( chip->qends_dev );
    free( chip );
    return NULL;
  }

  sc = rtems_task_start(
    worker_task_id,
    sja1000_worker,
    ( rtems_task_argument )chip
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    sc = rtems_interrupt_handler_remove( chip->irq, sja1000_interrupt, chip );
    _Assert( sc == RTEMS_SUCCESSFUL );
    free( internal );
    free( chip->qends_dev );
    free( chip );
    sc = rtems_task_delete( worker_task_id );
    _Assert( sc == RTEMS_SUCCESSFUL );
    return NULL;
  }

  rtems_can_set_bit( RTEMS_CAN_CHIP_CONFIGURED, &chip->flags );
  rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_STOPPED );

  return chip;
}
