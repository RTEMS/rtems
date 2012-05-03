/**
 * @file
 *
 * @ingroup QorIQUartBridge
 *
 * @brief UART to Intercom bridge API.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_QORIQ_UART_BRIDGE_H
#define LIBBSP_POWERPC_QORIQ_UART_BRIDGE_H

#include <libchip/serial.h>

#include <bsp/intercom.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup QorIQUartBridge QorIQ - UART to Intercom Bridge Support
 *
 * @ingroup QorIQ
 *
 * @brief UART to Intercom bridge support.
 *
 * @{
 */

typedef struct {
  const char *device_path;
  intercom_type type;
  rtems_id transmit_task;
  rtems_chain_control transmit_fifo;
} uart_bridge_master_control;

typedef struct {
  struct rtems_termios_tty *tty;
  intercom_type type;
  rtems_id transmit_task;
  rtems_chain_control transmit_fifo;
} uart_bridge_slave_control;

extern console_fns qoriq_uart_bridge_master;

extern console_fns qoriq_uart_bridge_slave;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_QORIQ_UART_BRIDGE_H */
