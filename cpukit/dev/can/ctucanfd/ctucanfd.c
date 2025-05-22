/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CTUCANFD
 *
 * @brief This source file contains the implementation of CTU CAN FD
 *   controller.
 */

/*
 * Copyright (C) 2023-2024 Michal Lenc <michallenc@seznam.cz>
 * Implementation is based on Linux CTU CAN FD dlriver
 * Copyright (C) 2015-2018 Ondrej Ille <ondrej.ille@gmail.com> FEE CTU
 * Copyright (C) 2018-2021 Ondrej Ille <ondrej.ille@gmail.com> self-funded
 * Copyright (C) 2018-2019 Martin Jerabek <martin.jerabek01@gmail.com> FEE CTU
 * Copyright (C) 2018-2022 Pavel Pisa <pisa@cmp.felk.cvut.cz> FEE CTU/self-funded
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the folowing disclaimer.
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

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/score/basedefs.h>

#include <dev/can/can-devcommon.h>
#include <dev/can/can-helpers.h>
#include <dev/can/can.h>

#include <dev/can/ctucanfd.h>
#include "ctucanfd_kframe.h"
#include "ctucanfd_kregs.h"
#include "ctucanfd_txb.h"
#include "ctucanfd_internal.h"

#ifdef RTEMS_DEBUG
# define CTUCANFD_DEBUG 1
#else
# undef CTUCANFD_DEBUG
#endif

/**
 * @brief The magic number synthesized to the core. See 3.1.1 DEVICE_ID
 *  register description in CTU CAN FD Core Datasheet Documentation.
 */
#define CTUCANFD_ID 0xCAFD
#define CTUCANFD_ID_MASK 0xFFFF

/**
 * @brief This defines initial transmit buffer order. Every 4 bits correspond
 *  to one buffer number. The buffer located on the least significant 4 bits
 *  has the highest priority.
 */
#define CTUCANFD_INIT_TXB_ORDER  0x76543210
#define CAN_CTUCANFD_TYPE "ctucanfd"

enum ctucanfd_txtb_status {
  /* Buffer does not exists in the core */
  TXT_NOT_EXIST       = 0x0,
  /* Buffer is in ready state */
  TXT_RDY             = 0x1,
  /* Buffer is in TX in progress state */
  TXT_TRAN            = 0x2,
  /* Buffer is in abort in progress state */
  TXT_ABTP            = 0x3,
  /* Buffer is in TX OK state */
  TXT_TOK             = 0x4,
  /* Buffer is in failed state */
  TXT_ERR             = 0x6,
  /* Buffer is in aborted state */
  TXT_ABT             = 0x7,
  /* Buffer is in empty state */
  TXT_ETY             = 0x8,
};

enum ctucanfd_txtb_command {
  /* Requests TXT buffer to move to "Empty" state */
  TXT_CMD_SET_EMPTY   = 0x01,
  /* Requests TXT buffer to move to "Aborted" or "Abort in progress" state */
  TXT_CMD_SET_READY   = 0x02,
  /* Requests TXT buffer to move to "Ready" state*/
  TXT_CMD_SET_ABORT   = 0x04
};

static const struct rtems_can_bittiming_const ctucanfd_nominal_bittiming_const = {
  .name = "ctucanfd_nominal",
  .tseg1_min = 2,
  .tseg1_max = 190,
  .tseg2_min = 1,
  .tseg2_max = 63,
  .sjw_max = 31,
  .brp_min = 1,
  .brp_max = 8,
  .brp_inc = 1,
};

static const struct rtems_can_bittiming_const ctucanfd_data_bittiming_const = {
  .name = "ctucanfd_data",
  .tseg1_min = 2,
  .tseg1_max = 94,
  .tseg2_min = 1,
  .tseg2_max = 31,
  .sjw_max = 31,
  .brp_min = 1,
  .brp_max = 2,
  .brp_inc = 1,
};

#ifdef CTUCANFD_DEBUG

static int ctucanfd_check_state(
  struct ctucanfd_internal *internal,
  const char *where
)
{
  int i;
  int ret = 0;
  int prio = 0;
  unsigned int idx_limit = 0;
  uint32_t txb_alloc = 0;
  uint32_t txb_found = 0;
  uint32_t tx_status = ctucanfd_read32( internal, CTUCANFD_TX_STATUS );

  for (
    prio = 0,
    idx_limit = internal->ntxbufs;
    prio < RTEMS_CAN_QUEUE_PRIO_NR; prio++
  ) {
    if ( internal->txb_prio_tail[prio] > internal->ntxbufs) {
      printk( "CTUCANFD CHECK at %s: prio_tail[%d] %d > ntxbufs %d\n",
              where, prio, internal->txb_prio_tail[prio], internal->ntxbufs );
      ret = -1;
    } else if ( internal->txb_prio_tail[prio] > idx_limit ) {
      printk( "CTUCANFD CHECK at %s: prio_tail[%d] %d > lower prio tail %d\n",
              where, prio, internal->txb_prio_tail[prio], idx_limit );
      ret = -1;
    }
    idx_limit = internal->txb_prio_tail[prio];
  }

  for ( i = 0; i < internal->ntxbufs; i++ ) {
    uint32_t mask;
    unsigned int txtb_st;
    unsigned int txtb_id;

    txtb_id = ctucanfd_txb_from_order( internal->txb_order, i );
    if ( txtb_id >= internal->ntxbufs ) {
      printk(
        "CTUCANFD CHECK at %s: txb_order[%d] %d >= ntxbufs %d, "
        "txb_order = 0x%08x\n",
        where,
        i,
        txtb_id,
        internal->ntxbufs,
        internal->txb_order
      );
      ret = -1;
      continue;
    }
    mask = 1 << txtb_id;
    if ( txb_found & mask ) {
      printk(
        "CTUCANFD CHECK at %s: txtb_id %d doubled in txb_order[%d], "
        "txb_order = 0x%08x\n",
        where,
        txtb_id,
        i,
        internal->txb_order
      );
      ret = -1;
    }
    txb_found |= mask;
    txtb_st = TXTB_GET_STATUS( tx_status, txtb_id );
    if ( i < internal->txb_prio_tail[0]) {
      txb_alloc |= mask;
      if ( txtb_st == TXT_ETY ) {
        printk(
          "CTUCANFD CHECK at %s: txtb_id %d @ txb_order[%d] is EMPTY but "
          "before tail %d, txb_order = 0x%08x\n",
          where,
          txtb_id,
          i,
          internal->txb_prio_tail[0],
          internal->txb_order
        );
        ret = -1;
      }
    } else {
      if ( txtb_st != TXT_ETY ) {
        printk(
          "CTUCANFD CHECK at %s: txtb_id %d @ txb_order[%d] is not EMPTY "
          "but after tail %d, txb_order = 0x%08x\n",
          where,
          txtb_id,
          i,
          internal->txb_prio_tail[0],
          internal->txb_order
        );
        ret = -1;
      }
    }
  }

  if ( txb_found != ( 1 << internal->ntxbufs ) - 1 ) {
     printk(
      "CTUCANFD CHECK at %s: missing buffers in txb_order = 0x%08x, "
      "txb_found = 0x%02x ntxbufs %d, tail %d\n",
      where,
      internal->txb_order,
      txb_found,
      internal->ntxbufs,
      internal->txb_prio_tail[0]
    );
     ret = -1;
  }
  for ( i = 0; i < internal->ntxbufs; i++ ) {
    if ( txb_alloc & ( 1 << i ) ) {
      if (
        ( internal->txb_info[i].slot == NULL ) ||
        ( internal->txb_info[i].edge == NULL )
      ) {
        printk(
          "CTUCANFD CHECK at %s: txb_info[%d] not filled, but active in "
          "txb_order = 0x%08x, ntxbufs %d, tail %d\n",
          where,
          i,
          internal->txb_order,
          internal->ntxbufs,
          internal->txb_prio_tail[0]
        );
        ret = -1;
      }
    } else {
      if (
        ( internal->txb_info[i].slot != NULL ) ||
        ( internal->txb_info[i].edge != NULL )
      ) {
        printk(
          "CTUCANFD CHECK at %s: txb_info[%d] filled, but shoul be inactive "
          "active in txb_order = 0x%08x, ntxbufs %d, tail %d\n",
           where,
           i,
           internal->txb_order,
           internal->ntxbufs,
           internal->txb_prio_tail[0]
          );
        ret = -1;
      }
    }
  }
  return ret;
}

#else /*CTUCANFD_DEBUG*/

static int ctucanfd_check_state(
  struct ctucanfd_internal *internal,
  const char *where
) {
  return 0;
}

#endif /*CTUCANFD_DEBUG*/

static int ctucanfd_get_timestamp( struct rtems_can_chip *chip, uint64_t *timestamp )
{
  struct ctucanfd_internal *internal = chip->internal;
  uint32_t lower;
  uint32_t upper;
  uint32_t upper_check;

  upper = ctucanfd_read32( internal, CTUCANFD_TIMESTAMP_HIGH );
  lower = ctucanfd_read32( internal, CTUCANFD_TIMESTAMP_LOW );
  upper_check = ctucanfd_read32( internal, CTUCANFD_TIMESTAMP_HIGH );
  if ( upper != upper_check ) {
    upper = ctucanfd_read32( internal, CTUCANFD_TIMESTAMP_HIGH );
    lower = ctucanfd_read32( internal, CTUCANFD_TIMESTAMP_LOW );
  }

  *timestamp = ( ( uint64_t )upper << 32) | lower;

  return 0;
}

static int ctucanfd_get_chip_info( struct rtems_can_chip *chip, int what )
{
  int ret;

  switch( what ) {
    case RTEMS_CAN_CHIP_BITRATE:
      ret = chip->bittiming.bitrate;
      break;
    case RTEMS_CAN_CHIP_DBITRATE:
      ret = chip->data_bittiming.bitrate;
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
 * @brief  This function issues software reset request to CTU CAN FD.
 *
 * @param  internal Pointer to internal CTU CAN FD structure.
 *
 * @return 0 for success, -ETIMEDOUT if CAN controller does not leave reset
 */
static int ctucanfd_reset( struct ctucanfd_internal *internal )
{
  uint16_t dev_id;
  int i = 100;

  ctucanfd_write32( internal, CTUCANFD_MODE, REG_MODE_RST );

  while ( i ) {
    /* According to CTU CAN FD documentation, if HW reset was issued to
     * the driver shall not be accessed for two clock periods of System clock.
     * For example if CTU CAN FD System clock is 100 MHz, SW shall wait 20 ns
     * after HW reset was released.
     *
     * This while loop check is to avoid stuck when there is problem with
     * FPGA synthesis or some clocks blocked, not configured.
     */
    dev_id = FIELD_GET( REG_DEVICE_ID_DEVICE_ID,
                        ctucanfd_read32( internal, CTUCANFD_DEVICE_ID ) );

    if ( dev_id == CTUCANFD_ID ) {
      return 0;
    }

    i--;
  }

  return -ETIMEDOUT;
}

/**
 * @brief  This function sets CAN bus bit timing in CTU CAN FD
 *
 * @param  chip     Pointer to chip structure.
 * @param  bt       Pointer to bit timing structure.
 * @param  nominal  True - Nominal bit timing, False - Data bit timing
 *
 * @return 0 for success, -EPERM if CAN controller is enabled
 */
static int ctucanfd_set_btr(
  struct rtems_can_chip *chip,
  struct rtems_can_bittiming *bt,
  bool nominal
)
{
  struct ctucanfd_internal *internal = chip->internal;
  int max_ph1_len = 31;
  uint32_t btr = 0;
  uint32_t prop_seg = bt->prop_seg;
  uint32_t phase_seg1 = bt->phase_seg1;

  if ( CTU_CAN_FD_ENABLED( internal ) ) {
    return -EPERM;
  }

  if ( nominal ) {
    max_ph1_len = 63;
  }

  /* The timing calculation functions have only constraints on tseg1, which is
   * prop_seg + phase1_seg combined. tseg1 is then split in half and stored
   * into prog_seg and phase_seg1. In CTU CAN FD, PROP is 6/7 bits wide
   * but PH1 only 6/5, so we must re-distribute the values here.
   */
  if ( phase_seg1 > max_ph1_len ) {
    prop_seg += phase_seg1 - max_ph1_len;
    phase_seg1 = max_ph1_len;
    bt->prop_seg = prop_seg;
    bt->phase_seg1 = phase_seg1;
  }

  if ( nominal ) {
    btr = FIELD_PREP( REG_BTR_PROP, prop_seg );
    btr |= FIELD_PREP( REG_BTR_PH1, phase_seg1 );
    btr |= FIELD_PREP( REG_BTR_PH2, bt->phase_seg2 );
    btr |= FIELD_PREP( REG_BTR_BRP, bt->brp );
    btr |= FIELD_PREP( REG_BTR_SJW, bt->sjw );

    ctucanfd_write32( internal, CTUCANFD_BTR, btr );
  } else {
    btr = FIELD_PREP( REG_BTR_FD_PROP_FD, prop_seg );
    btr |= FIELD_PREP( REG_BTR_FD_PH1_FD, phase_seg1 );
    btr |= FIELD_PREP( REG_BTR_FD_PH2_FD, bt->phase_seg2 );
    btr |= FIELD_PREP( REG_BTR_FD_BRP_FD, bt->brp );
    btr |= FIELD_PREP( REG_BTR_FD_SJW_FD, bt->sjw );

    ctucanfd_write32( internal, CTUCANFD_BTR_FD, btr );
  }

  return 0;
}

/**
 * @brief  This function sets nominal bit timing routine
 *
 * @param  chip Pointer to chip structure.
 *
 * @return 0 for success, -EPERM on error
 */
static int ctucanfd_set_bittiming( struct rtems_can_chip *chip )
{
  struct rtems_can_bittiming *bt = &chip->bittiming;

  /* Note that bt may be modified here */
  return ctucanfd_set_btr( chip, bt, true );
}

/**
 * @brief  This function sets data bit timing routine
 *
 * @param  chip Pointer to chip structure.
 *
 * @return 0 for success, -EPERM on error
 */
static int ctucanfd_set_data_bittiming( struct rtems_can_chip *chip )
{
  struct rtems_can_bittiming *dbt = &chip->data_bittiming;

  /* Note that dbt may be modified here */
  return ctucanfd_set_btr( chip, dbt, false );
}

/**
 * @brief  This function sets secondary sample point in CTU CAN FD
 *
 * @param  chip Pointer to chip structure.
 *
 * @return 0 for success, -EPERM if controller is enabled
 */
static int ctucanfd_set_secondary_sample_point( struct rtems_can_chip *chip )
{
  struct ctucanfd_internal *internal = chip->internal;
  struct rtems_can_bittiming *dbt = &chip->data_bittiming;
  int ssp_offset = 0;
  uint32_t ssp_cfg = 0; /* No SSP by default */

  if ( CTU_CAN_FD_ENABLED( internal ) ) {
    return -EPERM;
  }

  /* Use SSP for bit-rates above 1 Mbits/s */
  if ( dbt->bitrate > 1000000 ) {
    /* Calculate SSP in minimal time quanta */
    ssp_offset = ( chip->freq / 1000 ) * dbt->sample_point / dbt->bitrate;

    if ( ssp_offset > 127 ) {
      ssp_offset = 127;
    }

    ssp_cfg = FIELD_PREP( REG_TRV_DELAY_SSP_OFFSET, ssp_offset );
    ssp_cfg |= FIELD_PREP( REG_TRV_DELAY_SSP_SRC, 0x1 );
  }

  ctucanfd_write32( internal, CTUCANFD_TRV_DELAY, ssp_cfg );

  return 0;
}

/**
 * @brief  This function sets CTU CAN FDs mode
 *
 * @param  internal Pointer to internal CTU CAN FD structure
 * @param  ctrlmode Mode set bitfield
 *
 * @return None
 */
static void ctucanfd_set_mode(
  struct ctucanfd_internal *internal,
  uint32_t ctrlmode
)
{
  uint32_t mode_reg = ctucanfd_read32( internal, CTUCANFD_MODE );

  mode_reg = ( ctrlmode & CAN_CTRLMODE_LOOPBACK ) ?
             ( mode_reg | REG_MODE_ILBP ) :
             ( mode_reg & ~REG_MODE_ILBP );

  mode_reg = ( ctrlmode & CAN_CTRLMODE_LISTENONLY ) ?
             ( mode_reg | REG_MODE_BMM ) :
             ( mode_reg & ~REG_MODE_BMM );

  mode_reg = ( ctrlmode & CAN_CTRLMODE_FD ) ?
             ( mode_reg | REG_MODE_FDE ) :
             ( mode_reg & ~REG_MODE_FDE );

  mode_reg = ( ctrlmode & CAN_CTRLMODE_PRESUME_ACK ) ?
             ( mode_reg | REG_MODE_STM ) :
             ( mode_reg & ~REG_MODE_STM );

  mode_reg = ( ctrlmode & CAN_CTRLMODE_FD_NON_ISO ) ?
             ( mode_reg | REG_MODE_NISOFD ) :
             ( mode_reg & ~REG_MODE_NISOFD );

  /* One shot mode supported indirectly via Retransmit limit */
  mode_reg &= ~FIELD_PREP( REG_MODE_RTRTH, 0xF );
  mode_reg = ( ctrlmode & CAN_CTRLMODE_ONE_SHOT ) ?
             ( mode_reg | REG_MODE_RTRLE ) :
             ( mode_reg & ~REG_MODE_RTRLE );

  /* Some bits fixed:
   *   TSTM  - Off, User shall not be able to change REC/TEC by hand during
   *   operation
   */
  mode_reg &= ~REG_MODE_TSTM;

  ctucanfd_write32( internal, CTUCANFD_MODE, mode_reg );
}

/**
 * @brief  This function gets status of TXT buffer
 *
 * @param  internal Pointer to internal CTU CAN FD structure
 * @param  buf  Buffer index (0-based)
 *
 * @return Status of TXT buffer
 */
static enum ctucanfd_txtb_status ctucanfd_get_tx_status(
  struct ctucanfd_internal *internal,
  uint8_t buf
)
{
  uint32_t tx_status = ctucanfd_read32( internal, CTUCANFD_TX_STATUS );
  enum ctucanfd_txtb_status status = ( tx_status >> ( buf * 4 ) ) & 0x7;

  return status;
}

/**
 * @brief  This function checks if frame can be inserted to TXT Buffer
 *
 * @param  internal Pointer to internal CTU CAN FD structure
 * @param  buf  Buffer index (0-based)
 *
 * @return True - Frame can be inserted to TXT Buffer, False - If attempted,
 *     frame will not be inserted to TXT Buffer
 */
static bool ctucanfd_is_txt_buf_writable(
  struct ctucanfd_internal *internal,
  uint8_t buf
)
{
  enum ctucanfd_txtb_status buf_status;

  buf_status = ctucanfd_get_tx_status( internal, buf );
  if (
    buf_status == TXT_RDY ||
    buf_status == TXT_TRAN ||
    buf_status == TXT_ABTP
  ) {
    return false;
  }

  return true;
}

/**
 * @brief  This function inserts frame to TXT buffer
 *
 * @param  internal Pointer to internal CTU CAN FD structure
 * @param  cf   Pointer to CAN frame.
 * @param  buf  TX buffer index (0-based)
 *
 * @return True - Frame inserted successfully
 *     False - Frame was not inserted due to one of:
 *      1. TXT Buffer is not writable (it is in wrong state)
 *      2. Invalid TXT buffer index
 *      3. Invalid frame length
 */
static bool ctucanfd_insert_frame(
	struct ctucanfd_internal *internal,
	const struct can_frame *cf,
	uint8_t buf
)
{
  uint32_t buf_base;
  uint32_t ffw = 0;
  uint32_t idw = 0;
  unsigned int i;

  if ( buf >= internal->ntxbufs ) {
    return false;
  }

  if ( !ctucanfd_is_txt_buf_writable( internal, buf ) ) {
    return false;
  }

  if ( cf->header.dlen > CAN_FRAME_MAX_DLEN ) {
    return false;
  }

  /* Prepare Frame format */
  if ( cf->header.flags & CAN_FRAME_RTR ) {
    ffw |= REG_FRAME_FORMAT_W_RTR;
  }

  if ( cf->header.flags & CAN_FRAME_IDE ) {
    ffw |= REG_FRAME_FORMAT_W_IDE;
  }

  if ( cf->header.flags & CAN_FRAME_FDF ) {
    ffw |= REG_FRAME_FORMAT_W_FDF;
    if ( cf->header.flags & CAN_FRAME_BRS ) {
      ffw |= REG_FRAME_FORMAT_W_BRS;
    }
  }

  ffw |= FIELD_PREP(
    REG_FRAME_FORMAT_W_DLC,
    rtems_canfd_len2dlc( cf->header.dlen )
  );

  /* Prepare identifier */
  if ( cf->header.flags & CAN_FRAME_IDE ) {
    idw = cf->header.can_id & CAN_FRAME_EFF_ID_MASK;
  } else {
    idw = FIELD_PREP(
      REG_IDENTIFIER_W_IDENTIFIER_BASE,
      cf->header.can_id & CAN_FRAME_BFF_ID_MASK
    );
  }

  /* Write ID, Frame format, Don't write timestamp -> Time triggered
   * transmission disabled
   */
  buf_base = ( buf + 1 ) * 0x100;
  ctucanfd_write_txt_buf( internal, buf_base, CTUCANFD_FRAME_FORMAT_W, ffw );
  ctucanfd_write_txt_buf( internal, buf_base, CTUCANFD_IDENTIFIER_W, idw );

  /* Write Data payload */
  if ( !( cf->header.flags & CAN_FRAME_RTR ) ) {
    for ( i = 0; i < cf->header.dlen; i += 4 ) {
      uint32_t data = *( uint32_t * )( cf->data + i );

      ctucanfd_write_txt_buf(
        internal,
        buf_base,
        CTUCANFD_DATA_1_4_W + i,
        data
      );
    }
  }

  return true;
}

/**
 * @brief  This function applies command on TXT buffer
 *
 * @param  internal Pointer to internal CTU CAN FD structure
 * @param  cmd  Command to give.
 * @param  buf  TX buffer index (0-based)
 *
 * @return None
 */
static void ctucanfd_give_txtb_cmd(
	struct ctucanfd_internal *internal,
	enum ctucanfd_txtb_command cmd,
	uint8_t buf
)
{
  uint32_t tx_cmd = cmd;

  tx_cmd |= 1 << ( buf + 8 );
  ctucanfd_write32( internal, CTUCANFD_TX_COMMAND, tx_cmd );
}

/**
 * @brief  This function reads frame from RX FIFO
 *
 * @param  chip Pointer to CAN chip structure
 * @param  cf   Pointer to CAN frame
 * @param  ffw  Previously read frame format word
 *
 * @return TX buffer number on success, negative value on error.
 */
static void ctucanfd_read_rx_frame(
  struct rtems_can_chip *chip,
  struct can_frame *frame,
  uint32_t ffw
)
{
  struct ctucanfd_internal *internal = chip->internal;
  struct rtems_can_queue_ends_dev *qends_dev = chip->qends_dev;
  struct rtems_can_queue_ends *qends = &qends_dev->base;
  uint32_t idw;
  unsigned int i;
  unsigned int wc;
  unsigned int len;

  frame->header.flags = 0;

  idw = ctucanfd_read32( internal, CTUCANFD_RX_DATA );
  if ( FIELD_GET( REG_FRAME_FORMAT_W_IDE, ffw ) ) {
    frame->header.can_id = idw & CAN_FRAME_EFF_ID_MASK;
    frame->header.flags |= CAN_FRAME_IDE;
  } else {
    frame->header.can_id = ( idw >> 18 ) & CAN_FRAME_BFF_ID_MASK;
  }

  /* BRS, ESI, RTR Flags */
  if ( FIELD_GET( REG_FRAME_FORMAT_W_FDF, ffw ) ) {
    frame->header.flags |= CAN_FRAME_FDF;
    if ( FIELD_GET( REG_FRAME_FORMAT_W_BRS, ffw ) ) {
      frame->header.flags |= CAN_FRAME_BRS;
    }
    if ( FIELD_GET( REG_FRAME_FORMAT_W_ESI_RSV, ffw ) ) {
      frame->header.flags |= CAN_FRAME_ESI;
    }
  } else if ( FIELD_GET( REG_FRAME_FORMAT_W_RTR, ffw ) ) {
    frame->header.flags |= CAN_FRAME_RTR;
  }

  wc = FIELD_GET( REG_FRAME_FORMAT_W_RWCNT, ffw ) - 3;

  /* DLC */
  if ( FIELD_GET( REG_FRAME_FORMAT_W_DLC, ffw ) <= 8 ) {
    len = FIELD_GET( REG_FRAME_FORMAT_W_DLC, ffw );
  } else {
    if ( FIELD_GET( REG_FRAME_FORMAT_W_FDF, ffw ) ) {
      len = wc << 2;
    } else {
      len = 8;
    }
  }
  frame->header.dlen = len;
  if ( len > wc * 4 ) {
    len = wc * 4;
  }

  /* Timestamp */
  frame->header.timestamp = ctucanfd_read32( internal, CTUCANFD_RX_DATA );
  frame->header.timestamp |= ( uint64_t )ctucanfd_read32(
                                           internal,
                                           CTUCANFD_RX_DATA
                                         ) << 32;

  /* Data */
  for ( i = 0; i < len; i += 4 ) {
    uint32_t data = ctucanfd_read32( internal, CTUCANFD_RX_DATA );
    *(uint32_t *)( frame->data + i ) = data;
  }
  while ( i < wc * 4 ) {
    ctucanfd_read32( internal, CTUCANFD_RX_DATA );
    i += 4;
  }

  /* Pass frame to edges */
  rtems_can_queue_filter_frame_to_edges( qends, NULL, frame, 0 );
}

/**
 * @brief  Called from CAN interrupt worker to complete the received
 *  frame processing
 *
 * @param  chip Pointer to CAN chip structure
 *
 * @return 1 when frame is received, -EAGAIN otherwise.
 */
static int ctucanfd_receive( struct rtems_can_chip *chip )
{
  struct ctucanfd_internal *internal = chip->internal;
  struct can_frame frame;
  uint32_t ffw;

  ffw = ctucanfd_read32( internal, CTUCANFD_RX_DATA );

  if ( !FIELD_GET( REG_FRAME_FORMAT_W_RWCNT, ffw ) ) {
    return -EAGAIN;
  }

  ctucanfd_read_rx_frame( chip, &frame, ffw );

  rtems_can_stats_add_rx_done( &chip->chip_stats );
  rtems_can_stats_add_rx_bytes( &chip->chip_stats, frame.header.dlen );

  return 1;
}

/**
 * @brief  This function reads CTU CAN FDs fault confinement state.
 *
 * @param  internal Pointer to internal CTU CAN FD structure
 *
 * @return Fault confinement state of controller
 */
static enum can_state ctucanfd_read_fault_state(
  struct ctucanfd_internal *internal
)
{
  uint32_t fs;
  uint32_t rec_tec;
  uint32_t ewl;

  fs = ctucanfd_read32( internal, CTUCANFD_EWL );
  rec_tec = ctucanfd_read32( internal, CTUCANFD_REC );
  ewl = FIELD_GET( REG_EWL_EW_LIMIT, fs );

  if ( FIELD_GET( REG_EWL_ERA, fs ) ) {
    if ( ewl > FIELD_GET( REG_REC_REC_VAL, rec_tec ) &&
         ewl > FIELD_GET( REG_REC_TEC_VAL, rec_tec ) ) {
      return CAN_STATE_ERROR_ACTIVE;
    } else {
      return CAN_STATE_ERROR_WARNING;
    }
  } else if ( FIELD_GET( REG_EWL_ERP, fs ) ) {
    return CAN_STATE_ERROR_PASSIVE;
  } else if ( FIELD_GET( REG_EWL_BOF, fs ) ) {
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
static void ctucanfd_err_interrupt( struct rtems_can_chip *chip, uint32_t isr )
{
  struct ctucanfd_internal *internal = chip->internal;
  struct can_frame err_frame = {};
  enum can_state state;
  uint32_t regval;
  uint16_t rxerr;
  uint16_t txerr;

  regval = ctucanfd_read32( internal, CTUCANFD_REC );

  rxerr = FIELD_GET( REG_REC_REC_VAL, regval );
  txerr = FIELD_GET( REG_REC_TEC_VAL, regval );

  state = ctucanfd_read_fault_state( internal );
  rtems_can_stats_set_state(&chip->chip_stats, state);

  if (
    FIELD_GET( REG_INT_STAT_FCSI, isr ) ||
    FIELD_GET( REG_INT_STAT_EWLI, isr )
  ) {
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
  if ( FIELD_GET( REG_INT_STAT_ALI, isr ) ) {
    err_frame.header.can_id |= CAN_ERR_ID_CRTL | CAN_ERR_ID_LOSTARB;
    err_frame.header.flags |= CAN_FRAME_ERR;
    err_frame.data[CAN_ERR_DATA_BYTE_TRX_LOSTARB] = CAN_ERR_LOSTARB_UNSPEC;
  }

  /* Check for Bus Error interrupt */
  if ( FIELD_GET( REG_INT_STAT_BEI, isr ) ) {
    rtems_can_stats_add_rx_error( &chip->chip_stats );
    err_frame.header.can_id |= CAN_ERR_ID_CRTL | CAN_ERR_ID_PROT | \
                               CAN_ERR_ID_BUSERROR;
    err_frame.header.flags |= CAN_FRAME_ERR;
    err_frame.data[CAN_ERR_DATA_BYTE_TRX_PROT] = CAN_ERR_PROT_UNSPEC;
    err_frame.data[CAN_ERR_DATA_BYTE_TRX_PROT_LOC] = CAN_ERR_PROT_LOC_UNSPEC;
  }

  /* Check for RX overflow interrupt */
  if ( FIELD_GET( REG_INT_STAT_DOI, isr ) ) {
    rtems_can_stats_add_rx_overflows( &chip->chip_stats );
    ctucanfd_write32( internal, CTUCANFD_TX_COMMAND, REG_COMMAND_CDO );
    err_frame.header.can_id |= CAN_ERR_ID_CRTL;
    err_frame.header.flags |= CAN_FRAME_ERR;
    err_frame.data[CAN_ERR_ID_LOSTARB] |= CAN_ERR_CRTL_RX_OVERFLOW;
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
 * @brief  This function adds message to TX buffer priority slot.
 *
 * @param  internal    Pointer to CTU CAN FD structure
 * @param  txtb_id ID to be moved
 * @param  prio    Message buffer priority
 *
 * @return None
 */
static void ctucanfd_txb_add(
  struct ctucanfd_internal *internal,
  unsigned int txb_order_idx,
  unsigned int prio
) {
  int i;
  internal->txb_order = ctucanfd_txb_slot_promote(
    internal->txb_order,
    txb_order_idx,
    internal->txb_prio_tail[prio]
  );
  for ( i = 0; i <= prio; i++ ) {
    internal->txb_prio_tail[i]++;
  }
}

/**
 * @brief  This function removes send message from TX buffer priority slot.
 *
 * @param  internal    Pointer to CTU CAN FD structure
 * @param  txtb_id ID to be removed
 *
 * @return None
 */
static void ctucanfd_txb_free(
  struct ctucanfd_internal *internal,
  unsigned int txb_order_idx
) {
  int i = RTEMS_CAN_QUEUE_PRIO_NR - 1;
  internal->txb_order = ctucanfd_txb_slot_demote(
    internal->txb_order,
    txb_order_idx,
    internal->ntxbufs - 1
  );
  do {
    if ( internal->txb_prio_tail[i] > txb_order_idx ) {
      internal->txb_prio_tail[i]--;
    }
  } while( i-- );
}

/**
 * @brief  This function handles TX buffer done interrupt.
 *
 * @param  internal       Pointer to CTU CAN FD structure
 * @param  txtb_id        ID of the buffer
 * @param  txb_order_idx  Index of the buffer in transmission order
 * @param  txtb_st        Status of the buffer
 *
 * @return None
 */
static void ctucanfd_handle_txtb_done(
  struct rtems_can_chip *chip,
  unsigned int txtb_id,
  unsigned int txb_order_idx,
  unsigned int txtb_st
)
{
  struct ctucanfd_internal *internal;
  struct ctucanfd_txb_info *txb_info;

  internal = (struct ctucanfd_internal *) chip->internal;
  txb_info = &internal->txb_info[txtb_id];
  ctucanfd_give_txtb_cmd( internal, TXT_CMD_SET_EMPTY, txtb_id );
  switch ( txtb_st ) {
    case TXT_TOK:
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
      ctucanfd_txb_free( internal, txb_order_idx );
      break;
    case TXT_ABT:
      /* Reschedule aborted frames only if the chip is running, otherwise
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
        txb_info->slot = NULL;
        txb_info->edge = NULL;
        ctucanfd_txb_free( internal, txb_order_idx );
        break;
      }
    case TXT_ERR:
  #ifdef CTUCANFD_DEBUG
        printk("CTUCANFD TXT_ERR\n");
  #endif /*CTUCANFD_DEBUG*/
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
      txb_info->slot = NULL;
      txb_info->edge = NULL;
      ctucanfd_txb_free( internal, txb_order_idx );
      rtems_can_stats_add_tx_error( &chip->chip_stats );
      break;
    default:
  #ifdef CTUCANFD_DEBUG
        printk("CTUCANFD TXT state unknown\n");
  #endif /*CTUCANFD_DEBUG*/
      rtems_can_queue_free_outslot(
        &chip->qends_dev->base,
        txb_info->edge,
        txb_info->slot
      );
      txb_info->slot = NULL;
      txb_info->edge = NULL;
      ctucanfd_txb_free( internal, txb_order_idx );
      break;
  }
}

/**
 * @brief  This function handles TX interrupts
 *
 * TX done, TX abort and TX error interrupts are handled in this function.
 * It takes care of freeing HW buffers on succesfull transfer or of
 * rescheduling messages in case of abort/error.
 *
 * @param  chip Pointer to CAN chip structure
 *
 * @return None
 */
static void ctucanfd_tx_interrupt( struct rtems_can_chip *chip )
{
  struct ctucanfd_internal *internal =
    ( struct ctucanfd_internal * )chip->internal;

  while ( 1 ) {
    uint32_t tx_status;
    unsigned int txb_order_idx = 0;
    unsigned int txtb_id;
    unsigned int txtb_st;
    ctucanfd_write32( internal, CTUCANFD_INT_STAT, REG_INT_STAT_TXBHCI );
    tx_status = ctucanfd_read32( internal, CTUCANFD_TX_STATUS );
    if ( ( tx_status & TXT_ANY_DONE ) == 0 )
      break;

    txtb_id = ctucanfd_txb_from_order( internal->txb_order, txb_order_idx );
    txtb_st = TXTB_GET_STATUS( tx_status, txtb_id );
    if ( ( txtb_st & TXT_DONE ) == 0 ) {
      for (
        txb_order_idx = 1;
        txb_order_idx < RTEMS_CTUCANFD_NTXBUFS_MAX;
        txb_order_idx++
      ) {
        txtb_id = ctucanfd_txb_from_order(
          internal->txb_order,
          txb_order_idx
        );
        txtb_st = TXTB_GET_STATUS( tx_status, txtb_id );
        if ( ( txtb_st & TXT_DONE ) != 0 )
          break;
      }

      if ( txb_order_idx >= internal->ntxbufs ) {
        ctucanfd_give_txtb_cmd( internal, TXT_CMD_SET_EMPTY, txtb_id );
        continue;
      }
    }

    ctucanfd_check_state( internal, "before Tx done processed" );
    ctucanfd_handle_txtb_done( chip, txtb_id, txb_order_idx, txtb_st );
    ctucanfd_check_state( internal, "after Tx done processed" );
  }
}

/**
 * @brief  This function handles CAN interrupts.
 *
 * @param  arg Argument passed throught interrupt routine.
 *
 * @return None
 */
static void ctucanfd_interrupt( void *arg )
{
  struct rtems_can_chip *chip = (struct rtems_can_chip *)arg;
  struct ctucanfd_internal *internal = chip->internal;

  /* Just disable interrupts and start worker thread. */

  ctucanfd_write32( internal, CTUCANFD_INT_ENA_CLR, 0xffff );

  rtems_binary_semaphore_post( &chip->qends_dev->worker_sem );

  return;
}

/**
 * @brief  This is the part of the worker function that processes interrupts.
 *
 * This is implemented as a while loop that goes on until there are no
 * interrupts to be read.
 *
 * @param  rtems_can_chip      Pointer to chip structure.
 * @param  abort_recheck This indicates TX part of the worker requested abort.
 *
 * @return None
 */
static inline void ctucanfd_process_interrupts(
  struct rtems_can_chip *chip,
  int *abort_recheck
)
{
  struct ctucanfd_internal *internal =
    (struct ctucanfd_internal *)chip->internal;
#ifdef CTUCANFD_DEBUG
  int loop_counter = 0;
#endif /* CTUCANFD_DEBUG*/
  uint32_t isr;
  uint32_t icr;

  while ( 1 ) {
    ctucanfd_check_state( internal, "in worker loop start" );
#ifdef CTUCANFD_DEBUG
    if ( ++loop_counter > 100 ) {
      printk("CTUCANFD CHECK detected too much active loops\n");
    }
#endif /*CTUCANFD_DEBUG*/

    /* Get interrupt status register */
    isr = ctucanfd_read32( internal, CTUCANFD_INT_STAT );
    if ( ( isr == 0 ) && ( *abort_recheck == 0 ) ) {
      /* Leave the loop if no interrupt is set */
      break;
    }

    if ( FIELD_GET( REG_INT_STAT_RBNEI, isr ) ) {
      /* RX available interrupt, receive new message */
      ctucanfd_write32( internal, CTUCANFD_INT_MASK_SET, REG_INT_STAT_RBNEI );
      ctucanfd_write32( internal, CTUCANFD_INT_STAT, REG_INT_STAT_RBNEI );
      ctucanfd_receive( chip );
      ctucanfd_write32( internal, CTUCANFD_INT_STAT, REG_INT_STAT_RBNEI );
    }

    if ( FIELD_GET( REG_INT_STAT_TXBHCI, isr ) || ( *abort_recheck == 1 ) ) {
      /* TX interrupt */
      *abort_recheck = 0;
      ctucanfd_tx_interrupt( chip );
    }

    if (
      FIELD_GET( REG_INT_STAT_BEI, isr ) ||
      FIELD_GET( REG_INT_STAT_EWLI, isr ) ||
      FIELD_GET( REG_INT_STAT_FCSI, isr ) ||
      FIELD_GET( REG_INT_STAT_ALI, isr ) ||
      FIELD_GET( REG_INT_STAT_DOI, isr )
    ) {
      /* Error interrupts */
      icr = isr & (
        REG_INT_STAT_EWLI |
        REG_INT_STAT_FCSI |
        REG_INT_STAT_ALI |
        REG_INT_STAT_BEI |
        REG_INT_STAT_DOI
      );

      ctucanfd_write32( internal, CTUCANFD_INT_STAT, icr );
      ctucanfd_err_interrupt( chip, isr );
    }
  }
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
static rtems_task ctucanfd_worker( rtems_task_argument arg )
{
  struct rtems_can_chip *chip = (struct rtems_can_chip *)arg;
  struct ctucanfd_internal *internal =
    (struct ctucanfd_internal *)chip->internal;
  struct ctucanfd_txb_info *txb_info;
  struct rtems_can_queue_ends_dev *qends_dev = chip->qends_dev;
  struct rtems_can_queue_ends *qends = &qends_dev->base;
  struct rtems_can_queue_edge *qedge;
  struct rtems_can_queue_slot *slot;
  unsigned int txtb_id;
  int abort_recheck = 0;
  int ret;

  while ( 1 ) {
    /* First process all received interrupts. This is implemented as a
     * while loop that goes on until there are no interrupts to be read.
     */
    ctucanfd_process_interrupts( chip, &abort_recheck );

    if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 0 ) {
      /* Abort all filled HW buffers. */
      for ( int i = 0; i < internal->txb_prio_tail[0]; i++ ) {
        txtb_id = ctucanfd_txb_from_order( internal->txb_order, i );

        if ( ctucanfd_get_tx_status( internal, txtb_id ) == TXT_ETY ) {
          txb_info = &internal->txb_info[txtb_id];
          if ( txb_info != NULL ) {
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
            txb_info->slot = NULL;
            txb_info->edge = NULL;
            ctucanfd_txb_free( internal, i );
          }
        } else {
          ctucanfd_give_txtb_cmd( internal, TXT_CMD_SET_ABORT, txtb_id );
          abort_recheck = 1;
        }
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

      if ( internal->txb_prio_tail[0] == 0 ) {
        /* Notify the stop function all frames were aborted/sent back */
        rtems_binary_semaphore_post( &chip->stop_sem );
      }
    } else if ( internal->txb_prio_tail[0] < internal->ntxbufs ) {
      /* We have some space in HW buffers for outgoing messages,
       * chek whether there is something to send.
       */
      ret = rtems_can_queue_test_outslot( qends, &qedge, &slot );
      if ( ret >= 0 ) {
        unsigned int txb_order_idx = internal->txb_prio_tail[0];
        unsigned int txtb_id = ctucanfd_txb_from_order (
          internal->txb_order,
          txb_order_idx
        );
        ctucanfd_check_state( internal, "before insert_frame" );
        /* Insert frame to HW buffer */
        bool ok = ctucanfd_insert_frame( internal, &slot->frame, txtb_id );
        if ( ok == true ) {
          /* Frame inserted succesfully, update TX buffer representation,
           * buffer priorities and set buffer as ready.
           */
          txb_info = &internal->txb_info[txtb_id];
          txb_info->edge = qedge;
          txb_info->slot = slot;
          ctucanfd_txb_add( internal, txb_order_idx, qedge->edge_prio );
          ctucanfd_write32(
            internal,
            CTUCANFD_TX_PRIORITY,
            ctucanfd_txb_order2prio( internal->txb_order )
          );
          ctucanfd_give_txtb_cmd( internal, TXT_CMD_SET_READY, txtb_id );
          ctucanfd_give_txtb_cmd( internal, TXT_CMD_SET_READY, txtb_id );
          ctucanfd_check_state( internal, "after insert_frame succeed" );
          continue;
        } else {
          /* Insert failed, schedule frame for later processing */
          ctucanfd_check_state( internal, "before insert_frame failed" );
          rtems_can_queue_push_back_outslot( qends, qedge, slot );
        }
      }
    } else {
      /* There is no free space in HW buffers. Check whether pending
       * message has higher priority class then some message in HW buffers.
       */
      int pending_prio = -1;
      int avail_prio;
      for (
        avail_prio = 1;
        avail_prio < RTEMS_CAN_QUEUE_PRIO_NR;
        avail_prio++
      ) {
        if ( internal->txb_prio_tail[avail_prio] < internal->ntxbufs ) {
          pending_prio = rtems_can_queue_pending_outslot_prio(
            qends,
            avail_prio
          );
          break;
        }
      }
      if ( pending_prio >= avail_prio ) {
        /* There is a pending message with higher priority class. Abort
         * the lowest priority message and discard it from HW buffer. High
         * priority message will be inserted into this buffer in next loop.
         */
        unsigned int txtb_id;
        txtb_id = ctucanfd_txb_from_order(
          internal->txb_order,
          internal->ntxbufs - 1
        );
        ctucanfd_give_txtb_cmd( internal, TXT_CMD_SET_ABORT, txtb_id );
        abort_recheck = 1;
        continue;
      }
    }

    /* Re-enable the interrupts */
    ctucanfd_write32( internal, CTUCANFD_INT_ENA_SET, internal->int_ena );
    ctucanfd_write32( internal, CTUCANFD_INT_MASK_CLR, internal->int_ena );

    ctucanfd_check_state( internal, "in worker before wait" );

    rtems_binary_semaphore_wait( &qends_dev->worker_sem );
  }
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
static int ctucanfd_chip_ioctl(
  struct rtems_can_chip *chip,
  ioctl_command_t command,
  void *arg
)
{
  int ret;

  switch( command ) {
    default:
      ret = -EINVAL;
      break;
  }

  return ret;
}

/**
 * @brief  This function calculates and sets bittiming from giver bitrate
 *
 * @param  chip  Pointer to chip structure.
 * @param  type  Bittiming type (nominal, FD)
 * @param  bt    Pointer to rtems_can_bittiming structure
 *
 * @return 0 on success, negated errno on error.
 * @retval -EPERM  Chip is already started, cannot change bittiming.
 * @retval -EINVAL Incorrect bit time type.
 */
static int ctucanfd_calc_and_set_bittiming(
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

  if ( type == RTEMS_CAN_BITTIME_NOMINAL ) {
    btc = &ctucanfd_nominal_bittiming_const;
    chip_bt = &chip->bittiming;
  } else if ( type == RTEMS_CAN_BITTIME_DATA ) {
    btc = &ctucanfd_data_bittiming_const;
    chip_bt = &chip->data_bittiming;
  } else {
    return -EINVAL;
  }

  ret = rtems_can_bitrate2bittiming( chip, bt, btc );
  if ( ret < 0 ) {
    return ret;
  }

  *chip_bt = *bt;

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
static int ctucanfd_check_and_set_bittiming(
  struct rtems_can_chip *chip,
  int type,
  struct rtems_can_bittiming *bt
)
{
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) ) {
    /* Chip was already started, cannot change bitrate */
    return -EPERM;
  }

  // TODO: check whether inserted rtems_can_bittiming is within limits

  if ( type == RTEMS_CAN_BITTIME_NOMINAL ) {
    chip->bittiming = *bt;
    ctucanfd_set_bittiming( chip );
  } else if ( type == RTEMS_CAN_BITTIME_DATA ) {
    chip->data_bittiming = *bt;
    ctucanfd_set_data_bittiming( chip );
  } else {
    return -EINVAL;
  }

  return 0;
}

/**
 * @brief  This function stops the chip.
 *
 * @param  chip  Pointer to chip structure.
 *
 * @return 0 on success. -ETIME in case of timeout or -EAGAIN if no timeout
 *         is set.
 */
static int ctucanfd_stop_chip(
  struct rtems_can_chip *chip,
  struct timespec *ts
)
{
  struct ctucanfd_internal *internal = chip->internal;
  rtems_interval timeout;
  struct timespec curr;
  struct timespec final;
  struct timespec towait;
  uint32_t mode;
  uint32_t mask = 0xffff;
  int ret = -1;

  rtems_mutex_lock( &chip->lock );
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 0 ) {
    /* Already stopped, return. */
    rtems_mutex_unlock( &chip->lock );
    return 0;
  }

  rtems_can_clear_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags );
  internal->int_ena &= REG_INT_STAT_TXBHCI;
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
    ret = rtems_binary_semaphore_try_wait( &chip->stop_sem );
  }

  if (ret < 0) {
    /* The controller didn't managed to clear all frames before timeout,
     * flush the buffers.
     */
    rtems_can_queue_ends_flush_outlist( &chip->qends_dev->base );
  }

  /* Disable interrupts and disable the controller */
  internal->int_ena = 0;
  ctucanfd_write32( internal, CTUCANFD_INT_ENA_CLR, mask );
  ctucanfd_write32( internal, CTUCANFD_INT_MASK_SET, mask );
  mode = ctucanfd_read32( internal, CTUCANFD_MODE );
  mode &= ~REG_MODE_ENA;
  rtems_binary_semaphore_post( &chip->qends_dev->worker_sem );
  ctucanfd_write32( internal, CTUCANFD_MODE, mode );
  rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_STOPPED );
  rtems_can_bus_notify_chip_stop( chip );
  rtems_mutex_unlock( &chip->lock );

  return ret;
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
static int ctucanfd_start_chip( struct rtems_can_chip *chip )
{
  struct ctucanfd_internal *internal = chip->internal;
  uint32_t regval;
  int ret = 0;

  rtems_mutex_lock( &chip->lock );
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) ) {
    /* Chip was already configured, skip */
    rtems_mutex_unlock( &chip->lock );
    return 0;
  }

  internal->int_ena  = 0;
  internal->int_mask = 0;

  ctucanfd_write32( internal, CTUCANFD_TX_PRIORITY, 0x01234567 );

  /* Set timestamp at the Start of Frame bit */
  ctucanfd_write32( internal, CTUCANFD_RX_STATUS, REG_RX_STATUS_RTSOP);

  /* Configure bit-rates and ssp */
  ret = ctucanfd_set_bittiming( chip );
  if ( ret < 0 )
    goto start_chip_unlock;

  ret = ctucanfd_set_data_bittiming( chip );
  if ( ret < 0 )
    goto start_chip_unlock;

  ret = ctucanfd_set_secondary_sample_point( chip );
  if ( ret < 0 )
    goto start_chip_unlock;

  /* Configure modes */
  ctucanfd_set_mode( internal, chip->ctrlmode );

  /* Configure interrupts */
  internal->int_ena = REG_INT_STAT_RBNEI |
                  REG_INT_STAT_TXBHCI |
                  REG_INT_STAT_EWLI |
                  REG_INT_STAT_FCSI |
                  REG_INT_STAT_DOI;

  /* Bus error reporting -> Allow Error/Arb.lost interrupts */
  if ( chip->ctrlmode & CAN_CTRLMODE_BERR_REPORTING )
    internal->int_ena |= REG_INT_STAT_ALI | REG_INT_STAT_BEI;

  internal->int_mask = ~internal->int_ena;

  ctucanfd_write32( internal, CTUCANFD_INT_MASK_CLR, internal->int_ena );
  ctucanfd_write32( internal, CTUCANFD_INT_MASK_SET, internal->int_mask );
  ctucanfd_write32( internal, CTUCANFD_INT_ENA_SET, internal->int_ena );

  /* Enable the controller */
  regval = ctucanfd_read32( internal, CTUCANFD_MODE );
  regval |= REG_MODE_ENA;
  ctucanfd_write32( internal, CTUCANFD_MODE, regval );

  /* Mark HW as configured */
  rtems_binary_semaphore_try_wait( &chip->stop_sem );
  rtems_can_set_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags );
  rtems_can_stats_reset( &chip->chip_stats );
  rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_ERROR_ACTIVE );

start_chip_unlock:
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
static void ctucanfd_register( struct rtems_can_chip_ops *chip_ops )
{
  chip_ops->start_chip = ctucanfd_start_chip;
  chip_ops->stop_chip = ctucanfd_stop_chip;
  chip_ops->chip_ioctl = ctucanfd_chip_ioctl;
  chip_ops->get_chip_timestamp = ctucanfd_get_timestamp;
  chip_ops->get_chip_info = ctucanfd_get_chip_info;
  chip_ops->calc_bittiming = ctucanfd_calc_and_set_bittiming;
  chip_ops->check_and_set_bittiming = ctucanfd_check_and_set_bittiming;
}

struct rtems_can_chip *rtems_ctucanfd_initialize(
  uintptr_t addr,
  rtems_vector_number irq,
  rtems_task_priority worker_priority,
  int ntxbufs,
  rtems_option irq_option,
  unsigned long can_clk_rate
)
{
  rtems_id worker_task_id;
  rtems_status_code sc;
  struct rtems_can_chip *chip;
  struct ctucanfd_internal *internal;
  uint32_t txb_count;
  int ret;

  if ( ntxbufs < 0 ) {
    return NULL;
  }

  internal = calloc( 1, sizeof( struct ctucanfd_internal ) );
  if ( internal == NULL ) {
    return NULL;
  }

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

  chip->type = CAN_CTUCANFD_TYPE;
  chip->irq = irq;
  chip->freq = can_clk_rate;
  chip->ctrlmode = CAN_CTRLMODE_FD;
  chip->ctrlmode_supported = CAN_CTRLMODE_LOOPBACK |
                             CAN_CTRLMODE_LISTENONLY |
                             CAN_CTRLMODE_FD |
                             CAN_CTRLMODE_PRESUME_ACK |
                             CAN_CTRLMODE_BERR_REPORTING |
                             CAN_CTRLMODE_FD_NON_ISO |
                             CAN_CTRLMODE_ONE_SHOT;
  chip->bittiming_const = &ctucanfd_nominal_bittiming_const;
  chip->data_bittiming_const = &ctucanfd_data_bittiming_const;
  chip->capabilities = RTEMS_CAN_CHIP_CAPABILITIES_FD;
  rtems_mutex_init( &chip->lock, "ctucanfd_lock" );

  ctucanfd_register( &chip->chip_ops );

  internal->txb_order = CTUCANFD_INIT_TXB_ORDER;
  internal->base = addr;

  if (
    ( ctucanfd_read32( internal, CTUCANFD_DEVICE_ID ) & CTUCANFD_ID_MASK ) !=
    CTUCANFD_ID
  ) {
    free( internal );
    free( chip->qends_dev );
    free( chip );
    return NULL;
  }

  txb_count = FIELD_GET(
    REG_TX_COMMAND_TXT_BUFFER_COUNT,
    ctucanfd_read32( internal, CTUCANFD_TX_COMMAND )
  );

  if ( txb_count == 0 ) {
    /* Ensure the compatibility with older IP core versions before synthesis
     * parameter for TX buffer count was been added.
     */
    txb_count = 4;
  }

  if ( ntxbufs > txb_count ) {
    /* Limit the number of HW buffers if bigger than supported in HW. */
    ntxbufs = txb_count;
  }

  if ( ntxbufs > RTEMS_CTUCANFD_NTXBUFS_MAX ) {
    /* Limit the number of HW buffers if bigger than driver is compiled to. */
    ntxbufs = RTEMS_CTUCANFD_NTXBUFS_MAX;
  }

  internal->ntxbufs = ntxbufs;
  chip->internal = internal;

  ret = ctucanfd_reset( internal );
  if ( ret < 0 ) {
    free( internal );
    free( chip->qends_dev );
    free( chip );
    return NULL;
  }

  rtems_binary_semaphore_init(
    &chip->stop_sem,
    "can_ctucanfd_stop_sem"
  );

  rtems_can_queue_ends_init_chip(
    chip,
    "can_ctucanfd_worker"
  );

  sc = rtems_interrupt_handler_install(
    chip->irq,
    "ctucanfd",
    irq_option,
    ctucanfd_interrupt,
    chip
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    free( internal );
    free( chip->qends_dev );
    free( chip );
    return NULL;
  }

  sc = rtems_task_create(
    rtems_build_name( 'C', 'A', 'N', 'C' ),
    worker_priority,
    RTEMS_MINIMUM_STACK_SIZE + 0x1000,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &worker_task_id
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    free( internal );
    rtems_interrupt_handler_remove(
      chip->irq,
      ctucanfd_interrupt,
      chip
    );
    free( chip->qends_dev );
    free( chip );
    return NULL;
  }

  rtems_task_start( worker_task_id, ctucanfd_worker,
                    (rtems_task_argument)chip );

  rtems_can_set_bit( RTEMS_CAN_CHIP_CONFIGURED, &chip->flags );
  rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_STOPPED );

  return chip;
}
