/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This header file is part of CAN/CAN FD bus common support. It defines
 *  structures used for bit timing representation and settings.
 *
 * Implementation is based on original LinCAN - Linux CAN bus driver
 * Part of OrtCAN project https://ortcan.sourceforge.net/
 */

/*
 * Copyright (C) 2023-2024 Michal Lenc <michallenc@seznam.cz>
 * Copyright (C) 2002-2009 DCE FEE CTU Prague
 * Copyright (C) 2002-2024 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 * Copyright (C) 2005      Stanislav Marek
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

#ifndef _DEV_CAN_CAN_BITTIMING_H
#define _DEV_CAN_CAN_BITTIMING_H

/**
 * @name Bit Timing Type
 *
 * @{
 */

/**
 * @brief This define is used to select nominal bit timing.
 */
#define RTEMS_CAN_BITTIME_NOMINAL ( 0 )
/**
 * @brief This define is used to select data bit timing.
 */
#define RTEMS_CAN_BITTIME_DATA    ( 1 )

/** @} */

/**
 * @name Bit Timing Source
 *
 * @{
 */

/**
 * @brief This define is used to use bit timing calculation from bitrate.
 */
#define RTEMS_CAN_BITTIME_FROM_BITRATE      ( 0 )
/**
 * @brief This define is used to use bit timing from precomputed values.
 */
#define RTEMS_CAN_BITTIME_FROM_PRECOMPUTED  ( 1 )

/** @} */

/**
 * @brief This structure is used to represent CAN bit timing constants
 *
 * Controller should define these constants based on its maximal and minimal
 * values.
 */
struct rtems_can_bittiming_const {
  /**
   * @brief This member holds the controller's name.
   */
  const char *name;
  /**
   * @brief This member holds mimimal possible TSEG1 value.
   */
  uint32_t tseg1_min;
  /**
   * @brief This member holds maximal possible TSEG1 value.
   */
  uint32_t tseg1_max;
  /**
   * @brief This member holds mimimal possible TSEG2 value.
   */
  uint32_t tseg2_min;
  /**
   * @brief This member holds maximal possible TSEG2 value.
   */
  uint32_t tseg2_max;
  /**
   * @brief This member holds maximal possible Sync Jump Width value.
   */
  uint32_t sjw_max;
  /**
   * @brief This member holds mimimal possible Bit Rate Prescaler value.
   */
  uint32_t brp_min;
  /**
   * @brief This member holds maximal possible Bit Rate Prescaler value.
   */
  uint32_t brp_max;
  /**
   * @brief This member holds Bit Rate Prescaler initial value.
   */
  uint32_t brp_inc;
};

/**
 * @brief This structure is used to represent CAN bit timing
 *
 * This structure is used to set new bit timing or obtain the current one.
 */
struct rtems_can_bittiming {
  /**
   * @brief This member holds CAN bitrate.
   */
  uint32_t bitrate;
  /**
   * @brief This member holds CAN sample point.
   */
  uint32_t sample_point;
  /**
   * @brief This member holds time quantum value.
   */
  uint32_t tq;
  /**
   * @brief This member holds propagation segment.
   */
  uint32_t prop_seg;
  /**
   * @brief This member holds phase segment 1.
   */
  uint32_t phase_seg1;
  /**
   * @brief This member holds phase segment 2.
   */
  uint32_t phase_seg2;
  /**
   * @brief This member holds Sync Jump Width value.
   */
  uint32_t sjw;
  /**
   * @brief This member holds Bit Rate Prescaler value.
   */
  uint32_t brp;
};

/**
 * @brief This structure is used to set CAN bit timing values via IOCTL call.
 *
 */
struct rtems_can_set_bittiming {
  /**
   * @brief This member specifies which bittiming is to be set (nominal, data)
   *  @ref RTEMS_CAN_BITTIME_NOMINAL - nominal bit timing
   *  @ref RTEMS_CAN_BITTIME_DATA    - data bit timing (for CAN FD chips)
   */
  uint16_t type;
  /**
   * @brief This member specifies the source of bit timing constants
   *  @ref RTEMS_CAN_BITTIME_FROM_BITRATE      - calculate from bitrate
   *  @ref RTEMS_CAN_BITTIME_FROM_PRECOMPUTED  - used precomputed values
   */
  uint16_t from;
  /**
   * @brief This member holds the @ref rtems_can_bittiming structure. This
   *  is used to specify bitrate or precomputed values.
   */
  struct rtems_can_bittiming bittiming;
};

/**
 * @brief This structure is used to get CAN bit timing values via IOCTL call.
 *
 */
struct rtems_can_get_bittiming {
  /**
   * @brief This member specifies which bittiming is to be set (nominal, data)
   *  @ref RTEMS_CAN_BITTIME_NOMINAL - nominal bit timing
   *  @ref RTEMS_CAN_BITTIME_DATA    - data bit timing (for CAN FD chips)
   */
  uint16_t type;
  /**
   * @brief This member holds the rtems_can_bittiming structure. This
   *  represents currently set values.
   */
  struct rtems_can_bittiming bittiming;
  /**
   * @brief This member holds the @ref rtems_can_bittiming_const structure.
   *  This represents maximum and minimal possible bit timing values.
   */
  struct rtems_can_bittiming_const bittiming_const;
};

#endif /* _DEV_CAN_CAN_BITTIMING_H */
