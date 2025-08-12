/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RaspberryPi
 *
 * @brief Raspberry Pi 4B Console Device Definitions
 */

/*
 * Copyright (C) 2023 Utkarsh Verma
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

#ifndef LIBBSP_AARCH64_RASPBERRYPI_BSP_RASPBERRYPI_UART_H
#define LIBBSP_AARCH64_RASPBERRYPI_BSP_RASPBERRYPI_UART_H

#include <bspopts.h>
#include <bsp/irq.h>
#include <bsp/raspberrypi.h>
#include <bsp/rpi-gpio.h>
#include <dev/serial/arm-pl011.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This macro exists to serve as a common point of definition for the
 * parameters of the UARTs present in the Raspberry Pi 4. It is used in
 * multiple locations with different rendering macros to prevent duplication
 * of information.
 */
#define RASPBERRYPI_CONSOLE_DEVICES(CONSOLE_DEVICE_DEFINITION_RENDERER) \
    CONSOLE_DEVICE_DEFINITION_RENDERER( \
      0, "/dev/ttyAMA0", BCM2711_UART0_BASE,BCM2711_UART0_SIZE, \
      BSP_PL011_CLOCK_FREQ, BCM2711_IRQ_PL011_UART, arm_pl011_context, \
      &arm_pl011_fns, arm_pl011_write_polled, 15, 14, GPIO_AF0) \
    CONSOLE_DEVICE_DEFINITION_RENDERER( \
      2, "/dev/ttyAMA1", BCM2711_UART2_BASE, BCM2711_UART2_SIZE, \
      BSP_PL011_CLOCK_FREQ, BCM2711_IRQ_PL011_UART, arm_pl011_context, \
      &arm_pl011_fns, arm_pl011_write_polled, 1, 0, GPIO_AF4) \
    CONSOLE_DEVICE_DEFINITION_RENDERER( \
      3, "/dev/ttyAMA2", BCM2711_UART3_BASE, BCM2711_UART3_SIZE, \
      BSP_PL011_CLOCK_FREQ, BCM2711_IRQ_PL011_UART, arm_pl011_context, \
      &arm_pl011_fns, arm_pl011_write_polled, 5, 4, GPIO_AF4) \
    CONSOLE_DEVICE_DEFINITION_RENDERER( \
      4, "/dev/ttyAMA3", BCM2711_UART4_BASE, BCM2711_UART4_SIZE, \
      BSP_PL011_CLOCK_FREQ, BCM2711_IRQ_PL011_UART, arm_pl011_context, \
      &arm_pl011_fns, arm_pl011_write_polled, 9, 8, GPIO_AF4) \
    CONSOLE_DEVICE_DEFINITION_RENDERER( \
     5, "/dev/ttyAMA4", BCM2711_UART5_BASE, BCM2711_UART5_SIZE, \
      BSP_PL011_CLOCK_FREQ, BCM2711_IRQ_PL011_UART, arm_pl011_context, \
      &arm_pl011_fns, arm_pl011_write_polled, 13, 12, GPIO_AF4)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AARCH64_RASPBERRYPI_BSP_RASPBERRYPI_UART_H */
