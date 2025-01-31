/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This header file is part of CAN/CAN FD bus common support. It
 *  implements structure that represents filter for CAN frames.
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

#ifndef _DEV_CAN_CAN_FILTER_H
#define _DEV_CAN_CAN_FILTER_H

/**
 * @brief This structure is used to represent CAN ID and flags in one
 * unified structure.
 *
 */
struct rtems_can_filter {
  /**
   * @brief This member is a bitfield that holds required CAN identifier
   *  values to assign CAN frame to the FIFO queue.
   */
  uint32_t id;
  /**
   * @brief This member is a bitfield that holds forbidden CAN identifier
   *  values. If the frame has identifier in range of this bit field,
   *  it is not assigned to the FIFO queue.
   */
  uint32_t id_mask;
  /**
   * @brief This member is a bitfield that holds CAN flags required in
   *  CAN frame to be assigned to the FIFO queue.
   *
   *  Refer to @ref CANFrameFlags for possible flags.
   */
  uint32_t flags;
  /**
   * @brief This member is a bitfield that holds CAN flags forbidden in
   *  CAN frame. If the frame has some of these flags, it is not assigned
   *  to the FIFO queue.
   *
   *  Refer to @ref CANFrameFlags for possible flags.
   */
  uint32_t flags_mask;
};

#endif /* _DEV_CAN_CAN_FILTER_H */
