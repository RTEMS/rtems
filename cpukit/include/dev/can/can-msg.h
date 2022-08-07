/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CANBus
 *
 * @brief Controller Area Network (CAN) Bus Implementation
 *
 */

/*
 * Copyright (C) 2022 Prashanth S <fishesprashanth@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
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

#ifndef _DEV_CAN_CAN_MSG_H
#define _DEV_CAN_CAN_MSG_H

/**
 * @defgroup Controller Area Network (CAN) Driver
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief Controller Area Network (CAN) bus and device driver support.
 *
 * @{
 */

/**
 * @defgroup CANBus CAN Bus Driver
 *
 * @ingroup CAN
 *
 * @{
 */

/**
 * @brief CAN message size
 */
#define CAN_MSG_MAX_SIZE  (8u)

/**
 * @brief Number of CAN tx buffers
 */
#define CAN_TX_BUF_COUNT  (10u)

/**
 * @brief Number of CAN rx buffers
 */
#define CAN_RX_BUF_COUNT  (2u)

#define CAN_MSGLEN(msg) (sizeof(struct can_msg) - (CAN_MSG_MAX_SIZE - msg->len))

/**
 * @brief CAN message structure.
 */
struct can_msg {
 /**
   * @brief CAN message id.
   */
  uint32_t id;
 /**
   * @brief CAN message timestamp.
   */
  uint32_t timestamp;
 /**
   * @brief CAN message flags RTR | BRS | EXT.
   */
  uint16_t flags;
 /**
   * @brief CAN message length.
   */
  uint16_t len;
 /**
   * @brief CAN message.
   */
  uint8_t data[CAN_MSG_MAX_SIZE];
};

/** @} */  /* end of CAN message */

/** @} */

#endif /* _DEV_CAN_CAN_MSG_H */
