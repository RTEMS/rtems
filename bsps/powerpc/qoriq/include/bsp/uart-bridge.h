/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQUartBridge
 *
 * @brief UART to Intercom bridge API.
 */

/*
 * Copyright (C) 2011, 2015 embedded brains GmbH & Co. KG
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
