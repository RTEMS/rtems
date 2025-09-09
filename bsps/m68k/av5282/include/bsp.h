/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup RTEMSBSPsM68kAV5282
 *
 * @brief av5282 BSP header file
 */

/*
 * Copyright (C) 2005 Eric Norum <eric@norum.ca>
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

#ifndef LIBBSP_M68K_AV5282_BSP_H
#define LIBBSP_M68K_AV5282_BSP_H

/**
 * @defgroup RTEMSBSPsM68kAV5282 AV5282
 *
 * @ingroup RTEMSBSPsM68k
 *
 * @brief AV5282 Board Support Package.
 *
 * @{
 */

#ifndef ASM

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/bspIo.h>

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf5282/mcf5282.h>   /* internal MCF5282 modules */

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
/**  Network driver configuration                                         **/
struct rtems_bsdnet_ifconfig;
extern int rtems_fec_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching );
#define RTEMS_BSP_NETWORK_DRIVER_NAME     "fs1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH   rtems_fec_driver_attach

/***************************************************************************/
/**  User Definable configuration                                         **/

/* define which port the console should use - all other ports are then defined as general purpose */
#define CONSOLE_PORT        0

/* externals */

/* constants */

/* miscellaneous stuff assumed to exist */

/* functions */

uint32_t get_CPU_clock_speed(void);

/*
 * Interrupt assignments
 *  Highest-priority listed first
 */
#define FEC_IRQ_LEVEL       4
#define FEC_IRQ_RX_PRIORITY 7
#define FEC_IRQ_TX_PRIORITY 6

#define PIT3_IRQ_LEVEL      4
#define PIT3_IRQ_PRIORITY   0

#define UART0_IRQ_LEVEL     3
#define UART0_IRQ_PRIORITY  7
#define UART1_IRQ_LEVEL     3
#define UART1_IRQ_PRIORITY  6
#define UART2_IRQ_LEVEL     3
#define UART2_IRQ_PRIORITY  5

/*
 * Prototypes for methods called from .S to alow dependency tracking
 */
void Init5282(void);

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

/** @} */

#endif
