/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQUartBridge
 *
 * @brief UART to Intercom bridge API.
 */

/*
 * Copyright (c) 2011-2015 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_POWERPC_QORIQ_UART_BRIDGE_H
#define LIBBSP_POWERPC_QORIQ_UART_BRIDGE_H

#include <rtems/termiostypes.h>

#include <bsp/intercom.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup QorIQUartBridge QorIQ - UART to Intercom Bridge Support
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief UART to Intercom bridge support.
 *
 * @{
 */

typedef struct {
  rtems_termios_device_context base;
  const char *device_path;
  intercom_type type;
  rtems_id transmit_task;
  rtems_chain_control transmit_fifo;
} uart_bridge_master_context;

typedef struct {
  rtems_termios_device_context base;
  struct rtems_termios_tty *tty;
  intercom_type type;
  rtems_id transmit_task;
  rtems_chain_control transmit_fifo;
} uart_bridge_slave_context;

bool qoriq_uart_bridge_master_probe(rtems_termios_device_context *base);

extern const rtems_termios_device_handler qoriq_uart_bridge_master;

extern const rtems_termios_device_handler qoriq_uart_bridge_slave;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_QORIQ_UART_BRIDGE_H */
