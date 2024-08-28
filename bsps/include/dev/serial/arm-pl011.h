/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup RTEMSBSPsARMShared
 *
 *  @brief ARM PL011 Support Package
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_SHARED_ARM_PL011_H
#define LIBBSP_ARM_SHARED_ARM_PL011_H

#include <rtems/termiostypes.h>

#include <dev/serial/arm-pl011-regs.h>
#include <bspopts.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ARM PL011 Support
 * @ingroup RTEMSBSPsARMShared
 * @brief UART Support
 */

typedef struct {
  pl011_base base;
  uint32_t reserved_4c[997];
  uint32_t uartperiphid0;
  uint32_t uartperiphid1;
  uint32_t uartperiphid2;
  uint32_t uartperiphid3;
  uint32_t uartpcellid0;
  uint32_t uartpcellid1;
  uint32_t uartpcellid2;
  uint32_t uartpcellid3;
} arm_pl011_uart;

typedef struct {
  rtems_termios_device_context base;
  volatile arm_pl011_uart *regs;
  rtems_vector_number irq;
  uint32_t initial_baud;
  uint32_t clock;

#ifdef BSP_CONSOLE_USE_INTERRUPTS
    /*
     * Due to HW limitation, the first TX interrupt should be triggered by the
     * software. This is because TX interrupts are based on transition through
     * a level, rather than on the level itself. When the UART interrupt and
     * UART is enabled before any data is written to the TXFIFO, the interrupt
     * is not set. The interrupt is only set once the TXFIFO becomes empty
     * after being filled to the trigger level. Until then, this flag variable
     * ensures that the interrupt handler is software triggered.
     */
    bool needs_sw_triggered_tx_irq;

    int tx_queued_chars;
    rtems_termios_tty* tty;
#endif
} arm_pl011_context;

#ifdef BSP_CONSOLE_USE_INTERRUPTS

#define FIFO_LEVEL_ONE_EIGHTH   0
#define FIFO_LEVEL_ONE_FOURTH   1
#define FIFO_LEVEL_ONE_HALF     2
#define FIFO_LEVEL_THREE_FOURTH 3
#define FIFO_LELEL_SEVEN_HALF   4

#define TXFIFO_IRQ_TRIGGER_LEVEL FIFO_LEVEL_ONE_EIGHTH
#define RXFIFO_IRQ_TRIGGER_LEVEL FIFO_LEVEL_ONE_HALF

#endif

extern const rtems_termios_device_handler arm_pl011_fns;

/**
 * @brief Initialize pl011 registers.
 *
 * @param base Termios device context.
 *
 * @retval true Successfully initialized pl011 registers.
 */
bool arm_pl011_probe(rtems_termios_device_context *base);

/**
 * @brief Write a char using polling mode.
 *
 * @param base Termios device context.
 * @param c Character to be written.
 */
void arm_pl011_write_polled(rtems_termios_device_context *base, char c);

/**
 * @brief Read a char using polling mode.
 *
 * @param base Termios device context.
 *
 * @retval Character read.
 * @retval -1 rxfifo is empty.
 */
int arm_pl011_read_polled(rtems_termios_device_context *base);

/**
 * @brief Get register base address.
 *
 * @param base Termios device context.
 *
 * @retval Register base address.
 */
volatile arm_pl011_uart *arm_pl011_get_regs(rtems_termios_device_context *base);

/**
 * @brief Compute baudrate params.
 *
 * @param ibrd The integer part of the baud rate divisor value.
 * @param fbrd The fractional part of the baud rate divisor value.
 * @param baudrate Termios device context.
 * @param clock The UART reference clock frequency.
 * @param max_error Maximum error.
 *
 * @retval 0 Success.
 * @retval 1 The error is too large.
 * @retval 2 ibrd is invalid.
 */
int arm_pl011_compute_baudrate_params(
  uint32_t *ibrd,
  uint32_t *fbrd,
  const uint32_t baudrate,
  const uint32_t clock,
  const unsigned short max_error
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_PL011_H */
