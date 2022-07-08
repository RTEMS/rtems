/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsLM32EVR
 *
 * @brief Global BSP definitions.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#ifndef LIBBSP_LM32_LM32_EVR_BSP_H
#define LIBBSP_LM32_LM32_EVR_BSP_H

/**
 * @defgroup RTEMSBSPsLM32EVR LM32 EVR
 *
 * @ingroup RTEMSBSPsLM32
 *
 * @brief LM32 EVR Board Support Package.
 *
 * @{
 */
#include <stdint.h>
#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * lm32 requires certain aligment of mbuf because unaligned uint32_t
 * accesses are not handled properly.
 */

#define CPU_U32_FIX

#if defined(RTEMS_NETWORKING)
struct rtems_bsdnet_ifconfig;

extern int rtems_tsmac_driver_attach(struct rtems_bsdnet_ifconfig *config,
				     int attaching);

#define RTEMS_BSP_NETWORK_DRIVER_NAME "TSMAC0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_tsmac_driver_attach

  /*
   * Due to a hardware design error (RJ45 connector with 10baseT magnetics)
   * we are forced to use 10baseT mode.
   */

#define TSMAC_FORCE_10BASET
#endif

/* functions */
rtems_isr_entry set_vector(                     /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

/*
 * Prototypes for BSP methods that cross file boundaries
 */
void BSP_uart_polled_write(char ch);
int BSP_uart_polled_read( void );
char BSP_uart_is_character_ready(char *ch);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
