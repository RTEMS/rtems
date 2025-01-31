/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This header file is part of CAN/CAN FD bus common support. It
 *  implements CAN frame structure and related defines.
 *
 * Implementation is based on original LinCAN - Linux CAN bus driver
 * Part of OrtCAN project https://ortcan.sourceforge.net/
 */

/*
 * Copyright (C) 2023-2024 Michal Lenc <michallenc@seznam.cz>
 * Copyright (C) 2002-2009 DCE FEE CTU Prague
 * Copyright (C) 2002-2024 Pavel Pisa <pisa@cmp.felk.cvut.cz>
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

#ifndef _DEV_CAN_CAN_FRAME_H
#define _DEV_CAN_CAN_FRAME_H

/**
 * @defgroup CANFrame
 *
 * @ingroup CANFDStack
 *
 * @{
 */

/**
 * @brief This define provides mask for base frame format identifier.
 */
#define CAN_FRAME_BFF_ID_MASK ( 0x000007ff )
/**
 * @brief This define provides mask for extended frame format identifier.
 */
#define CAN_FRAME_EFF_ID_MASK ( 0x1fffffff )
/**
 * @brief Represents standard (not FD) CAN frame data length.
 */
#define CAN_FRAME_STANDARD_DLEN ( 8u )
/**
 * @brief Represents CAN FD frame data length.
 */
#define CAN_FRAME_FD_DLEN       ( 64u )
/**
 * @brief Represents maximum allowed CAN frame data length.
 */
#define CAN_FRAME_MAX_DLEN      ( CAN_FRAME_FD_DLEN )

/**
 * @defgroup CANFrameFlags
 *
 * @ingroup CANFrame
 *
 * @{
 */

/**
 * @name CAN Frame Flags
 *
 * See structure @ref can_frame for reference.
 *
 * @{
 */

/**
 * @brief Represents extended frame format.
 */
#define CAN_FRAME_IDE   ( 1 << 0 )
/**
 * @brief Represents remote transfer request.
 */
#define CAN_FRAME_RTR   ( 1 << 1 )
/**
 * @brief Represents echo flag.
 */
#define CAN_FRAME_ECHO  ( 1 << 2 )
/**
 * @brief Represents local flag.
 */
#define CAN_FRAME_LOCAL ( 1 << 3 )
/**
 * @brief Represents frame transmission error.
 */
#define CAN_FRAME_TXERR ( 1 << 4 )
/**
 * @brief Represents generic error flag.
 */
#define CAN_FRAME_ERR   ( 1 << 5 )
/**
 * @brief Represents local FIFO overflow.
 */
#define CAN_FRAME_FIFO_OVERFLOW ( 1 << 6 )
/**
 * @brief Represents CAN FD frame format.
 */
#define CAN_FRAME_FDF   ( 1 << 7 )
/**
 * @brief Represents bit rate shift for CAN FD frames.
 */
#define CAN_FRAME_BRS   ( 1 << 8 )
/**
 * @brief Represents error state inicator of transmitting node.
 */
#define CAN_FRAME_ESI   ( 1 << 9 )

/** @} */
/** @} */

/**
 * @defgroup CANFrameErrors
 *
 * @ingroup CANFrame
 *
 * @{
 */

/**
 * @name CAN Error Types
 *
 * These types are defined in identifier field of @ref can_frame structure.
 *
 * @{
 */

/**
 * @brief Represents CAN frame TX timeout error.
 */
#define CAN_ERR_ID_TXTIMEOUT    ( 1 << 0 )
/**
 * @brief Represents lost arbitration error. Additional information
 *  are stored in @ref CAN_ERR_DATA_BYTE_TRX_CTRL of data.
 */
#define CAN_ERR_ID_LOSTARB      ( 1 << 1 )
/**
 * @brief Represents CAN controller error. Additional information
 *  are stored in @ref CAN_ERR_DATA_BYTE_TRX_PROT of data.
 */
#define CAN_ERR_ID_CRTL         ( 1 << 2 )
/**
 * @brief Represents CAN frame protocol violations. Additional information
 *  are stored in @ref CAN_ERR_DATA_BYTE_TRX_PROT and
 *  @ref CAN_ERR_DATA_BYTE_TRX_PROT_LOC of data.
 */
#define CAN_ERR_ID_PROT         ( 1 << 3 )
/**
 * @brief Represents transceiver status error. Additional information
 *  are stored in @ref CAN_ERR_TRX_DATA_BYTE of data.
 */
#define CAN_ERR_ID_TRX          ( 1 << 4 )
/**
 * @brief Represents no acknowledgment on transmission error.
 */
#define CAN_ERR_ID_ACK          ( 1 << 5 )
/**
 * @brief Represents bus off state.
 */
#define CAN_ERR_ID_BUSOFF       ( 1 << 6 )
/**
 * @brief Represents bus error.
 */
#define CAN_ERR_ID_BUSERROR     ( 1 << 7 )
/**
 * @brief Represents controller restarted information.
 */
#define CAN_ERR_ID_RESTARTED    ( 1 << 8 )
/**
 * @brief Represents TX/RX error counter. Values are stored in
 *  @ref CAN_ERR_DATA_BYTE_CNT_TX and @ref CAN_ERR_DATA_BYTE_CNT_RX of data.
 */
#define CAN_ERR_ID_CNT          ( 1 << 9 )
/**
 * @brief Represents stack internal error.
 */
#define CAN_ERR_ID_INTERNAL     ( 1 << 10 )
/**
 * @brief Represents CAN error tag. This sets 31st bit of identifier to
 *  logical one to make the frame invalid. It should be used to further
 *  distinquish error frames from standard ones.
 */
#define CAN_ERR_ID_TAG          ( 1 << 31 )

/** @} */

/**
 * @name CAN Error Types Offsets to Data
 *
 * These defines offsets to data field of @ref can_frame structure, where
 * additional information for given error type are stored.
 *
 * @{
 */

/**
 * @brief Represents data offset in bytes to lost arbitration information.
 */
#define CAN_ERR_DATA_BYTE_TRX_LOSTARB  ( 0 )
/**
 * @brief Represents data offset in bytes to controller error information.
 */
#define CAN_ERR_DATA_BYTE_TRX_CTRL     ( 1 )
/**
 * @brief Represents data offset in bytes to information about protocol
 *  violation.
 */
#define CAN_ERR_DATA_BYTE_TRX_PROT     ( 2 )
/**
 * @brief Represents data offset in bytes to information about protocol
 *  violation location.
 */
#define CAN_ERR_DATA_BYTE_TRX_PROT_LOC ( 3 )
/**
 * @brief Represents data offset in bytes to additional transceiver status
 *  error information.
 */
#define CAN_ERR_TRX_DATA_BYTE      ( 4 )
/**
 * @brief Represents data offset in bytes to TX counter.
 */
#define CAN_ERR_DATA_BYTE_CNT_TX   ( 6 )
/**
 * @brief Represents data offset in bytes to RX counter.
 */
#define CAN_ERR_DATA_BYTE_CNT_RX   ( 7 )

/** @} */

/**
 * @name CAN Lost Arbitration Errors
 *
 * @{
 */

/**
 * @brief Represents unspecified lost arbitration error.
 */
#define CAN_ERR_LOSTARB_UNSPEC   ( 0 )
/**
 * @brief Represents bit in which arbitration was lost.
 */
#define CAN_ERR_LOSTARB_BIT( n ) ( n )

/** @} */

/**
 * @name CAN Controller Errors
 *
 * @{
 */

/**
 * @brief Represents unspecified controller error.
 */
#define CAN_ERR_CRTL_UNSPEC      ( 0 )
/**
 * @brief Represents RX buffer overflow controller error.
 */
#define CAN_ERR_CRTL_RX_OVERFLOW ( 1 << 0 )
/**
 * @brief Represents TX buffer overflow controller error.
 */
#define CAN_ERR_CRTL_TX_OVERFLOW ( 1 << 1 )
/**
 * @brief Identifies controller reached warning level for RX errors.
 */
#define CAN_ERR_CRTL_RX_WARNING  ( 1 << 2 )
/**
 * @brief Identifies controller reached warning level for TX errors.
 */
#define CAN_ERR_CRTL_TX_WARNING  ( 1 << 3 )
/**
 * @brief Identifies controller reached error passive status for RX.
 */
#define CAN_ERR_CRTL_RX_PASSIVE  ( 1 << 4 )
/**
 * @brief Identifies controller reached error passive status for TX.
 */
#define CAN_ERR_CRTL_TX_PASSIVE  ( 1 << 5 )
/**
 * @brief Identifies controller recovered to error active state.
 */
#define CAN_ERR_CRTL_ACTIVE      ( 1 << 6 )

/** @} */

/**
 * @name CAN Protocol Violation Errors
 *
 * @{
 */

/**
 * @brief Represents unspecified protocol violation.
 */
#define CAN_ERR_PROT_UNSPEC      ( 0 )
/**
 * @brief Represents single bit error.
 */
#define CAN_ERR_PROT_BIT         (1 << 0)
/**
 * @brief Represents frame format error.
 */
#define CAN_ERR_PROT_FOR         (1 << 1)
/**
 * @brief Represents bit stuffing error.
 */
#define CAN_ERR_PROT_STUFF       (1 << 2)
/**
 * @brief Identifies the controller is unable to send dominant bit.
 */
#define CAN_ERR_PROT_BIT0        (1 << 3)
/**
 * @brief Identifies the controller is unable to send recessive bit.
 */
#define CAN_ERR_PROT_BIT1        (1 << 4)
/**
 * @brief Represents bus overload.
 */
#define CAN_ERR_PROT_OVERLOAD    (1 << 5)
/**
 * @brief Represents active error announcement.
 */
#define CAN_ERR_PROT_ACTIVE      (1 << 6)
/**
 * @brief Identifies that rrror occurred on transmission.
 */
#define CAN_ERR_PROT_TX          (1 << 7)

/** @} */

/**
 * @name CAN Protocol Violation Errors Location
 *
 * @{
 */

/**
 * @brief Represents unspecified protocol violation location.
 */
#define CAN_ERR_PROT_LOC_UNSPEC  ( 0x00 )
/**
 * @brief Represents protocol violation location at start of frame.
 */
#define CAN_ERR_PROT_LOC_SOF     ( 0x01 )
/**
 * @brief Represents protocol violation location at ID bits 0-4.
 */
#define CAN_ERR_PROT_LOC_ID0     ( 0x02 )
/**
 * @brief Represents protocol violation location at ID bits 5-12.
 */
#define CAN_ERR_PROT_LOC_ID1     ( 0x03 )
/**
 * @brief Represents protocol violation location at ID bits 13-17.
 */
#define CAN_ERR_PROT_LOC_ID2     ( 0x04 )
/**
 * @brief Represents protocol violation location at ID bits 21-28.
 */
#define CAN_ERR_PROT_LOC_ID3     ( 0x05 )
/**
 * @brief Represents protocol violation location at ID bits 18-20.
 */
#define CAN_ERR_PROT_LOC_ID4     ( 0x06 )
/**
 * @brief Represents protocol violation location at IDE bit.
 */
#define CAN_ERR_PROT_LOC_IDE     ( 0x07 )
/**
 * @brief Represents protocol violation location at RTR bit.
 */
#define CAN_ERR_PROT_LOC_RTR     ( 0x08 )
/**
 * @brief Represents protocol violation location at SRTR bit.
 */
#define CAN_ERR_PROT_LOC_SRTR    ( 0x09 )
/**
 * @brief Represents protocol violation location at reserved bit 0.
 */
#define CAN_ERR_PROT_LOC_RES0    ( 0x0a )
/**
 * @brief Represents protocol violation location at reserved bit 1.
 */
#define CAN_ERR_PROT_LOC_RES1    ( 0x0b )
/**
 * @brief Represents protocol violation location at DLC.
 */
#define CAN_ERR_PROT_LOC_DLC     ( 0x0c )
/**
 * @brief Represents protocol violation location at data section.
 */
#define CAN_ERR_PROT_LOC_DATA    ( 0x0d )
/**
 * @brief Represents protocol violation location at CRC sequence.
 */
#define CAN_ERR_PROT_LOC_CRCSEQ  ( 0x0e )
/**
 * @brief Represents protocol violation location at CRC delimiter.
 */
#define CAN_ERR_PROT_LOC_CRCDEL  ( 0x0f )
/**
 * @brief Represents protocol violation location at ACK slot.
 */
#define CAN_ERR_PROT_LOC_ACK     ( 0x10 )
/**
 * @brief Represents protocol violation location at ACK delimiter.
 */
#define CAN_ERR_PROT_LOC_ACKDEL  ( 0x11 )
/**
 * @brief Represents protocol violation location at end of frame.
 */
#define CAN_ERR_PROT_LOC_EOF     ( 0x12 )
/**
 * @brief Represents protocol violation location at intermission.
 */
#define CAN_ERR_PROT_LOC_INTERM  ( 0x13 )

/** @} */

/**
 * @name CAN Transmission Errors
 *
 * @{
 */

/**
 * @brief Represents unspecified transmission error.
 */
#define CAN_ERR_TRX_UNSPEC        ( 0x00 )
/**
 * @brief Represents mask for CANH transmission error.
 */
#define CAN_ERR_TRX_H_MASK        ( 0x0f )
/**
 * @brief Represents no CAN high wire detected.
 */
#define CAN_ERR_TRX_H_NOWIRE      ( 0x01 )
/**
 * @brief Identifies CAN H shortage to bat.
 */
#define CAN_ERR_TRX_H_SHORT2BAT   ( 0x02 )
/**
 * @brief Identifies CAN H shortage to VCC.
 */
#define CAN_ERR_TRX_H_SHORT2VCC   ( 0x03 )
/**
 * @brief Identifies CAN H shortage to ground.
 */
#define CAN_ERR_TRX_H_SHORT2GND   ( 0x04 )
/**
 * @brief Represents mask for CANL transmission error.
 */
#define CAN_ERR_TRX_L_MASK        ( 0xf0 )
/**
 * @brief Represents no CAN low wire detected.
 */
#define CAN_ERR_TRX_L_NOWIRE      ( 0x10 )
/**
 * @brief Identifies CAN L shortage to bat.
 */
#define CAN_ERR_TRX_L_SHORT2BAT   ( 0x20 )
/**
 * @brief Identifies CAN L shortage to VCC.
 */
#define CAN_ERR_TRX_L_SHORT2VCC   ( 0x30 )
/**
 * @brief Identifies CAN L shortage to ground.
 */
#define CAN_ERR_TRX_L_SHORT2GND   ( 0x40 )
/**
 * @brief Identifies CAN L shortage to CAN H.
 */
#define CAN_ERR_TRX_L_SHORT2CANH  ( 0x50 )

/** @} */
/** @} */

/**
 * @brief This structure represents the CAN message header.
 *
 */
struct can_frame_header {
  /**
   * @brief This member holds the CAN timestamp value.
   */
  uint64_t timestamp;
  /**
   * @brief This member holds the CAN identifier value. Only first 29 bits
   *  are be valid for CAN ID. In case 31st bit is set ( @ref CAN_ERR_ID_TAG ),
   *  the frame is not valid. In this case, it might be an error frame.
   */
  uint32_t can_id;
  /**
   * @brief This member holds the CAN flags. These are used to pass additional
   *  information between driver and application. For possible flags, refer
   *  to @ref CANFrameFlags group.
   */
  uint16_t flags;
  /**
   * @brief This member holds the data length of CAN frame. This length
   *  does not include size of the header.
   */
  uint16_t dlen;
};

/**
 * @brief This structure represents one CAN frame. It consists of
 *  CAN header and data.
 */
struct can_frame {
  /**
   * @brief This member stores the structure @ref can_frame_header
   * representing CAN header.
   */
  struct can_frame_header header;
  /**
   * @brief This member stores CAN data.
   */
  uint8_t data[CAN_FRAME_MAX_DLEN];
};

/**
 * @brief This function computes the length of CAN frame.
 *
 * @param frame Pointer to the @ref can_frame structure.
 *
 * @return Length of CAN frame in bytes.
 */
static inline size_t can_framesize( struct can_frame *frame )
{
  return offsetof( struct can_frame, data ) + frame->header.dlen;
}

/** @} */

#endif /* _DEV_CAN_CAN_FRAME_H */
