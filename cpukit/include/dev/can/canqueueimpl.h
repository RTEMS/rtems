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

#ifndef _DEV_CAN_CAN_QUEUE_H
#define _DEV_CAN_CAN_QUEUE_H

#include <rtems/imfs.h>
#include <rtems/thread.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dev/can/can-msg.h>
#include <dev/can/can.h>

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
 * @brief Create CAN tx buffers.
 *
 * @param[in] bus Bus control structure.
 *
 * @retval 0 Successful operation.
 * @retval >0 error number in case of an error.
 */
static rtems_status_code can_bus_create_tx_buffers(struct can_bus *bus);

/**
 * @brief Free CAN tx buffers.
 *
 * @param[in] bus Bus control structure.
 *
 */
static void can_bus_free_tx_buffers(struct can_bus *bus);

/**
 * @brief Check for atleast one empty CAN tx buffer.
 *
 * @param[in] bus Bus control structure.
 *
 * @retval true If atleast one CAN buffer is empty.
 * @retval false If no CAN buffer is empty.
 */
static bool can_bus_tx_buf_is_empty(struct can_bus *bus);

/**
 * @brief Get a produced tx buffer to transmit from the tx fifo.
 *
 * @param[in] bus Bus control structure.
 *
 * @retval can_msg Pointer to can_msg structure buffer.
 * @retval NULL If no can_msg buffer.
 */
static struct can_msg *can_bus_tx_get_data_buf(struct can_bus *bus);

/**
 * @brief Get a empty tx buffer.
 *
 * @param[in] bus Bus control structure.
 *
 * @retval can_msg Pointer to can_msg structure buffer.
 * @retval NULL If no empty can_msg buffer.
 */
static struct can_msg *can_bus_tx_get_empty_buf(struct can_bus *bus);

/**
 * @brief Creates tx buffers for the CAN bus driver.
 *
 * @param[in] bus Bus control structure.
 *
 * @retval rtems_status_code
 */
static rtems_status_code can_bus_create_tx_buffers(struct can_bus *bus)
{
  bus->tx_fifo.pbuf = (struct can_msg *)malloc(CAN_TX_BUF_COUNT *
                              sizeof(struct can_msg));
  if (bus->tx_fifo.pbuf == NULL) {
    CAN_ERR("can_create_tx_buffers: malloc failed\n");
    return RTEMS_NO_MEMORY;
  }

  bus->tx_fifo.empty_count = CAN_TX_BUF_COUNT;

  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Free tx buffers for the CAN bus driver.
 *
 * @param[in] bus Bus control structure.
 *
 */
static void can_bus_free_tx_buffers(struct can_bus *bus)
{
  free(bus->tx_fifo.pbuf);
}

/**
 * @brief Check if there is atleast one tx buffer in the CAN
 * bus driver.
 *
 * @param[in] bus Bus control structure.
 *
 * @retval true - If there is at least one free tx buffer.
 *         false - If there is no free tx buffer.
 */
static bool can_bus_tx_buf_is_empty(struct can_bus *bus)
{
  if (bus->tx_fifo.empty_count == 0) {
    return false;
  }

  return true;
}

/**
 * @brief To get a can_msg tx buf which contains valid data to send in 
 * in the CAN bus.
 *
 * Note: freeing the returned data buf is done in the same function,
 * So the returned buffer should be sent before releasing the
 * lock acquired while calling this function.
 *
 * @param[in] bus Bus control structure.
 *
 * @retval *can_msg - If there is atleast one tx buffer to send in the CAN bus.
 *         NULL - If there is no valid tx buffer.
 */
static struct can_msg *can_bus_tx_get_data_buf(struct can_bus *bus)
{
  struct can_msg *msg = NULL;

  if (bus->tx_fifo.empty_count == CAN_TX_BUF_COUNT || 
                                    bus->tx_fifo.tail >= CAN_TX_BUF_COUNT) {
    CAN_DEBUG_BUF("can_bus_tx_get_data_buf: All buffers are empty\n");
    return NULL;
  }

  msg = &bus->tx_fifo.pbuf[bus->tx_fifo.tail];
  bus->tx_fifo.empty_count++;
  bus->tx_fifo.tail = (bus->tx_fifo.tail + 1) % CAN_TX_BUF_COUNT;

  return msg;
}
    
/**
 * @brief To get a can_msg tx buf which is empty (contains no valid data).
 *
 * Note: marking the returned buf valid is done in the same function
 * So a valid CAN message should be copied to the returned buffer before
 * releasing the lock acquired while calling this function.
 *
 * @param[in] bus Bus control structure.
 *
 * @retval *can_msg - If there is atleast one empty tx buffer.
 *         NULL - If there is no empty tx buffer.
 */
static struct can_msg *can_bus_tx_get_empty_buf(struct can_bus *bus)
{
  struct can_msg *msg = NULL;

  /* Check whether there is a empty CAN msg buffer */
  if (can_bus_tx_buf_is_empty(bus) == false) {
    CAN_DEBUG_BUF("can_bus_tx_get_empty_buf: No empty buffer\n");
    return NULL;
  }

  bus->tx_fifo.empty_count--;

  /* tx_fifo.head always points to a empty buffer if there is atleast one */
  msg = &bus->tx_fifo.pbuf[bus->tx_fifo.head];
  bus->tx_fifo.head = (bus->tx_fifo.head + 1) % CAN_TX_BUF_COUNT;

  return msg;
}

/** @} */  /* end of CAN device driver */

/** @} */

#endif /*_DEV_CAN_CAN_QUEUE_H */
