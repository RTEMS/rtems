/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup versal_uart
 *
 * @brief Xilinx Versal UART support.
 */

/*
 * Copyright (C) 2021 Gedare Bloom <gedare@rtems.org>
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

#ifndef LIBBSP_ARM_XILINX_VERSAL_UART_H
#define LIBBSP_ARM_XILINX_VERSAL_UART_H

#include <rtems/termiostypes.h>
#include <dev/serial/arm-pl011.h>
#include <dev/serial/arm-pl011-regs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup versal_uart Xilinx Versal UART Support
 * @ingroup RTEMSBSPsARMVersal
 * @brief UART Support
 *
 * This driver operates an instance of the Xilinx UART present in the
 * family of Xilinx Versal SoCs.
 */

typedef struct {
  arm_pl011_context pl011_ctx;
  volatile bool transmitting;
} versal_pl011_context;

extern const rtems_termios_device_handler versal_uart_handler;

#define VERSAL_UART_DEFAULT_BAUD 115200

int versal_uart_initialize(rtems_termios_device_context *base);

void versal_uart_reset_tx_flush(rtems_termios_device_context *base);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_VERSAL_UART_H */
