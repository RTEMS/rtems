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
 * Copyright (C) 2022 Prashanth S (fishesprashanth@gmail.com)
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

#ifndef _DEV_CAN_CAN_H
#define _DEV_CAN_CAN_H

#include <rtems/imfs.h>
#include <rtems/thread.h>
#include <semaphore.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dev/can/can-msg.h>

#define DEBUG(str, ...)                                                   \
    do {                                                                      \
      printf("CAN: %s:%d ID: %08X ", __FILE__, __LINE__, rtems_task_self());  \
      printf(str, ##__VA_ARGS__);                                             \
    } while (false);

#define CAN_DEBUG(str, ...) DEBUG(str, ##__VA_ARGS__)
#define CAN_DEBUG_BUF(str, ...) CAN_DEBUG(str, ##__VA_ARGS__)
#define CAN_DEBUG_ISR(str, ...) CAN_DEBUG(str, ##__VA_ARGS__)
#define CAN_DEBUG_LOCK(str, ...) CAN_DEBUG(str, ##__VA_ARGS__)
#define CAN_DEBUG_RX(str, ...) CAN_DEBUG(str, ##__VA_ARGS__)
#define CAN_DEBUG_TX(str, ...) CAN_DEBUG(str, ##__VA_ARGS__)
#define CAN_DEBUG_REG(str, ...) //CAN_DEBUG(str, ##__VA_ARGS__)
#define CAN_ERR(str, ...) DEBUG(str, ##__VA_ARGS__)

#define CAN_MSG_LEN(msg) ((char *)(&((struct can_msg *)msg)->data[(uint16_t)((struct can_msg *)msg)->len]) - (char *)(msg))

/* Maximum Bus Reg (255) */
#define CAN_BUS_REG_MAX (255)

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
 * @brief CAN tx fifo data structure.
 */
struct ring_buf {
 /**
   * @brief Pointer to array of can_msg structure.
   */
  struct can_msg *pbuf;
 /**
   * @brief Index of the next free buffer.
   */
  uint32_t head;
 /**
   * @brief Index of the produced buffer.
   */
  uint32_t tail;
  /**
   * @brief Number of empty buffers.
   */
  uint32_t empty_count;
};

/**
 * @brief CAN Controller device specific operations.
 * These function pointers are initialized by the CAN device driver while
 * registering (can_bus_register).
 */
typedef struct can_dev_ops {
  /**
   * @brief Transfers CAN messages to device fifo.
   *
   * @param[in] priv device control structure.
   * @param[in] msg can_msg message structure.
   *
   * @retval 0 Successful operation.
   * @retval >0 error number in case of an error.
   */
  int32_t (*dev_tx)(void *priv, struct can_msg *msg);
  /**
   * @brief Check device is ready to transfer a CAN message
   *
   * @param[in] priv device control structure.
   *
   * @retval true device ready.
   * @retval false device not ready.
   */
  bool (*dev_tx_ready)(void *priv);
  /**
   * @brief Enable/Disable CAN interrupts.
   *
   * @param[in] priv device control structure.
   * @param[in] flag true/false to Enable/Disable CAN interrupts.
   *
   */
  void (*dev_int)(void *priv, bool flag);
  /**
   * @brief CAN device specific I/O controls.
   *
   * @param[in] priv device control structure.
   * @param[in] buffer This depends on the cmd.
   * @param[in] cmd Device specific I/O commands.
   *
   * @retval 0 Depends on the cmd.
   */
  int32_t (*dev_ioctl)(void *priv, void *buffer, size_t cmd);
} can_dev_ops;

/**
 * @name CAN bus control
 *
 * @{
 */

/**
 * @brief Obtains the bus.
 *
 * This command has no argument.
 */
typedef struct can_bus {
 /**
   * @brief Device specific control.
   */
  void *priv;
 /**
   * @brief Device controller index.
   */
  uint8_t index;
 /**
   * @brief Device specific operations.
   */
  struct can_dev_ops *can_dev_ops;
 /**
   * @brief tx fifo.
   */
  struct ring_buf tx_fifo;
 /**
   * @brief Counting semaphore id (for fifo sync).
   */
  rtems_id tx_fifo_sem_id;

 /* FIXME: Using only one CAN msg buffer, Should create a ring buffer */
 /**
   * @brief rx fifo.
   */
  struct can_msg can_rx_msg;
 /**
   * @brief Mutex to handle bus concurrency.
   */
  rtems_mutex mutex;
  /**
   * @brief Destroys the bus.
   *
   * @param[in] bus control structure.
   */
  void (*destroy)(struct can_bus *bus);
#ifdef CAN_DEBUG_LOCK

 /**
   * @brief For debugging semaphore obtain/release.
   */
  int sem_count;

#endif /* CAN_DEBUG_LOCK */

} can_bus;

/** @} */

/**
 * @brief Register a CAN node with the CAN bus driver.
 *
 * @param[in] bus bus control structure.
 * @param[in] bus_path path of device node.
 *
 * @retval >=0 rtems status.
 */
rtems_status_code can_bus_register(can_bus *bus, const char *bus_path);

/**
 * @brief Allocate and initilaize bus control structure.
 *
 * @param[in] size Size of the bus control structure.
 *
 * @retval NULL No memory available.
 * @retval Address Pointer to the allocated bus control structure.
 */
can_bus *can_bus_alloc_and_init(size_t size);

/**
 * @brief initilaize bus control structure.
 *
 * @param[in] bus bus control structure.
 *
 * @retval 0 success.
 * @retval >0 error number.
 */
int can_bus_init(can_bus *bus);

/**
 * @brief Initiates CAN message transfer.
 * 
 * Should be called with CAN interrupt disabled.
 *
 * @param[in] bus Bus control structure.
 *
 * @retval 0 success.
 * @retval >0 error number.
 */
int can_tx_done(struct can_bus *bus);

/**
 * @brief Sends the received CAN message to the application.
 * 
 * Should be called by the device when CAN message should be sent to applicaiton.
 * Should be called only with CAN interrupts disabled.
 *
 * @param[in] bus bus control structure.
 * @param[in] msg can_msg structure.
 *
 * @retval 0 success.
 * @retval >0 error number.
 */
int can_receive(struct can_bus *bus, struct can_msg *msg);

/**
 * @brief Prints the can_msg values pointed by msg.
 * 
 * @param[in] msg can_msg structure.
 *
 */
void can_print_msg(struct can_msg const *msg);

/** @} */  /* end of CAN device driver */

/** @} */

#endif /* _DEV_CAN_CAN_H */
