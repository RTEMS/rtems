/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup DCAN
 *
 * @brief This source file contains the implementation of the DCAN driver.
 */

/*
 * Copyright (C) 2026 Ning Zhang <riverquiet66@gmail.com>
 * Copyright (C) 2026 Michal Lenc <michallenc@seznam.cz>
 * Copyright (C) 2026 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 * Based on DCAN driver code by Prashanth S <fishesprashanth@gmail.com>
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
#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <errno.h>
#include <rtems.h>

#include <dev/can/dcan.h>
#include "dcan_regs.h"

/*
 * Message Object allocation.
 */
#define DCAN_RX_MO_FIRST 1u
#define DCAN_RX_MO_LAST 8u
#define DCAN_RX_MO_COUNT \
  ( DCAN_RX_MO_LAST - DCAN_RX_MO_FIRST + 1u )

#define DCAN_TX_MO_FIRST 9u
#define DCAN_TX_MO_LAST 9u
#define DCAN_TX_MO_COUNT \
  ( DCAN_TX_MO_LAST - DCAN_TX_MO_FIRST + 1u )

#define DCAN_RX_FIFO_FIRST_MOBJ DCAN_RX_MO_FIRST
#define DCAN_RX_FIFO_LAST_MOBJ DCAN_RX_MO_LAST
#define DCAN_RX_FIFO_SIZE DCAN_RX_MO_COUNT


/**
 * @brief This represents one DCAN hardware transmit buffer.
 *
 * Each configured TX Message Object owns at most one RTEMS CAN queue slot.
 * The slot remains owned by the driver until the hardware transmission
 * completes, is aborted for rescheduling, or is returned as an error.
 */
struct dcan_txb_info {
  struct rtems_can_queue_slot *slot;
  struct rtems_can_queue_edge *edge;
  int prio;
  unsigned int mobj;
};

struct dcan_rx_mo_status {
  bool new_data;
  bool message_lost;
  bool end_of_buffer;
  uint32_t mctl;
};

/**
 * @brief This represents internal DCAN controller data.
 */
struct dcan_internal {
  /**
   * @brief Base memory address of the controller registers.
   */
  uintptr_t base;

  /**
   * @brief Software ownership state for the hardware TX Message Objects.
   */
  struct dcan_txb_info txb_info[ DCAN_TX_MO_COUNT ];

  /**
   * @brief Worker task identifier.
   */
  rtems_id worker_task_id;

  /**
   * @brief This indicates that controller stop cleanup is requested.
   */
  atomic_bool do_stop;

  /**
   * @brief Current CAN controller fault state.
   */
  enum can_state state;

  /**
   * @brief Suppresses repeated ACK error reports while hardware retries.
   */
  bool tx_error_reported;
};

static const struct rtems_can_bittiming_const dcan_bittiming_const = {
  .name = "dcan",
  .tseg1_min = 1,
  .tseg1_max = 16,
  .tseg2_min = 1,
  .tseg2_max = 8,
  .sjw_max = 4,
  .brp_min = 1,
  .brp_max = 1024,
  .brp_inc = 1,
};

static uint32_t dcan_read_reg(
  const struct dcan_internal *internal,
  uint32_t reg
)
{
  return *( volatile uint32_t * )( internal->base + reg );
}

static void dcan_write_reg(
  const struct dcan_internal *internal,
  uint32_t reg,
  uint32_t value
)
{
  *( volatile uint32_t * )( internal->base + reg ) = value;
}

static bool dcan_mobj_is_rx_fifo( unsigned int mobj )
{
  return mobj >= DCAN_RX_FIFO_FIRST_MOBJ &&
         mobj <= DCAN_RX_FIFO_LAST_MOBJ;
}

static bool dcan_mobj_is_tx( unsigned int mobj )
{
  return mobj >= DCAN_TX_MO_FIRST &&
         mobj <= DCAN_TX_MO_LAST;
}

static unsigned int dcan_tx_mobj_to_index(
  unsigned int mobj
)
{
  return mobj - DCAN_TX_MO_FIRST;
}

static unsigned int dcan_tx_index_to_mobj(
  unsigned int index
)
{
  return DCAN_TX_MO_FIRST + index;
}

/**
 * @brief Checks whether a queue slot is already owned by a TX Message Object.
 */
static bool dcan_slot_is_owned(
  const struct dcan_internal *internal,
  const struct rtems_can_queue_slot *slot
)
{
  unsigned int index;

  if ( slot == NULL ) {
    return false;
  }

  for ( index = 0; index < DCAN_TX_MO_COUNT; ++index ) {
    if ( internal->txb_info[ index ].slot == slot ) {
      return true;
    }
  }

  return false;
}

/**
 * @brief Checks whether another TX Message Object owns the same queue slot.
 */
static bool dcan_slot_owned_by_other_mobj(
  const struct dcan_internal *internal,
  const struct rtems_can_queue_slot *slot,
  unsigned int owner_index
)
{
  unsigned int index;

  if ( slot == NULL ) {
    return false;
  }

  for ( index = 0; index < DCAN_TX_MO_COUNT; ++index ) {
    if (
      index != owner_index &&
      internal->txb_info[ index ].slot == slot
    ) {
      return true;
    }
  }

  return false;
}

static bool dcan_has_pending_tx(
  const struct dcan_internal *internal
)
{
  unsigned int index;

  for ( index = 0; index < DCAN_TX_MO_COUNT; ++index ) {
    if ( internal->txb_info[ index ].slot != NULL ) {
      return true;
    }
  }

  return false;
}

/**
 * @brief This function waits until a DCAN interface register set is idle.
 *
 * The DCAN IF command register contains a busy bit while a transfer
 * between interface registers and message RAM is active.
 *
 * @param chip Pointer to CAN chip structure.
 * @param ifno DCAN interface register set number.
 *
 * @return 0 on success, negated ETIMEDOUT otherwise.
 */
static int dcan_wait_if_not_busy(
  struct rtems_can_chip *chip,
  unsigned int ifno
)
{
  struct dcan_internal *internal = chip->internal;
  int i = 1000;
  while ( i > 0 ) {
    if ( ( dcan_read_reg( internal, DCAN_IFCMD( ifno ) ) & 
      DCAN_IFCMD_BUSY ) == 0 ) 
    {
      return 0;
    }
    i--;
  }
  return -ETIMEDOUT;
}

/**
 * @brief This function enables the configuration mode.
 *
 * This operation brings the controller to initialization mode
 * and enables access to configuration registers.
 *
 * @param chip Pointer to CAN chip structure.
 *
 * @return 0 on success, negated ETIMEDOUT otherwise.
 */
static int dcan_enter_init_config_mode( struct rtems_can_chip *chip )
{
  uint32_t ctl;
  struct dcan_internal *internal = chip->internal;
  int i = 1000;

  ctl = dcan_read_reg( internal, DCAN_CTL );
  ctl |= DCAN_CTL_INIT;
  ctl |= DCAN_CTL_CCE;

  dcan_write_reg( internal, DCAN_CTL, ctl );

  while ( i > 0 ) {
    ctl = dcan_read_reg( internal, DCAN_CTL );
    if ( ( ctl & DCAN_CTL_INIT ) != 0 &&
         ( ctl & DCAN_CTL_CCE ) != 0 ) {
      return 0;
    }
    i--;
  }
  return -ETIMEDOUT;
}

/**
 * @brief This function disables the configuration mode.
 *
 * This operation brings the controller from initialization mode
 * to normal operation mode.
 *
 * @param chip Pointer to CAN chip structure.
 *
 * @return 0 on success, negated ETIMEDOUT otherwise.
 */
static int dcan_leave_init_mode( struct rtems_can_chip *chip )
{
  struct dcan_internal *internal = chip->internal;
  uint32_t ctl;
  int i = 100;

  ctl = dcan_read_reg( internal, DCAN_CTL );

  while ( i ) {
    ctl &= ~( DCAN_CTL_INIT | DCAN_CTL_CCE );
    dcan_write_reg( internal, DCAN_CTL, ctl );

    ctl = dcan_read_reg( internal, DCAN_CTL );
    if ( !( ctl & DCAN_CTL_INIT ) ) {
      return 0;
    }

    i--;
  }
  return -ETIMEDOUT;
}

static int dcan_calc_and_set_bittiming(
  struct rtems_can_chip *chip,
  int type,
  struct rtems_can_bittiming *bt
)
{
  const struct rtems_can_bittiming_const *btc;
  int ret;

  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) ) {
    return -EPERM;
  }

  if ( type != RTEMS_CAN_BITTIME_NOMINAL ) {
    return -EINVAL;
  }

  btc = &dcan_bittiming_const;

  ret = rtems_can_bitrate2bittiming( chip, bt, btc );
  if ( ret < 0 ) {
    return ret;
  }
  chip->bittiming = *bt;
  return 0;
}

static int dcan_check_and_set_bittiming(
  struct rtems_can_chip *chip,
  int type,
  struct rtems_can_bittiming *bt
)
{
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) ) {
    return -EPERM;
  }

  if ( type != RTEMS_CAN_BITTIME_NOMINAL ) {
    return -EINVAL;
  }

  chip->bittiming = *bt;
  return 0;
}

static int dcan_set_bittiming(
  struct rtems_can_chip *chip
)
{
  struct dcan_internal *internal = chip->internal;
  struct rtems_can_bittiming *bt = &chip->bittiming;

  uint32_t brp;
  uint32_t brp_reg;
  uint32_t brpe_reg;
  uint32_t sjw_reg;
  uint32_t tseg1_reg;
  uint32_t tseg2_reg;
  uint32_t btr;

  brp = bt->brp;

  /*
   * DCAN encodes BRP as:
   *
   *   real BRP = register value + 1
   *
   * BTR[5:0]   = BRP low 6 bits
   * BTR[19:16] = BRP extension bits
   */
  brp_reg = brp - 1;

  /*
   * Low 6 bits go to BTR[5:0].
   */
  btr = brp_reg & 0x3fu;

  /*
   * BRPE extension bits go to BTR[19:16].
   */
  brpe_reg = ( brp_reg >> 6 ) & 0x0fu;
  btr |= brpe_reg << 16;

  /*
   * SJW field:
   * BTR[7:6] = sjw - 1
   */
  sjw_reg = ( bt->sjw - 1 ) & 0x03u;
  btr |= sjw_reg << 6;

  /*
   * TSEG1 field:
   * BTR[11:8] = prop_seg + phase_seg1 - 1
   */
  tseg1_reg = ( bt->prop_seg + bt->phase_seg1 - 1 ) & 0x0fu;
  btr |= tseg1_reg << 8;

  /*
   * TSEG2 field:
   * BTR[14:12] = phase_seg2 - 1
   */
  tseg2_reg = ( bt->phase_seg2 - 1 ) & 0x07u;
  btr |= tseg2_reg << 12;

  dcan_write_reg( internal, DCAN_BTR, btr );

  return 0;
}

/**
 * @brief Checks whether transmission is pending in a Message Object.
 */
static bool dcan_tx_mo_is_pending(
  struct rtems_can_chip *chip,
  unsigned int mobj
)
{
  struct dcan_internal *internal = chip->internal;
  unsigned int reg_index;
  unsigned int bit_index;
  uint32_t txrq;
  uint32_t mask;

  if ( mobj < 1u || mobj > 64u ) {
    return false;
  }

  reg_index = ( mobj - 1u ) / 32u;
  bit_index = ( mobj - 1u ) % 32u;
  mask = 1u << bit_index;

  txrq = dcan_read_reg(
    internal,
    DCAN_TXRQ( reg_index )
  );

  return ( txrq & mask ) != 0u;
}

/**
 * @brief Writes a CAN frame to a DCAN transmit Message Object.
 *
 * The frame is copied to the IF2 registers and transferred to the
 * selected Message Object in message RAM with TxRqst/NewDat set.
 *
 * @param chip Pointer to CAN chip structure.
 * @param mobj Message Object number.
 * @param frame Pointer to CAN frame to transmit.
 *
 * @return 0 on success, negated errno otherwise.
 */
static int dcan_mo_write_frame(
  struct rtems_can_chip *chip,
  int mobj,
  const struct can_frame *frame
)
{
  struct dcan_internal *internal = chip->internal;
  uint8_t data[ CAN_FRAME_STANDARD_DLEN ] = { 0 };
  uint32_t ifarb;
  uint32_t ifmctl;
  uint32_t ifdata;
  uint32_t ifdatb;
  uint32_t ifcmd;
  uint32_t id;
  unsigned int dlen;
  unsigned int i;
  int ret;

  if ( frame == NULL || mobj < 1 || mobj > 64 ) {
    return -EINVAL;
  }

  dlen = frame->header.dlen;
  if ( dlen > CAN_FRAME_STANDARD_DLEN ) {
    return -EINVAL;
  }

  id = frame->header.can_id & 0x7ffu;

  /*
   * Copy only valid Classical CAN payload bytes.
   */
  for ( i = 0; i < dlen; ++i ) {
    data[ i ] = frame->data[ i ];
  }

  /*
   * Keep TX register traffic on IF2.  IF1 remains dedicated to RX and
   * deferred interrupt-side Message Object access.
   */
  ret = dcan_wait_if_not_busy( chip, 2 );
  if ( ret < 0 ) {
    return ret;
  }

  ifdata =
    ( ( uint32_t ) data[ 0 ] << 0 ) |
    ( ( uint32_t ) data[ 1 ] << 8 ) |
    ( ( uint32_t ) data[ 2 ] << 16 ) |
    ( ( uint32_t ) data[ 3 ] << 24 );

  ifdatb =
    ( ( uint32_t ) data[ 4 ] << 0 ) |
    ( ( uint32_t ) data[ 5 ] << 8 ) |
    ( ( uint32_t ) data[ 6 ] << 16 ) |
    ( ( uint32_t ) data[ 7 ] << 24 );

  ifmctl =
    ( dlen & DCAN_IFMCTL_DATALENGTHCODE ) |
    DCAN_IFMCTL_EOB |
    DCAN_IFMCTL_TXIE |
    DCAN_IFMCTL_TXRQST;

  ifarb =
    DCAN_IFARB_MSGVAL |
    DCAN_IFARB_DIR |
    ( id << 18 );

  dcan_write_reg( internal, DCAN_IFMSK( 2 ), 0u );
  dcan_write_reg( internal, DCAN_IFARB( 2 ), ifarb );
  dcan_write_reg( internal, DCAN_IFMCTL( 2 ), ifmctl );
  dcan_write_reg( internal, DCAN_IFDATA( 2 ), ifdata );
  dcan_write_reg( internal, DCAN_IFDATB( 2 ), ifdatb );

  ifcmd =
    DCAN_IFCMD_WR_RD |
    DCAN_IFCMD_ARB |
    DCAN_IFCMD_CONTROL |
    DCAN_IFCMD_DATAA |
    DCAN_IFCMD_DATAB |
    DCAN_IFCMD_TXRQST_NEWDAT |
    DCAN_IFCMD_MSG_NUM( mobj );

  dcan_write_reg( internal, DCAN_IFCMD( 2 ), ifcmd );

  return dcan_wait_if_not_busy( chip, 2 );
}

/**
 * @brief Requests cancellation of one pending TX Message Object.
 *
 * The function writes the Message Object control field with TxRqst
 * cleared. The TxRqst/NewDat command bit must not be set because, in
 * write direction, it forces TxRqst to one.
 *
 * @return 0 if TxRqst was cleared, 1 if it was already clear,
 *         or a negated errno value on failure.
 */
static int dcan_abort_tx_mobj(
  struct rtems_can_chip *chip,
  unsigned int mobj
)
{
  struct dcan_internal *internal = chip->internal;
  uint32_t ifmctl;
  uint32_t ifcmd;
  int retry;
  int ret;

  if ( !dcan_mobj_is_tx( mobj ) ) {
    return -EINVAL;
  }

  /*
   * The transmission may have completed before the abort request was
   * processed.
   */
  if ( !dcan_tx_mo_is_pending( chip, mobj ) ) {
    return 1;
  }

  ret = dcan_wait_if_not_busy( chip, 1 );
  if ( ret < 0 ) {
    return ret;
  }

  /*
   * Read the current Message Object control field into IF1MCTL so DLC,
   * EOB, TXIE, and the other control bits can be preserved.
   */
  ifcmd =
    DCAN_IFCMD_CONTROL |
    DCAN_IFCMD_MSG_NUM( mobj );

  dcan_write_reg(
    internal,
    DCAN_IFCMD( 1 ),
    ifcmd
  );

  ret = dcan_wait_if_not_busy( chip, 1 );
  if ( ret < 0 ) {
    return ret;
  }

  ifmctl = dcan_read_reg(
    internal,
    DCAN_IFMCTL( 1 )
  );

  /*
   * Clear only TxRqst while preserving the remaining control fields.
   */
  ifmctl &= ~DCAN_IFMCTL_TXRQST;

  dcan_write_reg(
    internal,
    DCAN_IFMCTL( 1 ),
    ifmctl
  );

  /*
   * Write the control field back to the Message Object.
   *
   * Do not include DCAN_IFCMD_TXRQST_NEWDAT. In write direction that
   * command bit sets TxRqst to one regardless of IF1MCTL.TxRqst.
   */
  ifcmd =
    DCAN_IFCMD_WR_RD |
    DCAN_IFCMD_CONTROL |
    DCAN_IFCMD_MSG_NUM( mobj );

  dcan_write_reg(
    internal,
    DCAN_IFCMD( 1 ),
    ifcmd
  );

  ret = dcan_wait_if_not_busy( chip, 1 );
  if ( ret < 0 ) {
    return ret;
  }

  /*
   * Allow the centralized TXRQ bitmap a short bounded interval to
   * reflect the Message Object update.
   */
  for ( retry = 0; retry < 100; ++retry ) {
    if ( !dcan_tx_mo_is_pending( chip, mobj ) ) {
      return 0;
    }
  }

  return -EBUSY;
}

/**
 * @brief Finishes transmission from one DCAN TX Message Object.
 *
 * This function releases the RTEMS CAN queue slot associated with the
 * Message Object and returns a local echo to the originating queue edge.
 *
 * @param chip Pointer to CAN chip structure.
 * @param mobj Transmit Message Object which generated the interrupt.
 */
static void dcan_process_tx_finish(
  struct rtems_can_chip *chip,
  unsigned int mobj
)
{
  struct dcan_internal *internal = chip->internal;
  struct dcan_txb_info *txb_info;
  struct rtems_can_queue_slot *slot;
  struct rtems_can_queue_edge *edge;
  unsigned int index;

  if ( !dcan_mobj_is_tx( mobj ) ) {
    return;
  }

  index = dcan_tx_mobj_to_index( mobj );
  txb_info = &internal->txb_info[ index ];

  if ( txb_info->slot == NULL ) {
    return;
  }

  /*
   * Treat the Message Object interrupt as completion only after TxRqst
   * clears for this exact hardware object.
   */
  if ( dcan_tx_mo_is_pending( chip, mobj ) ) {
    return;
  }

  /*
   * Lightweight invariant: one RTEMS queue slot must have one TX MO owner.
   */
  if (
    dcan_slot_owned_by_other_mobj(
      internal,
      txb_info->slot,
      index
    )
  ) {
    return;
  }

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

  slot = txb_info->slot;
  edge = txb_info->edge;

  /*
   * Relinquish DCAN software ownership before returning the slot to the
   * RTEMS FIFO free list.
   */
  txb_info->slot = NULL;
  txb_info->edge = NULL;
  txb_info->prio = 0;

  rtems_can_queue_free_outslot(
    &chip->qends_dev->base,
    edge,
    slot
  );

  internal->tx_error_reported = false;
}

/**
 * @brief Fills available DCAN transmit Message Objects.
 *
 * Frames are acquired from the RTEMS CAN transmit queues and assigned
 * to currently unused DCAN TX Message Objects. A queue slot remains
 * owned by the corresponding TX Message Object until completion or
 * controller-stop cleanup.
 *
 * @param chip Pointer to CAN chip structure.
 */
static void dcan_process_tx(
  struct rtems_can_chip *chip
)
{
  struct dcan_internal *internal = chip->internal;
  struct rtems_can_queue_ends *qends = &chip->qends_dev->base;
  struct rtems_can_queue_edge *qedge;
  struct rtems_can_queue_slot *slot;
  struct dcan_txb_info *txb_info;
  unsigned int index;
  int ret;

  /*
   * Fill each currently free TX Message Object from the RTEMS software
   * transmit queues.
   *
   * The current configuration uses one TX Message Object to preserve FIFO
   * ordering within the same priority class.
   * 
   * TODO: Call rtems_can_queue_pending_outslot_prio() and rearrange
   * TX message objects when a higher-priority class would be blocked
   * by a lower-priority one.
   */
  for ( index = 0; index < DCAN_TX_MO_COUNT; ++index ) {
    txb_info = &internal->txb_info[ index ];

    if ( txb_info->slot != NULL ) {
      continue;
    }

    ret = rtems_can_queue_test_outslot(
      qends,
      &qedge,
      &slot
    );

    if ( ret < 0 ) {
      break;
    }

    /*
     * Lightweight invariant: test_outslot() must not return a slot which
     * is still owned by another DCAN TX Message Object.
     */
    if ( dcan_slot_is_owned( internal, slot ) ) {
      break;
    }

    /*
     * Establish ownership before the hardware request is enabled.
     */
    txb_info->slot = slot;
    txb_info->edge = qedge;
    txb_info->prio = qedge->edge_prio;

    ret = dcan_mo_write_frame(
      chip,
      txb_info->mobj,
      &slot->frame
    );

    if ( ret < 0 ) {
      txb_info->slot = NULL;
      txb_info->edge = NULL;
      txb_info->prio = 0;

      rtems_can_queue_push_back_outslot(
        qends,
        qedge,
        slot
      );

      break;
    }
  }
}

/**
 * @brief Configures a DCAN Message Object for transmission.
 *
 * This function configures the specified Message Object as a transmit
 * object and enables its transmission-complete interrupt.
 *
 * @param chip Pointer to CAN chip structure.
 * @param mobj Message Object number.
 *
 * @return 0 on success, negated errno otherwise.
 */
static int dcan_configure_tx(
  struct rtems_can_chip *chip,
  int mobj
)
{
  struct dcan_internal *internal = chip->internal;
  uint32_t ifarb;
  uint32_t ifmctl;
  uint32_t ifcmd;
  int ret;

  if ( mobj < 1 || mobj > 64 ) {
    return -EINVAL;
  }

  ret = dcan_wait_if_not_busy( chip, 2 );
  if ( ret < 0 ) {
    return ret;
  }

  dcan_write_reg( internal, DCAN_IFMSK( 2 ), 0u );

  ifmctl =
    DCAN_IFMCTL_EOB |
    DCAN_IFMCTL_TXIE;

  dcan_write_reg( internal, DCAN_IFMCTL( 2 ), ifmctl );

  ifarb =
    DCAN_IFARB_MSGVAL |
    DCAN_IFARB_DIR;

  dcan_write_reg( internal, DCAN_IFARB( 2 ), ifarb );

  ifcmd =
    DCAN_IFCMD_WR_RD |
    DCAN_IFCMD_MASK |
    DCAN_IFCMD_ARB |
    DCAN_IFCMD_CONTROL |
    DCAN_IFCMD_DATAA |
    DCAN_IFCMD_DATAB |
    DCAN_IFCMD_MSG_NUM( mobj );

  dcan_write_reg( internal, DCAN_IFCMD( 2 ), ifcmd );

  return dcan_wait_if_not_busy( chip, 2 );
}

static int dcan_configure_tx_buffers(
  struct rtems_can_chip *chip
)
{
  unsigned int mobj;
  int ret;

  for (
    mobj = DCAN_TX_MO_FIRST;
    mobj <= DCAN_TX_MO_LAST;
    ++mobj
  ) {
    ret = dcan_configure_tx(
      chip,
      mobj
    );

    if ( ret < 0 ) {
      return ret;
    }
  }

  return 0;
}

/**
 * @brief Configures a DCAN Message Object for reception.
 *
 * This function configures the specified Message Object as a receive
 * object for a standard CAN identifier. The receive interrupt and
 * acceptance mask are enabled.
 *
 * @param chip Pointer to CAN chip structure.
 * @param mobj Message Object number.
 * @param end_of_buffer End of buffer.
 *
 * @return 0 on success, negated errno otherwise.
 */
static int dcan_configure_rx( 
  struct rtems_can_chip *chip,
  unsigned int mobj,
  bool end_of_buffer )
{
  struct dcan_internal *internal = chip->internal;
  uint32_t ifarb;
  uint32_t ifmctl;
  uint32_t ifcmd;
  int ret;

  if ( mobj < 1u || mobj > 64u ) {
    return -EINVAL;
  }

  ret = dcan_wait_if_not_busy( chip, 1 );
  if ( ret < 0 ) {
    return ret;
  }

  /*
  * Accept all standard data frames.
  *
  * Identifier mask bits [28:18] remain zero, so the CAN identifier
  * is not compared. MXTD and MDIR still require a standard data frame.
  */
  dcan_write_reg( internal, DCAN_IFMSK( 1 ), 
    DCAN_IFMSK_MXTD |
    DCAN_IFMSK_MDIR );

  ifmctl = DCAN_IFMCTL_RXIE | DCAN_IFMCTL_UMASK;

  if ( end_of_buffer ) {
    ifmctl |= DCAN_IFMCTL_EOB;
  }

  dcan_write_reg( internal, DCAN_IFMCTL( 1 ), ifmctl );

  /*
   * Configure a valid receive Message Object for standard data frames.
   * DIR remains cleared for reception.
   */
  ifarb = DCAN_IFARB_MSGVAL;

  dcan_write_reg( internal, DCAN_IFARB( 1 ), ifarb );

  /*
   * Transfer the IF1 mask, arbitration, and control fields into the
   * selected Message Object and clear any stale interrupt state.
   */
  ifcmd = DCAN_IFCMD_WR_RD |
          DCAN_IFCMD_MASK |
          DCAN_IFCMD_ARB |
          DCAN_IFCMD_CONTROL |
          DCAN_IFCMD_CLRINTPND |
          DCAN_IFCMD_MSG_NUM( mobj );

  dcan_write_reg( internal, DCAN_IFCMD( 1 ), ifcmd );

  ret = dcan_wait_if_not_busy( chip, 1 );
  if ( ret < 0 ) {
    return ret;
  }
  return 0;
}

static int dcan_configure_rx_fifo(
  struct rtems_can_chip *chip
)
{
  unsigned int mobj;
  int ret;

  for (
    mobj = DCAN_RX_FIFO_FIRST_MOBJ;
    mobj <= DCAN_RX_FIFO_LAST_MOBJ;
    ++mobj
  ) {
    ret = dcan_configure_rx( chip, mobj, mobj == DCAN_RX_FIFO_LAST_MOBJ );
    if ( ret < 0 ) {
      return ret;
    }
  }
  return 0;
}

/**
 * @brief Reads a received CAN frame from a DCAN Message Object.
 *
 * This function transfers the selected receive Message Object from
 * message RAM to the IF1 registers, clears its interrupt pending and
 * NewDat flags, and copies the frame into the provided structure.
 *
 * @param chip Pointer to CAN chip structure.
 * @param mobj Message Object number.
 * @param frame Pointer to CAN frame storage.
 *
 * @return 1 on success, negated errno otherwise.
 */
static int dcan_mo_read_rx_message(
  struct rtems_can_chip *chip, 
  unsigned int mobj,
  struct can_frame *frame,
  struct dcan_rx_mo_status *status )
{
  struct dcan_internal *internal = chip->internal;
  uint32_t ifcmd;
  uint32_t ifarb;
  uint32_t ifmctl;
  uint32_t ifdata;
  uint32_t ifdatb;
  int ret;

  if ( mobj < 1u || mobj > 64u ||
    frame == NULL || status == NULL ) 
    { return -EINVAL; }

  ret = dcan_wait_if_not_busy( chip, 1 );
  if ( ret < 0 ) {
    return ret;
  }

  /*
   * Read the Message Object into IF1 and reset NewDat and IntPnd.
   *
   * IF1MCTL still reports the state that existed before these bits
   * were reset.
   */
  ifcmd = DCAN_IFCMD_MASK |
          DCAN_IFCMD_ARB |
          DCAN_IFCMD_CONTROL |
          DCAN_IFCMD_CLRINTPND |
          DCAN_IFCMD_TXRQST_NEWDAT |
          DCAN_IFCMD_DATAA |
          DCAN_IFCMD_DATAB |
          DCAN_IFCMD_MSG_NUM( mobj );

  dcan_write_reg( internal, DCAN_IFCMD( 1 ), ifcmd );

  ret = dcan_wait_if_not_busy( chip, 1 );
  if ( ret < 0 ) {
    return ret;
  }

  ifarb = dcan_read_reg( internal, DCAN_IFARB( 1 ));
  ifmctl = dcan_read_reg( internal, DCAN_IFMCTL( 1 ));

  status->mctl = ifmctl;

  status->new_data = ( ifmctl & DCAN_IFMCTL_NEWDAT ) != 0;

  status->message_lost = ( ifmctl & DCAN_IFMCTL_MSGLST ) != 0;

  status->end_of_buffer = ( ifmctl & DCAN_IFMCTL_EOB ) != 0;

  /*
   * The interrupt may have become stale before the worker handled it.
   * In this case there is no frame to copy.
   */
  if ( !status->new_data ) {
    return 0;
  }

  ifdata = dcan_read_reg( internal, DCAN_IFDATA( 1 ));
  ifdatb = dcan_read_reg( internal, DCAN_IFDATB( 1 ));
  *frame = (struct can_frame){};

  frame->header.can_id = ( ifarb >> 18 ) & 0x7ffu;
  frame->header.dlen = ifmctl & DCAN_IFMCTL_DATALENGTHCODE;

  frame->data[0] = ( ifdata >> 0 ) & 0xff;
  frame->data[1] = ( ifdata >> 8 ) & 0xff;
  frame->data[2] = ( ifdata >> 16 ) & 0xff;
  frame->data[3] = ( ifdata >> 24 ) & 0xff;
  frame->data[4] = ( ifdatb >> 0 ) & 0xff;
  frame->data[5] = ( ifdatb >> 8 ) & 0xff;
  frame->data[6] = ( ifdatb >> 16 ) & 0xff;
  frame->data[7] = ( ifdatb >> 24 ) & 0xff;

  return 1;
}

/**
 * For normal frames.
 */
static void dcan_process_received_frame(
  struct rtems_can_chip *chip, 
  struct can_frame *frame )
{
  /*
   * Deliver received frame to RTEMS CAN queue.
   * This is the real RTEMS CAN framework RX path.
   */
  rtems_can_queue_filter_frame_to_edges( &chip->qends_dev->base, NULL, 
    frame, 0 );

  rtems_can_stats_add_rx_done( &chip->chip_stats );

  rtems_can_stats_add_rx_bytes( &chip->chip_stats, frame->header.dlen );
}

/**
 * @brief Reports an RX FIFO overflow to the RTEMS CAN framework.
 *
 * MsgLst indicates that at least one unread frame was overwritten in
 * a receive Message Object.
 *
 * @param chip Pointer to CAN chip structure.
 */
static void dcan_report_rx_overflow(
  struct rtems_can_chip *chip
)
{
  struct can_frame err_frame = {};

  rtems_can_stats_add_rx_overflows( &chip->chip_stats );

  err_frame.header.can_id |= CAN_ERR_ID_CRTL;
  err_frame.header.flags |= CAN_FRAME_ERR;
  err_frame.header.dlen = 8;

  err_frame.data[CAN_ERR_DATA_BYTE_TRX_CTRL] = CAN_ERR_CRTL_RX_OVERFLOW;

  rtems_can_queue_filter_frame_to_edges( &chip->qends_dev->base, NULL,
    &err_frame, 0 );
}

static int dcan_clear_message_lost(
  struct rtems_can_chip *chip,
  unsigned int mobj,
  uint32_t ifmctl
)
{
  struct dcan_internal *internal = chip->internal;
  uint32_t ifcmd;
  int ret;

  ret = dcan_wait_if_not_busy( chip, 1 );
  if ( ret < 0 ) {
    return ret;
  }

  /*
   * The supplied IFMCTL value is the state captured before the RX read
   * command cleared NewDat and IntPnd. Do not write those stale bits
   * back into the Message Object.
   */
  ifmctl &= ~(
    DCAN_IFMCTL_MSGLST |
    DCAN_IFMCTL_NEWDAT |
    DCAN_IFMCTL_INTPND
  );

  dcan_write_reg( internal, DCAN_IFMCTL( 1 ), ifmctl );

  ifcmd =
    DCAN_IFCMD_WR_RD |
    DCAN_IFCMD_CONTROL |
    DCAN_IFCMD_CLRINTPND |
    DCAN_IFCMD_MSG_NUM( mobj );

  dcan_write_reg( internal, DCAN_IFCMD( 1 ), ifcmd );

  return dcan_wait_if_not_busy( chip, 1 );
}

/**
 * FIFO traversal
 */
static int dcan_process_rx_fifo(
  struct rtems_can_chip *chip,
  unsigned int first_mobj
)
{
  struct can_frame frame;
  struct dcan_rx_mo_status status;
  unsigned int mobj;
  int processed = 0;
  int ret;

  if ( !dcan_mobj_is_rx_fifo( first_mobj ) ) {
    return -EINVAL;
  }

  for (
    mobj = first_mobj;
    mobj <= DCAN_RX_FIFO_LAST_MOBJ;
    ++mobj
  ) {
    frame = ( struct can_frame ) {};
    status = ( struct dcan_rx_mo_status ) {};

    ret = dcan_mo_read_rx_message( chip, mobj, &frame,  &status );

    if ( ret < 0 ) {
      return ret;
    }

    /*
     * The interrupt was already cleared by the IF command, but no
     * unread frame remained in this object.
     */
    if ( !status.new_data ) {
      break;
    }

    if ( status.message_lost ) {
      dcan_report_rx_overflow( chip );

      ret = dcan_clear_message_lost( chip, mobj, status.mctl );
      if ( ret < 0 ) {
        return ret;
      }
    }

    dcan_process_received_frame( chip, &frame );
    ++processed;

    if ( status.end_of_buffer ) {
      break;
    }
  }

  return processed;
}

/**
 * @brief This function handles DCAN interrupts.
 *
 * The interrupt handler disables further DCAN interrupts and wakes the
 * worker task.  Message object processing is deferred to the worker.
 *
 * @param arg Pointer to CAN chip structure.
 */
static void dcan_interrupt( void *arg )
{
  struct rtems_can_chip *chip = arg;
  struct dcan_internal *internal = chip->internal;
  uint32_t ctl;

  /*
   * Keep ISR short:
   * disable DCAN interrupt and wake worker.
   */
  ctl = dcan_read_reg( internal, DCAN_CTL );
  ctl &= ~DCAN_CTL_IE0;
  dcan_write_reg( internal, DCAN_CTL, ctl );

  rtems_binary_semaphore_post( &chip->qends_dev->worker_sem );
}

static void dcan_enable_interrupts( struct rtems_can_chip *chip )
{
  struct dcan_internal *internal = chip->internal;
  uint32_t ctl;

  ctl = dcan_read_reg( internal, DCAN_CTL );
  ctl |= DCAN_CTL_IE0 | DCAN_CTL_EIE | DCAN_CTL_SIE;
  dcan_write_reg( internal, DCAN_CTL, ctl );
}

/**
 * @brief Clear the interrupt pending flag for a specific message object.
 *
 * @param chip Pointer to CAN chip structure.
 * @param mobj Message object number.
 *
 * @return 0 on success, negative error code on failure.
 */
static int dcan_clear_msg_obj_interrupt(
  struct rtems_can_chip *chip,
  unsigned int mobj
)
{
  struct dcan_internal *internal = chip->internal;
  uint32_t ifcmd;
  int ret;

  ret = dcan_wait_if_not_busy( chip, 1 );
  if ( ret < 0 ) {
    return ret;
  }

  ifcmd = DCAN_IFCMD_CLRINTPND | DCAN_IFCMD_MSG_NUM( mobj );

  dcan_write_reg( internal, DCAN_IFCMD( 1 ), ifcmd );

  return dcan_wait_if_not_busy( chip, 1 );
}

/**
 * @brief Determines the current DCAN controller fault state.
 *
 * @param es Error and Status register value.
 *
 * @return Current CAN controller state.
 */
static enum can_state dcan_read_fault_state(
  uint32_t es
)
{
  if (( es & DCAN_ES_BOFF ) != 0u ) {
    return CAN_STATE_BUS_OFF;
  }

  if (( es & DCAN_ES_EPASS ) != 0u ) {
    return CAN_STATE_ERROR_PASSIVE;
  }

  if (( es & DCAN_ES_EWARN ) != 0u ) {
    return CAN_STATE_ERROR_WARNING;
  }

  return CAN_STATE_ERROR_ACTIVE;
}

/**
 * @brief Reads the DCAN transmit and receive error counters.
 *
 * @param chip Pointer to CAN chip structure.
 * @param txerr Output transmit error counter.
 * @param rxerr Output receive error counter.
 * @param rx_passive Output receive error passive indication.
 */
static void dcan_read_error_counters(
  struct rtems_can_chip *chip,
  uint8_t *txerr,
  uint8_t *rxerr,
  bool *rx_passive
)
{
  struct dcan_internal *internal = chip->internal;
  uint32_t errc;

  errc = dcan_read_reg( internal, DCAN_ERRC );

  *txerr = (uint8_t)FIELD_GET( DCAN_ERRC_TEC, errc );

  *rxerr = (uint8_t)FIELD_GET( DCAN_ERRC_REC, errc );

  *rx_passive = ( errc & DCAN_ERRC_RP ) != 0u;
}

/**
 * @brief Abort all pending transmit frames.
 *
 * This function aborts the frame currently assigned to the hardware TX object
 * and returns all frames still waiting in the software TX queue, marking them
 * as transmission errors.
 */
static void dcan_abort_pending_tx(
  struct rtems_can_chip *chip
)
{
  struct dcan_internal *internal = chip->internal;
  struct rtems_can_queue_ends *qends = &chip->qends_dev->base;
  struct rtems_can_queue_edge *qedge;
  struct rtems_can_queue_slot *slot;
  struct dcan_txb_info *txb_info;
  unsigned int index;

  /*
   * First return frames already assigned to hardware TX Message Objects.
   */
  for ( index = 0; index < DCAN_TX_MO_COUNT; ++index ) {
    txb_info = &internal->txb_info[ index ];

    if ( txb_info->slot == NULL ) {
      continue;
    }

    ( void ) dcan_abort_tx_mobj(
      chip,
      txb_info->mobj
    );

    rtems_can_queue_filter_frame_to_edges(
      qends,
      txb_info->edge,
      &txb_info->slot->frame,
      CAN_FRAME_TXERR
    );

    rtems_can_stats_add_tx_error(
      &chip->chip_stats
    );

    rtems_can_queue_free_outslot(
      qends,
      txb_info->edge,
      txb_info->slot
    );

    txb_info->slot = NULL;
    txb_info->edge = NULL;
    txb_info->prio = 0;
  }

  /*
   * Then drain frames which never reached a hardware Message Object.
   */
  while (
    rtems_can_queue_test_outslot(
      qends,
      &qedge,
      &slot
    ) >= 0
  ) {
    rtems_can_queue_filter_frame_to_edges(
      qends,
      qedge,
      &slot->frame,
      CAN_FRAME_TXERR
    );

    rtems_can_stats_add_tx_error(
      &chip->chip_stats
    );

    rtems_can_queue_free_outslot(
      qends,
      qedge,
      slot
    );
  }

  internal->tx_error_reported = false;
}

/**
 * @brief Adds DCAN controller state information to an error frame.
 *
 * @param chip Pointer to CAN chip structure.
 * @param es Error and Status register value.
 * @param err_frame Error frame being constructed.
 * @param txerr Transmit error counter.
 * @param rxerr Receive error counter.
 * @param rx_passive Receive error passive indication.
 *
 * @return true if an error frame should be reported, otherwise false.
 */
static bool dcan_add_state_error(
  struct rtems_can_chip *chip,
  uint32_t es,
  struct can_frame *err_frame,
  uint8_t txerr,
  uint8_t rxerr,
  bool rx_passive
)
{
  struct dcan_internal *internal = chip->internal;
  enum can_state new_state;

  new_state = dcan_read_fault_state( es );

  /*
   * Report controller state only when it changes.
   */
  if ( new_state == internal->state ) {
    return false;
  }

  internal->state = new_state;

  rtems_can_stats_set_state( &chip->chip_stats, new_state );

  switch (new_state) {
    case CAN_STATE_ERROR_ACTIVE:
      err_frame->header.can_id |= CAN_ERR_ID_CRTL | CAN_ERR_ID_CNT;
      err_frame->data[CAN_ERR_DATA_BYTE_TRX_CTRL] |= CAN_ERR_CRTL_ACTIVE;
      break;

    case CAN_STATE_ERROR_WARNING:
      err_frame->header.can_id |= CAN_ERR_ID_CRTL | CAN_ERR_ID_CNT;

      if (txerr >= rxerr) {
        err_frame->data[CAN_ERR_DATA_BYTE_TRX_CTRL] |= CAN_ERR_CRTL_TX_WARNING;
      } else {
        err_frame->data[CAN_ERR_DATA_BYTE_TRX_CTRL] |= CAN_ERR_CRTL_RX_WARNING;
      }

      break;

    case CAN_STATE_ERROR_PASSIVE:
      err_frame->header.can_id |= CAN_ERR_ID_CRTL | CAN_ERR_ID_CNT;

      if (rx_passive) {
        err_frame->data[CAN_ERR_DATA_BYTE_TRX_CTRL] |= CAN_ERR_CRTL_RX_PASSIVE;
      } else {
        err_frame->data[CAN_ERR_DATA_BYTE_TRX_CTRL] |= CAN_ERR_CRTL_TX_PASSIVE;
      }

      break;

    case CAN_STATE_BUS_OFF:
      err_frame->header.can_id |= CAN_ERR_ID_BUSOFF | CAN_ERR_ID_CNT;

      break;

    default:
      return false;
  }

  err_frame->data[CAN_ERR_DATA_BYTE_CNT_TX] = txerr;
  err_frame->data[CAN_ERR_DATA_BYTE_CNT_RX] = rxerr;

  return true;
}

/**
 * @brief Adds DCAN Last Error Code information to an error frame.
 *
 * @param chip Pointer to CAN chip structure.
 * @param es Error and Status register value.
 * @param err_frame Error frame being constructed.
 *
 * @return true if a protocol error was added, otherwise false.
 */
static bool dcan_add_protocol_error(
  struct rtems_can_chip *chip,
  uint32_t es,
  struct can_frame *err_frame
)
{
  struct dcan_internal *internal = chip->internal;
  uint32_t lec;

  lec = es & DCAN_ES_LEC;

  switch (lec) {
    case DCAN_ES_LEC_NO_ERROR:
    case DCAN_ES_LEC_NO_EVENT:
      return false;

    case DCAN_ES_LEC_STUFF_ERROR:
      err_frame->header.can_id |= CAN_ERR_ID_PROT | CAN_ERR_ID_BUSERROR;
      err_frame->data[CAN_ERR_DATA_BYTE_TRX_PROT] |= CAN_ERR_PROT_STUFF;
      break;

    case DCAN_ES_LEC_FORM_ERROR:
      err_frame->header.can_id |= CAN_ERR_ID_PROT | CAN_ERR_ID_BUSERROR;
      err_frame->data[CAN_ERR_DATA_BYTE_TRX_PROT] |= CAN_ERR_PROT_FOR;
      break;

    case DCAN_ES_LEC_ACK_ERROR:
     /*
      * DCAN automatically retransmits unacknowledged frames. Report the
      * ACK error once while at least one hardware TX buffer remains
      * pending.
      */
      if (
        dcan_has_pending_tx( internal ) && internal->tx_error_reported) {
        return false;
      }

      err_frame->header.can_id |= CAN_ERR_ID_ACK | CAN_ERR_ID_BUSERROR;
      err_frame->data[CAN_ERR_DATA_BYTE_TRX_PROT_LOC] = CAN_ERR_PROT_LOC_ACK;

      if ( dcan_has_pending_tx( internal ) ) {
        internal->tx_error_reported = true; 
      }
      break;

    case DCAN_ES_LEC_BIT1_ERROR:
      err_frame->header.can_id |= CAN_ERR_ID_PROT | CAN_ERR_ID_BUSERROR;
      err_frame->data[CAN_ERR_DATA_BYTE_TRX_PROT] |= CAN_ERR_PROT_BIT1;
      break;

    case DCAN_ES_LEC_BIT0_ERROR:
      err_frame->header.can_id |= CAN_ERR_ID_PROT | CAN_ERR_ID_BUSERROR;
      err_frame->data[CAN_ERR_DATA_BYTE_TRX_PROT] |= CAN_ERR_PROT_BIT0;
      break;

    case DCAN_ES_LEC_CRC_ERROR:
      err_frame->header.can_id |= CAN_ERR_ID_PROT | CAN_ERR_ID_BUSERROR;
      err_frame->data[CAN_ERR_DATA_BYTE_TRX_PROT] |= CAN_ERR_PROT_UNSPEC;
      err_frame->data[CAN_ERR_DATA_BYTE_TRX_PROT_LOC] = 
        CAN_ERR_PROT_LOC_CRCSEQ;
      break;

    default:
      return false;
  }

  return true;
}

/**
 * @brief Processes a DCAN Error and Status register update.
 *
 * This function translates DCAN controller-state changes and Last Error
 * Code information into an RTEMS CAN error frame.
 *
 * @param chip Pointer to CAN chip structure.
 * @param es Error and Status register value.
 */
static void dcan_process_error_status(
  struct rtems_can_chip *chip,
  uint32_t es
)
{
  struct can_frame err_frame = { 0 };
  uint8_t txerr;
  uint8_t rxerr;
  bool rx_passive;
  bool report;

  dcan_read_error_counters(
    chip,
    &txerr,
    &rxerr,
    &rx_passive
  );

  err_frame.header.flags = CAN_FRAME_ERR;
  err_frame.header.dlen = 8;

  report = false;

  /*
   * Add a protocol error only when it is a new reportable event.
   */
  report |= dcan_add_protocol_error( chip, es, &err_frame );

  /*
   * Add controller state information only on a state transition.
   */
  report |= dcan_add_state_error( chip, es, &err_frame, txerr, rxerr, 
    rx_passive );

  /*
   * Include error counters whenever an error frame is generated.
   */
  if (report) {
    err_frame.header.can_id |= CAN_ERR_ID_CNT;

    err_frame.data[CAN_ERR_DATA_BYTE_CNT_TX] = txerr;
    err_frame.data[CAN_ERR_DATA_BYTE_CNT_RX] = rxerr;

    rtems_can_queue_filter_frame_to_edges(
      &chip->qends_dev->base,
      NULL,
      &err_frame,
      0
    );
  }
}

/**
 * Process D-CAN interrupts.
 */
static int dcan_process_interrupts( struct rtems_can_chip *chip )
{
  struct dcan_internal *internal = chip->internal;
  uint32_t interrupt;
  uint32_t msgindex;
  int handled = 0;
  int ret;

  while ( true ) {
    interrupt = dcan_read_reg( internal, DCAN_INT );

    if ( interrupt == 0u ) {
      break;
    }

    /*
    * Status/error interrupt.
    *
    * This will later call the DCAN error-frame handler.
    * Reading DCAN_ES is normally required to acknowledge it.
    */
    if ((interrupt & DCAN_INT_INT0ID) == DCAN_INT_STATUS_ID ) 
    {
      uint32_t es;
      es = dcan_read_reg( internal, DCAN_ES );
      dcan_process_error_status( chip, es );
      ++handled;
      continue;
    }

    /*
    * 1..64 means Message Object interrupt.
    */
    msgindex = interrupt & 0x7fffu;

    if ( dcan_mobj_is_tx( msgindex ) ) {
      ret = dcan_clear_msg_obj_interrupt(
        chip,
        msgindex
      );
      if ( ret < 0 ) {
        return ret;
      }
      dcan_process_tx_finish( chip, msgindex );
      ++handled;
      continue;
    }

    if ( dcan_mobj_is_rx_fifo( msgindex ) ) {
        ret = dcan_process_rx_fifo( chip, msgindex );
        if ( ret < 0 ) {
          return ret;
        }
        handled += ret;
        continue;
      }

    /*
     * Prevent an unexpected Message Object interrupt from keeping
     * the interrupt line asserted forever.
     */
    ret = dcan_clear_msg_obj_interrupt( chip, msgindex );
    if ( ret < 0 ) {
      return ret;
    }
    ++handled;
  }  
  return handled;
}

/**
 * @brief This is the main worker for DCAN TX, RX, and stop processing.
 *
 * The worker is released by the interrupt handler, the transmit queue,
 * or a controller stop request. While the controller is running, it
 * processes interrupts and pending transmit frames. During controller
 * shutdown, it aborts outstanding hardware and software TX frames.
 *
 * @param arg RTEMS task argument containing a pointer to the CAN chip.
 */
static rtems_task dcan_worker( rtems_task_argument arg )
{
  struct rtems_can_chip *chip;
  struct dcan_internal *internal;

  chip = ( struct rtems_can_chip * ) arg;
  internal = chip->internal;

  while ( 1 ) {
    rtems_binary_semaphore_wait(
      &chip->qends_dev->worker_sem
    );

    if ( atomic_load( &internal->do_stop ) ) {
      dcan_abort_pending_tx( chip );
      atomic_store( &internal->do_stop, false );
      rtems_binary_semaphore_post( &chip->stop_sem );
      continue;
    }

    if ( !rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) ) {
      continue;
    }

    /*
     * Drain all pending controller interrupts first.  TX completion frees
     * hardware Message Objects and their corresponding RTEMS queue slots.
     */
    ( void ) dcan_process_interrupts( chip );

    /*
     * Fill every currently free hardware TX Message Object.
     */
    dcan_process_tx( chip );

    if (
      rtems_can_test_bit(
        RTEMS_CAN_CHIP_RUNNING,
        &chip->flags
      ) &&
      !atomic_load( &internal->do_stop )
    ) {
      dcan_enable_interrupts( chip );
    }
  }
}

static int dcan_get_chip_info(
  struct rtems_can_chip *chip,
  int what
)
{
  int ret;

  switch ( what ) {
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

static int dcan_chip_ioctl(
  struct rtems_can_chip *chip,
  ioctl_command_t command,
  void *arg
)
{
  ( void ) chip;
  ( void ) command;
  ( void ) arg;

  return -ENOTSUP;
}

static int dcan_stop_chip(
  struct rtems_can_chip *chip,
  struct timespec *ts
)
{
  struct dcan_internal *internal = chip->internal;
  struct timespec final;
  struct timespec curr;
  struct timespec towait;
  rtems_interval timeout;
  uint32_t ctl;
  int ret;

  rtems_mutex_lock( &chip->lock );

  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 0 ) 
  {
    rtems_mutex_unlock( &chip->lock );
    return 0;
  }

  /*
   * Disable the interrupt output and the status/error
   * interrupt sources for this controller run.
   */
  ctl = dcan_read_reg( internal, DCAN_CTL );
  ctl &= ~ ( DCAN_CTL_IE0| DCAN_CTL_EIE | DCAN_CTL_SIE );
  dcan_write_reg( internal, DCAN_CTL, ctl );

  /*
   * Mark the controller as no longer running so that no new normal
   * worker processing is started during shutdown.
   */
  rtems_can_clear_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags );

  /*
   * Request TX cleanup from the worker.
   */
  atomic_store( &internal->do_stop, true );

  /*
   * Wake the worker to perform stop cleanup.
   */
  rtems_binary_semaphore_post( &chip->qends_dev->worker_sem );

  if ( ts != NULL ) {
    /*
     * Convert the relative timeout supplied by the caller into an
     * absolute monotonic deadline.
     */
    clock_gettime( CLOCK_MONOTONIC, &final );
    rtems_timespec_add_to( &final, ts );

    /*
     * Calculate the remaining time before waiting for the worker.
     */
    clock_gettime( CLOCK_MONOTONIC, &curr );
    rtems_timespec_subtract( &curr, &final, &towait );

    if ( towait.tv_sec < 0 ) {
      ret = rtems_binary_semaphore_try_wait( &chip->stop_sem );
    } else {
      timeout = rtems_timespec_to_ticks( &towait );
      ret = rtems_binary_semaphore_wait_timed_ticks( &chip->stop_sem, 
        timeout );
    }
  } else {
    /*
     * Without a timeout, do not block. Only check whether the worker
     * has already completed the stop cleanup.
     */
    ret = rtems_binary_semaphore_try_wait( &chip->stop_sem );
  }
  if ( ret < 0 ) {
    /*
     * The worker did not clear all queued frames before the timeout.
     * Flush frames that remain in the software TX outlist.
     */
    rtems_can_queue_ends_flush_outlist(
      &chip->qends_dev->base
    );
  }

  /*
   * Disable DCAN interrupts and enter initialization mode.
   */
  ctl = dcan_read_reg( internal, DCAN_CTL );
  ctl |= DCAN_CTL_INIT;
  dcan_write_reg( internal, DCAN_CTL, ctl );

  /*
   * Reset the software fault state and discard any TXOK saved from the
   * previous controller run.
   */
  internal->state = CAN_STATE_STOPPED;
  internal->tx_error_reported = false;

  rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_STOPPED );

  rtems_can_bus_notify_chip_stop( chip );

  rtems_mutex_unlock( &chip->lock );

  return 0;
}

static int dcan_start_chip( struct rtems_can_chip *chip )
{
  struct dcan_internal *internal = chip->internal;
  unsigned int index;
  int ret = 0;

  rtems_mutex_lock( &chip->lock );

  if (
    rtems_can_test_bit(
      RTEMS_CAN_CHIP_RUNNING,
      &chip->flags
    )
  ) {
    goto start_chip_unlock;
  }

  ret = dcan_enter_init_config_mode( chip );
  if ( ret < 0 ) {
    goto start_chip_unlock;
  }

  if ( chip->bittiming.bitrate == 0 ) {
    ret = -EINVAL;
    goto start_chip_unlock;
  }

  ret = dcan_set_bittiming( chip );
  if ( ret < 0 ) {
    goto start_chip_unlock;
  }

  ret = dcan_configure_tx_buffers( chip );
  if ( ret < 0 ) {
    goto start_chip_unlock;
  }

  ret = dcan_configure_rx_fifo( chip );
  if ( ret < 0 ) {
    goto start_chip_unlock;
  }

  for ( index = 0; index < DCAN_TX_MO_COUNT; ++index ) {
    internal->txb_info[ index ].slot = NULL;
    internal->txb_info[ index ].edge = NULL;
    internal->txb_info[ index ].prio = 0;
    internal->txb_info[ index ].mobj =
      dcan_tx_index_to_mobj( index );
  }

  internal->state = CAN_STATE_ERROR_ACTIVE;
  internal->tx_error_reported = false;

  ret = dcan_leave_init_mode( chip );
  if ( ret < 0 ) {
    goto start_chip_unlock;
  }

  rtems_binary_semaphore_try_wait( &chip->stop_sem );

  rtems_can_set_bit(
    RTEMS_CAN_CHIP_RUNNING,
    &chip->flags
  );

  rtems_can_stats_reset( &chip->chip_stats );
  rtems_can_stats_set_state(
    &chip->chip_stats,
    CAN_STATE_ERROR_ACTIVE
  );

  dcan_enable_interrupts( chip );

start_chip_unlock:
  rtems_mutex_unlock( &chip->lock );

  return ret;
}

static void dcan_register( struct rtems_can_chip_ops *chip_ops )
{
  chip_ops->start_chip = dcan_start_chip;
  chip_ops->stop_chip = dcan_stop_chip;
  chip_ops->chip_ioctl =  dcan_chip_ioctl;
  chip_ops->get_chip_timestamp = NULL;
  chip_ops->get_chip_info = dcan_get_chip_info;

  chip_ops->calc_bittiming = dcan_calc_and_set_bittiming;
  chip_ops->check_and_set_bittiming = dcan_check_and_set_bittiming;
}


struct rtems_can_chip *rtems_can_dcan_initialize(
  uintptr_t base, 
  rtems_vector_number irq, 
  unsigned long can_clk_rate )
{
  struct rtems_can_chip *chip;
  struct dcan_internal *internal;
  rtems_status_code sc;

  internal = calloc( 1, sizeof( *internal ) );
  if ( internal == NULL ) {
    return NULL;
  }

  chip = calloc( 1, sizeof( *chip ));
  if ( chip == NULL ) {
    free( internal );
    return NULL;
  }

  chip->qends_dev = calloc( 1, sizeof( *chip->qends_dev ) );
  if ( chip->qends_dev == NULL ) {
    free( internal );
    free( chip );
    return NULL;
  }
  internal->base = base;

  atomic_init( &internal->do_stop, false );

  /*
  * The controller has been allocated, but it has not been started yet.
  */
  internal->state = CAN_STATE_STOPPED;
  internal->tx_error_reported = false;

  chip->type = "dcan";
  chip->irq = irq;
  chip->freq = can_clk_rate;
  chip->ctrlmode_supported = 0;
  chip->bittiming_const = &dcan_bittiming_const;
  chip->internal = internal;
  
  dcan_register( &chip->chip_ops );
  rtems_mutex_init( &chip->lock, "dcan_lock" );

  rtems_can_queue_ends_init_chip( chip, "can_dcan_worker" );

  rtems_binary_semaphore_init( &chip->stop_sem, "can_dcan_stop_sem" );

  sc = rtems_interrupt_handler_install( chip->irq, "dcan", 
    RTEMS_INTERRUPT_UNIQUE, dcan_interrupt, chip );

  if ( sc != RTEMS_SUCCESSFUL ) {
    free( chip->qends_dev );
    free( chip );
    free( internal );
    return NULL;
  }

  sc = rtems_task_create(
  rtems_build_name( 'D', 'C', 'A', 'N' ),
  100,
  RTEMS_MINIMUM_STACK_SIZE + 0x400,
  RTEMS_DEFAULT_MODES,
  RTEMS_DEFAULT_ATTRIBUTES,
  &internal->worker_task_id
);

if ( sc != RTEMS_SUCCESSFUL ) {
  rtems_interrupt_handler_remove( chip->irq, dcan_interrupt, chip );
  free( chip->qends_dev );
  free( chip );
  free( internal );
  return NULL;
}

sc = rtems_task_start( internal->worker_task_id, dcan_worker,
  ( rtems_task_argument ) chip );
if (sc != RTEMS_SUCCESSFUL) {
  rtems_interrupt_handler_remove( chip->irq, dcan_interrupt, chip );
  free( chip->qends_dev );
  free( chip );
  free( internal );
  return NULL;
}

rtems_can_set_bit( RTEMS_CAN_CHIP_CONFIGURED, &chip->flags );

rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_STOPPED );
return chip;
}
