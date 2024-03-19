/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup zynq_uart
 *
 * @brief This header file provides interfaces with respect to the Zynq
 *   UltraScale+ MPSoC and RFSoC platforms.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#ifndef _DEV_SERIAL_ZYNQ_UART_ZYNQMP_H
#define _DEV_SERIAL_ZYNQ_UART_ZYNQMP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup zynq_uart
 *
 * @{
 */

/**
 * @brief This constant defines the Xilinx Zynq UART 0 base address.
 */
#define ZYNQ_UART_0_BASE_ADDR 0xff000000

/**
 * @brief This constant defines the Xilinx Zynq UART 1 base address.
 */
#define ZYNQ_UART_1_BASE_ADDR 0xff010000

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEV_SERIAL_ZYNQ_UART_ZYNQMP_H */
