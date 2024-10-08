/**
 * @file
 *
 * @ingroup RTEMSDeviceSerialZynq
 *
 * @brief This header file provides Zynq UART interfaces.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2013 embedded brains GmbH & Co. KG
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
 * @defgroup RTEMSDeviceSerialZynq Zynq UART Support
 *
 * @ingroup RTEMSDeviceConsole
 *
 * @brief This group contains the Zynq UART support.
 *
 * @{
 */

#ifndef LIBBSP_ARM_XILINX_ZYNQ_UART_REGS_H
#define LIBBSP_ARM_XILINX_ZYNQ_UART_REGS_H

#include <bsp/utility.h>

#define ZYNQ_UART_DEFAULT_BAUD 115200

#define ZYNQ_UART_FIFO_DEPTH 64

typedef struct zynq_uart {
	uint32_t control;
#define ZYNQ_UART_CONTROL_STPBRK BSP_BIT32(8)
#define ZYNQ_UART_CONTROL_STTBRK BSP_BIT32(7)
#define ZYNQ_UART_CONTROL_RSTTO BSP_BIT32(6)
#define ZYNQ_UART_CONTROL_TXDIS BSP_BIT32(5)
#define ZYNQ_UART_CONTROL_TXEN BSP_BIT32(4)
#define ZYNQ_UART_CONTROL_RXDIS BSP_BIT32(3)
#define ZYNQ_UART_CONTROL_RXEN BSP_BIT32(2)
#define ZYNQ_UART_CONTROL_TXRES BSP_BIT32(1)
#define ZYNQ_UART_CONTROL_RXRES BSP_BIT32(0)
	uint32_t mode;
#define ZYNQ_UART_MODE_CHMODE(val) BSP_FLD32(val, 8, 9)
#define ZYNQ_UART_MODE_CHMODE_GET(reg) BSP_FLD32GET(reg, 8, 9)
#define ZYNQ_UART_MODE_CHMODE_SET(reg, val) BSP_FLD32SET(reg, val, 8, 9)
#define ZYNQ_UART_MODE_CHMODE_NORMAL 0x00U
#define ZYNQ_UART_MODE_CHMODE_AUTO_ECHO 0x01U
#define ZYNQ_UART_MODE_CHMODE_LOCAL_LOOPBACK 0x02U
#define ZYNQ_UART_MODE_CHMODE_REMOTE_LOOPBACK 0x03U
#define ZYNQ_UART_MODE_NBSTOP(val) BSP_FLD32(val, 6, 7)
#define ZYNQ_UART_MODE_NBSTOP_GET(reg) BSP_FLD32GET(reg, 6, 7)
#define ZYNQ_UART_MODE_NBSTOP_SET(reg, val) BSP_FLD32SET(reg, val, 6, 7)
#define ZYNQ_UART_MODE_NBSTOP_STOP_1 0x00U
#define ZYNQ_UART_MODE_NBSTOP_STOP_1_5 0x01U
#define ZYNQ_UART_MODE_NBSTOP_STOP_2 0x02U
#define ZYNQ_UART_MODE_PAR(val) BSP_FLD32(val, 3, 5)
#define ZYNQ_UART_MODE_PAR_GET(reg) BSP_FLD32GET(reg, 3, 5)
#define ZYNQ_UART_MODE_PAR_SET(reg, val) BSP_FLD32SET(reg, val, 3, 5)
#define ZYNQ_UART_MODE_PAR_EVEN 0x00U
#define ZYNQ_UART_MODE_PAR_ODD 0x01U
#define ZYNQ_UART_MODE_PAR_SPACE 0x02U
#define ZYNQ_UART_MODE_PAR_MARK 0x03U
#define ZYNQ_UART_MODE_PAR_NONE 0x04U
#define ZYNQ_UART_MODE_CHRL(val) BSP_FLD32(val, 1, 2)
#define ZYNQ_UART_MODE_CHRL_GET(reg) BSP_FLD32GET(reg, 1, 2)
#define ZYNQ_UART_MODE_CHRL_SET(reg, val) BSP_FLD32SET(reg, val, 1, 2)
#define ZYNQ_UART_MODE_CHRL_8 0x00U
#define ZYNQ_UART_MODE_CHRL_7 0x02U
#define ZYNQ_UART_MODE_CHRL_6 0x03U
#define ZYNQ_UART_MODE_CLKS BSP_BIT32(0)
	uint32_t irq_en;
	uint32_t irq_dis;
	uint32_t irq_mask;
	uint32_t irq_sts;
#define ZYNQ_UART_TOVR BSP_BIT32(12)
#define ZYNQ_UART_TNFUL BSP_BIT32(11)
#define ZYNQ_UART_TTRIG BSP_BIT32(10)
#define ZYNQ_UART_DMSI BSP_BIT32(9)
#define ZYNQ_UART_TIMEOUT BSP_BIT32(8)
#define ZYNQ_UART_PARE BSP_BIT32(7)
#define ZYNQ_UART_FRAME BSP_BIT32(6)
#define ZYNQ_UART_ROVR BSP_BIT32(5)
#define ZYNQ_UART_TFUL BSP_BIT32(4)
#define ZYNQ_UART_TEMPTY BSP_BIT32(3)
#define ZYNQ_UART_RFUL BSP_BIT32(2)
#define ZYNQ_UART_REMPTY BSP_BIT32(1)
#define ZYNQ_UART_RTRIG BSP_BIT32(0)
	uint32_t baud_rate_gen;
#define ZYNQ_UART_BAUD_RATE_GEN_CD(val) BSP_FLD32(val, 0, 15)
#define ZYNQ_UART_BAUD_RATE_GEN_CD_GET(reg) BSP_FLD32GET(reg, 0, 15)
#define ZYNQ_UART_BAUD_RATE_GEN_CD_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)
	uint32_t rx_timeout;
#define ZYNQ_UART_RX_TIMEOUT_RTO(val) BSP_FLD32(val, 0, 7)
#define ZYNQ_UART_RX_TIMEOUT_RTO_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define ZYNQ_UART_RX_TIMEOUT_RTO_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t rx_fifo_trg_lvl;
#define ZYNQ_UART_RX_FIFO_TRG_LVL_RTRIG(val) BSP_FLD32(val, 0, 5)
#define ZYNQ_UART_RX_FIFO_TRG_LVL_RTRIG_GET(reg) BSP_FLD32GET(reg, 0, 5)
#define ZYNQ_UART_RX_FIFO_TRG_LVL_RTRIG_SET(reg, val) BSP_FLD32SET(reg, val, 0, 5)
	uint32_t modem_ctrl;
#define ZYNQ_UART_MODEM_CTRL_FCM BSP_BIT32(5)
#define ZYNQ_UART_MODEM_CTRL_RTS BSP_BIT32(1)
#define ZYNQ_UART_MODEM_CTRL_DTR BSP_BIT32(0)
	uint32_t modem_sts;
#define ZYNQ_UART_MODEM_STS_FCMS BSP_BIT32(8)
#define ZYNQ_UART_MODEM_STS_DCD BSP_BIT32(7)
#define ZYNQ_UART_MODEM_STS_RI BSP_BIT32(6)
#define ZYNQ_UART_MODEM_STS_DSR BSP_BIT32(5)
#define ZYNQ_UART_MODEM_STS_CTS BSP_BIT32(4)
#define ZYNQ_UART_MODEM_STS_DDCD BSP_BIT32(3)
#define ZYNQ_UART_MODEM_STS_TERI BSP_BIT32(2)
#define ZYNQ_UART_MODEM_STS_DDSR BSP_BIT32(1)
#define ZYNQ_UART_MODEM_STS_DCTS BSP_BIT32(0)
	uint32_t channel_sts;
#define ZYNQ_UART_CHANNEL_STS_TNFUL BSP_BIT32(14)
#define ZYNQ_UART_CHANNEL_STS_TTRIG BSP_BIT32(13)
#define ZYNQ_UART_CHANNEL_STS_FDELT BSP_BIT32(12)
#define ZYNQ_UART_CHANNEL_STS_TACTIVE BSP_BIT32(11)
#define ZYNQ_UART_CHANNEL_STS_RACTIVE BSP_BIT32(10)
#define ZYNQ_UART_CHANNEL_STS_TFUL BSP_BIT32(4)
#define ZYNQ_UART_CHANNEL_STS_TEMPTY BSP_BIT32(3)
#define ZYNQ_UART_CHANNEL_STS_RFUL BSP_BIT32(2)
#define ZYNQ_UART_CHANNEL_STS_REMPTY BSP_BIT32(1)
#define ZYNQ_UART_CHANNEL_STS_RTRIG BSP_BIT32(0)
	uint32_t tx_rx_fifo;
#define ZYNQ_UART_TX_RX_FIFO_FIFO(val) BSP_FLD32(val, 0, 7)
#define ZYNQ_UART_TX_RX_FIFO_FIFO_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define ZYNQ_UART_TX_RX_FIFO_FIFO_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t baud_rate_div;
#define ZYNQ_UART_BAUD_RATE_DIV_BDIV(val) BSP_FLD32(val, 0, 7)
#define ZYNQ_UART_BAUD_RATE_DIV_BDIV_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define ZYNQ_UART_BAUD_RATE_DIV_BDIV_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t flow_delay;
#define ZYNQ_UART_FLOW_DELAY_FDEL(val) BSP_FLD32(val, 0, 5)
#define ZYNQ_UART_FLOW_DELAY_FDEL_GET(reg) BSP_FLD32GET(reg, 0, 5)
#define ZYNQ_UART_FLOW_DELAY_FDEL_SET(reg, val) BSP_FLD32SET(reg, val, 0, 5)
	uint32_t reserved_3c[2];
	uint32_t tx_fifo_trg_lvl;
#define ZYNQ_UART_TX_FIFO_TRG_LVL_TTRIG(val) BSP_FLD32(val, 0, 5)
#define ZYNQ_UART_TX_FIFO_TRG_LVL_TTRIG_GET(reg) BSP_FLD32GET(reg, 0, 5)
#define ZYNQ_UART_TX_FIFO_TRG_LVL_TTRIG_SET(reg, val) BSP_FLD32SET(reg, val, 0, 5)
} zynq_uart;

void zynq_uart_initialize(volatile zynq_uart *regs);

int zynq_uart_read_char_polled(volatile zynq_uart *regs);

void zynq_uart_write_char_polled(volatile zynq_uart *regs, char c);

/**
  * Flush TX FIFO and wait until it is empty. Used in bsp_reset.
  */
void zynq_uart_reset_tx_flush(volatile zynq_uart *regs);

/**
 * @brief Returns the Zynq UART input clock frequency in Hz.
 */
uint32_t zynq_uart_input_clock(void);

/**
 * @brief Calculates the clock and baud divisor of the best approximation of
 *   the desired baud.
 *
 * The function tries to yield a sample set around 16 per RX-bit.
 *
 * @param desired_baud is the desired baud for an Zynq UART device.
 *
 * @param mode_clks is the value of the CLKS bit of the Zynq UART mode register.
 *
 * @param cd_ptr[out] is a reference to an uint32_t object.  The function
 *   stores the calculated clock divisor to this object.
 *
 * @param bdiv_ptr[out] is a reference to an uint32_t object.  The function
 *   stores the calculated baud divisor to this object.
 *
 * @return Returns the absolute error of the calculated baud to the desired
 *   baud in Hz.
 */
uint32_t zynq_uart_calculate_baud(
  uint32_t  desired_baud,
  uint32_t  mode_clks,
  uint32_t *cd_ptr,
  uint32_t *bdiv_ptr
);

/** @} */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_UART_REGS_H */
