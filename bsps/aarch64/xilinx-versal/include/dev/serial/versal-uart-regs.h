/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup versal_uart_regs
 *
 * @brief Xilinx Versal UART register definitions.
 *
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

/**
 * @defgroup versal_uart_regs Xilinx Versal UART Register Definitions
 * @ingroup versal
 * @brief Xilinx Versal UART Register Definitions
 * @{
 */

#ifndef LIBBSP_ARM_XILINX_VERSAL_UART_REGS_H
#define LIBBSP_ARM_XILINX_VERSAL_UART_REGS_H

#include <bsp/utility.h>

#define VERSAL_UART_FIFO_DEPTH 64

typedef struct versal_uart {
  uint32_t uartdr;
#define VERSAL_UARTDR_OE BSP_BIT32(11)
#define VERSAL_UARTDR_BE BSP_BIT32(10)
#define VERSAL_UARTDR_PE BSP_BIT32(9)
#define VERSAL_UARTDR_FE BSP_BIT32(8)
#define VERSAL_UARTDR_DATA(val) BSP_FLD32(val, 0, 7)
#define VERSAL_UARTDR_DATA_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define VERSAL_UARTDR_DATA_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
  uint32_t uartrsr_uartecr;
#define VERSAL_UARTRSR_UARTECR_OE BSP_BIT32(3)
#define VERSAL_UARTRSR_UARTECR_BE BSP_BIT32(2)
#define VERSAL_UARTRSR_UARTECR_PE BSP_BIT32(1)
#define VERSAL_UARTRSR_UARTECR_FE BSP_BIT32(0)
  uint32_t reserved_08[4];
  uint32_t uartfr;
#define VERSAL_UARTFR_RI BSP_BIT32(8)
#define VERSAL_UARTFR_TXFE BSP_BIT32(7)
#define VERSAL_UARTFR_RXFF BSP_BIT32(6)
#define VERSAL_UARTFR_TXFF BSP_BIT32(5)
#define VERSAL_UARTFR_RXFE BSP_BIT32(4)
#define VERSAL_UARTFR_BUSY BSP_BIT32(3)
#define VERSAL_UARTFR_DCD BSP_BIT32(2)
#define VERSAL_UARTFR_DSR BSP_BIT32(1)
#define VERSAL_UARTFR_CTS BSP_BIT32(0)
  uint32_t reserved_1c;
  uint32_t uartilpr;
#define VERSAL_UARTILPR_ILPDVSR(val) BSP_FLD32(val, 0, 7)
#define VERSAL_UARTILPR_ILPDVSR_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define VERSAL_UARTILPR_ILPDVSR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
  uint32_t uartibrd;
#define VERSAL_UARTIBRD_BAUD_DIVINT(val) BSP_FLD32(val, 0, 15)
#define VERSAL_UARTIBRD_BAUD_DIVINT_GET(reg) BSP_FLD32GET(reg, 0, 15)
#define VERSAL_UARTIBRD_BAUD_DIVINT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)
  uint32_t uartfbrd;
#define VERSAL_UARTFBRD_BAUD_DIVFRAC(val) BSP_FLD32(val, 0, 5)
#define VERSAL_UARTFBRD_BAUD_DIVFRAC_GET(reg) BSP_FLD32GET(reg, 0, 5)
#define VERSAL_UARTFBRD_BAUD_DIVFRAC_SET(reg, val) BSP_FLD32SET(reg, val, 0, 5)
  uint32_t uartlcr_h;
#define VERSAL_UARTLCR_H_SPS BSP_BIT32(7)
#define VERSAL_UARTLCR_H_WLEN(val) BSP_FLD32(val, 5, 6)
#define VERSAL_UARTLCR_H_WLEN_GET(reg) BSP_FLD32GET(reg, 5, 6)
#define VERSAL_UARTLCR_H_WLEN_SET(reg, val) BSP_FLD32SET(reg, val, 5, 6)
#define VERSAL_UARTLCR_H_WLEN_5 0x00U
#define VERSAL_UARTLCR_H_WLEN_6 0x01U
#define VERSAL_UARTLCR_H_WLEN_7 0x02U
#define VERSAL_UARTLCR_H_WLEN_8 0x03U
#define VERSAL_UARTLCR_H_FEN BSP_BIT32(4)
#define VERSAL_UARTLCR_H_STP2 BSP_BIT32(3)
#define VERSAL_UARTLCR_H_EPS BSP_BIT32(2)
#define VERSAL_UARTLCR_H_PEN BSP_BIT32(1)
#define VERSAL_UARTLCR_H_BRK BSP_BIT32(0)
  uint32_t uartcr;
#define VERSAL_UARTCR_CTSEN BSP_BIT32(15)
#define VERSAL_UARTCR_RTSEN BSP_BIT32(14)
#define VERSAL_UARTCR_OUT2 BSP_BIT32(13)
#define VERSAL_UARTCR_OUT1 BSP_BIT32(12)
#define VERSAL_UARTCR_RTS BSP_BIT32(11)
#define VERSAL_UARTCR_DTR BSP_BIT32(10)
#define VERSAL_UARTCR_RXE BSP_BIT32(9)
#define VERSAL_UARTCR_TXE BSP_BIT32(8)
#define VERSAL_UARTCR_LBE BSP_BIT32(7)
#define VERSAL_UARTCR_UARTEN BSP_BIT32(0)
  uint32_t uartifls;
#define VERSAL_UARTIFLS_RXIFLSEL(val) BSP_FLD32(val, 3, 5)
#define VERSAL_UARTIFLS_RXIFLSEL_GET(reg) BSP_FLD32GET(reg, 3, 5)
#define VERSAL_UARTIFLS_RXIFLSEL_SET(reg, val) BSP_FLD32SET(reg, val, 3, 5)
#define VERSAL_UARTIFLS_TXIFLSEL(val) BSP_FLD32(val, 0, 2)
#define VERSAL_UARTIFLS_TXIFLSEL_GET(reg) BSP_FLD32GET(reg, 0, 2)
#define VERSAL_UARTIFLS_TXIFLSEL_SET(reg, val) BSP_FLD32SET(reg, val, 0, 2)
  uint32_t uartimsc;
  uint32_t uartris;
  uint32_t uartmis;
  uint32_t uarticr;
#define VERSAL_UARTI_OEI BSP_BIT32(10)
#define VERSAL_UARTI_BEI BSP_BIT32(9)
#define VERSAL_UARTI_PEI BSP_BIT32(8)
#define VERSAL_UARTI_FEI BSP_BIT32(7)
#define VERSAL_UARTI_RTI BSP_BIT32(6)
#define VERSAL_UARTI_TXI BSP_BIT32(5)
#define VERSAL_UARTI_RXI BSP_BIT32(4)
#define VERSAL_UARTI_DSRMI BSP_BIT32(3)
#define VERSAL_UARTI_DCDMI BSP_BIT32(2)
#define VERSAL_UARTI_CTSMI BSP_BIT32(1)
#define VERSAL_UARTI_RIMI BSP_BIT32(0)
} versal_uart;

/** @} */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_UART_REGS_H */
