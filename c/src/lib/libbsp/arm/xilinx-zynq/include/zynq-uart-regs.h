/**
 * @file
 * @ingroup zynq_uart_regs
 * @brief UART register definitions.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/**
 * @defgroup zynq_uart_regs UART Register Definitions
 * @ingroup zynq_uart
 * @brief UART Register Definitions
 * @{
 */

#ifndef LIBBSP_ARM_XILINX_ZYNQ_UART_REGS_H
#define LIBBSP_ARM_XILINX_ZYNQ_UART_REGS_H

#include <bsp/utility.h>

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

/** @} */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_UART_REGS_H */
