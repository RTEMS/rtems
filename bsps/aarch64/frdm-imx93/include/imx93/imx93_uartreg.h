/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRTEMSBSPsAArch64FRDMIMX93
 *
 * @brief UART register definitions.
 */

/*
 * Copyright (C) 2026 Rongjie Lee
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

#include <bsp/utility.h>

#define UARTSTAT_TDRE BSP_BIT32( 23 )
#define UARTSTAT_TC   BSP_BIT32( 22 )
#define UARTSTAT_RDRF BSP_BIT32( 21 )

#define UARTCTRL_TIE                       BSP_BIT32( 23 )
#define UARTCTRL_TCIE                      BSP_BIT32( 22 )
#define UARTCTRL_RIE                       BSP_BIT32( 21 )
#define UARTCTRL_ILIE                      BSP_BIT32( 20 )
#define UARTCTRL_TE                        BSP_BIT32( 19 )
#define UARTCTRL_RE                        BSP_BIT32( 18 )
#define UARTCTRL_SBK                       BSP_BIT32( 16 )
#define IMX93_UARTCTRL_IDLECFG( reg, val ) BSP_FLD32SET( reg, val, 8, 10 )

#define IMX93_UART_DATA( val )     BSP_FLD32( val, 0, 15 )
#define IMX93_UART_DATA_GET( reg ) BSP_FLD32GET( reg, 0, 9 )

#define UARTMODIR_TXCTSE                     BSP_BIT32( 0 )
#define IMX93_UARTMODIR_RTSWATER( reg, val ) BSP_FLD32SET( reg, val, 8, 11 )

#define UARTFIFO_RXUF                        BSP_BIT32( 16 )
#define UARTFIFO_TXFLUSH                     BSP_BIT32( 15 )
#define UARTFIFO_RXFLUSH                     BSP_BIT32( 14 )
#define UARTFIFO_TXFE                        BSP_BIT32( 7 )
#define UARTFIFO_RXFE                        BSP_BIT32( 3 )
#define IMX93_UARTFIFO( val )                BSP_FLD32( val, 0, 23 )
#define IMX93_UARTFIFO_RXIDEN( reg, val )    BSP_FLD32SET( reg, val, 10, 12 )
#define IMX93_UARTFIFO_RXFIFOSIZE_GET( reg ) BSP_FLD32GET( reg, 0, 3 )

#define UARTWATER_TXWATER_OFF 0
#define UARTWATER_RXWATER_OFF 16
#define UARTWARER_RXCNT_OFF   24

typedef struct {
  uint32_t VERID;  /* 0x00 */
  uint32_t PARAM;  /* 0x04 */
  uint32_t GLOBAL; /* 0x08 */
  uint32_t PINCFG; /* 0x0C */
  uint32_t BAUD;   /* 0x10 */
  uint32_t STAT;   /* 0x14 */
  uint32_t CTRL;   /* 0x18 */
  uint32_t DATA;   /* 0x1C */
  uint32_t MATCH;  /* 0x20 */
  uint32_t MODIR;  /* 0x24 */
  uint32_t FIFO;   /* 0x28 */
  uint32_t WATER;  /* 0x2C */
  uint32_t DATARO; /* 0x30 */

  uint32_t reserved0[ 3 ]; /* 0x34-0x3C */

  uint32_t MCR;  /* 0x40 */
  uint32_t MSR;  /* 0x44 */
  uint32_t REIR; /* 0x48 */
  uint32_t TEIR; /* 0x4C */
  uint32_t HDCR; /* 0x50 */
  uint32_t TOCR; /* 0x58 */
  uint32_t TOSR; /* 0x5C */

  uint32_t TIMEOUTN[ 4 ]; /* 0x60-0x6C */

} imx93_uart;
