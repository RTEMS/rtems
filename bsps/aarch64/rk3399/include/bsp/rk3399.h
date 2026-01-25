/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RK3399
 *
 * @brief Register definitions.
 */

/*
 * Copyright (C) 2026 Ning Yang <yangn0@qq.com>
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

#ifndef LIBBSP_AARCH64_RK3399_RK3399_H
#define LIBBSP_AARCH64_RK3399_RK3399_H

#include <bsp/utility.h>
#include <bspopts.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name RK3399 UARTs
 *
 * @{
 */
#define RK3399_UART_BASE 0xff180000
#define RK3399_UART_DEVICE_SIZE 0x10000

#define RK3399_UART0_BASE (RK3399_UART_BASE + 0x00000)
#define RK3399_UART0_SIZE RK3399_UART_DEVICE_SIZE
#define RK3399_UART1_BASE (RK3399_UART_BASE + 0x10000)
#define RK3399_UART1_SIZE RK3399_UART_DEVICE_SIZE
#define RK3399_UART2_BASE (RK3399_UART_BASE + 0x20000)
#define RK3399_UART2_SIZE RK3399_UART_DEVICE_SIZE
#define RK3399_UART3_BASE (RK3399_UART_BASE + 0x30000)
#define RK3399_UART3_SIZE RK3399_UART_DEVICE_SIZE

#define RK3399_UART4_BASE 0xff370000
#define RK3399_UART4_SIZE RK3399_UART_DEVICE_SIZE

#define RK3399_UART_DEFAULT_BAUD 1500000
/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AARCH64_RK3399_RK3399_H */