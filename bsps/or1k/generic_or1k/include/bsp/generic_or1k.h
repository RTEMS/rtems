/**
 * @file
 *
 * @ingroup generic_or1k_reg
 *
 * @brief Register definitions.
 */

/*
 * COPYRIGHT (c) 2014-2015 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

#ifndef LIBBSP_GENERIC_OR1K_H
#define LIBBSP_GENERIC_OR1K_H

#include <stdint.h>

/**
 * @defgroup generic_or1k_reg Register Definitions
 *
 * @ingroup RTEMSBSPsOR1K
 *
 * @brief Shared register definitions for or1k systems.
 *
 * @{
 */

/**
 * @name Register Macros
 *
 * @{
 */

 #define OR1K_REG(x)           (*((volatile unsigned char *) (x)))
 #define OR1K_BIT(n)           (1 << (n))

/** @} */

/**
 * @name Internal OR1K UART Registers
 *
 * @{
 */
#define OR1K_BSP_CLOCK_FREQ       50000000UL
#define OR1K_BSP_UART_BASE        0x90000000

#define OR1K_BSP_UART_REG_TX              (OR1K_BSP_UART_BASE+0)
#define OR1K_BSP_UART_REG_RX              (OR1K_BSP_UART_BASE+0)
#define OR1K_BSP_UART_REG_DEV_LATCH_LOW   (OR1K_BSP_UART_BASE+0)
#define OR1K_BSP_UART_REG_DEV_LATCH_HIGH  (OR1K_BSP_UART_BASE+1)
#define OR1K_BSP_UART_REG_INT_ENABLE      (OR1K_BSP_UART_BASE+1)
#define OR1K_BSP_UART_REG_INT_ID          (OR1K_BSP_UART_BASE+2)
#define OR1K_BSP_UART_REG_FIFO_CTRL       (OR1K_BSP_UART_BASE+2)
#define OR1K_BSP_UART_REG_LINE_CTRL       (OR1K_BSP_UART_BASE+3)
#define OR1K_BSP_UART_REG_MODEM_CTRL      (OR1K_BSP_UART_BASE+4)
#define OR1K_BSP_UART_REG_LINE_STATUS     (OR1K_BSP_UART_BASE+5)
#define OR1K_BSP_UART_REG_MODEM_STATUS    (OR1K_BSP_UART_BASE+6)
#define OR1K_BSP_UART_REG_SCRATCH         (OR1K_BSP_UART_BASE+7)

/* FIFO Control Register */
#define OR1K_BSP_UART_REG_FIFO_CTRL_TRIGGER_1    (0x00)
#define OR1K_BSP_UART_REG_FIFO_CTRL_ENABLE_FIFO  (0x01)
#define OR1K_BSP_UART_REG_FIFO_CTRL_CLEAR_RCVR   (0x02)
#define OR1K_BSP_UART_REG_FIFO_CTRL_CLEAR_XMIT   (0x03)
#define OR1K_BSP_UART_REG_FIFO_CTRL_DMA_SELECT   (0x08)
#define OR1K_BSP_UART_REG_FIFO_CTRL_TRIGGER_4    (0x40)
#define OR1K_BSP_UART_REG_FIFO_CTRL_TRIGGER_8    (0x80)
#define OR1K_BSP_UART_REG_FIFO_CTRL_TRIGGER_14   (0xC0)
#define OR1K_BSP_UART_REG_FIFO_CTRL_TRIGGER_MASK (0xC0)

/* Line Control Register */
#define OR1K_BSP_UART_REG_LINE_CTRL_WLEN5  (0x00)
#define OR1K_BSP_UART_REG_LINE_CTRL_WLEN6  (0x01)
#define OR1K_BSP_UART_REG_LINE_CTRL_WLEN7  (0x02)
#define OR1K_BSP_UART_REG_LINE_CTRL_WLEN8  (0x03)
#define OR1K_BSP_UART_REG_LINE_CTRL_STOP   (0x04)
#define OR1K_BSP_UART_REG_LINE_CTRL_PARITY (0x08)
#define OR1K_BSP_UART_REG_LINE_CTRL_EPAR   (0x10)
#define OR1K_BSP_UART_REG_LINE_CTRL_SPAR   (0x20)
#define OR1K_BSP_UART_REG_LINE_CTRL_SBC    (0x40)
#define OR1K_BSP_UART_REG_LINE_CTRL_DLAB   (0x80)

/* Line Status Register */
#define OR1K_BSP_UART_REG_LINE_STATUS_DR   (0x01)
#define OR1K_BSP_UART_REG_LINE_STATUS_OE   (0x02)
#define OR1K_BSP_UART_REG_LINE_STATUS_PE   (0x04)
#define OR1K_BSP_UART_REG_LINE_STATUS_FE   (0x08)
#define OR1K_BSP_UART_REG_LINE_STATUS_BI   (0x10)
#define OR1K_BSP_UART_REG_LINE_STATUS_THRE (0x20)
#define OR1K_BSP_UART_REG_LINE_STATUS_TEMT (0x40)

/* Modem Control Register */
#define OR1K_BSP_UART_REG_MODEM_CTRL_DTR  (0x01)
#define OR1K_BSP_UART_REG_MODEM_CTRL_RTS  (0x02)
#define OR1K_BSP_UART_REG_MODEM_CTRL_OUT1 (0x04)
#define OR1K_BSP_UART_REG_MODEM_CTRL_OUT2 (0x08)
#define OR1K_BSP_UART_REG_MODEM_CTRL_LOOP (0x10)

/* Modem Status Register */
#define OR1K_BSP_UART_REG_MODEM_STATUS_DCTS (0x01)
#define OR1K_BSP_UART_REG_MODEM_STATUS_DDSR (0x02)
#define OR1K_BSP_UART_REG_MODEM_STATUS_TERI (0x04)
#define OR1K_BSP_UART_REG_MODEM_STATUS_DDCD (0x08)
#define OR1K_BSP_UART_REG_MODEM_STATUS_CTS  (0x10)
#define OR1K_BSP_UART_REG_MODEM_STATUS_DSR  (0x20)
#define OR1K_BSP_UART_REG_MODEM_STATUS_RI   (0x40)
#define OR1K_BSP_UART_REG_MODEM_STATUS_DCD  (0x80)
#define OR1K_BSP_UART_REG_MODEM_STATUS_ANY_DELTA (0x0F)

/** @} */

/** @} */

#endif /* LIBBSP_GENERIC_OR1K_H */
