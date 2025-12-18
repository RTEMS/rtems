/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMEFM32GG11
 *
 * @brief EFM32GG11 Console Driver
 */

/*
 * Copyright (C) 2025 Allan N. Hessenflow
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

#include <bsp.h>
#include <bsp/processor.h>
#include <bsp/efm32gg11_clock.h>
#include <bsp/efm32gg11_gpio.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>


#define PASTE2(a,b)             a ## b
#define PASTE3(a,b,c)           a ## b ## c
#define PASTE4(a,b,c,d)         a ## b ## c ## d

#define REGS(t, i)              PASTE2(t, i)
#define TX_IRQN(t, i)           PASTE3(t, i, _TX_IRQn)
#define RX_IRQN(t, i)           PASTE3(t, i, _RX_IRQn)
#define TX_PORT(t, i, l)        PASTE4(AF_, t, i, _TX_PORT)(l)
#define TX_PIN(t, i, l)         PASTE4(AF_, t, i, _TX_PIN)(l)
#define RX_PORT(t, i, l)        PASTE4(AF_, t, i, _RX_PORT)(l)
#define RX_PIN(t, i, l)         PASTE4(AF_, t, i, _RX_PIN)(l)
#define CTS_PORT(t, i, l)       PASTE4(AF_, t, i, _CTS_PORT)(l)
#define CTS_PIN(t, i, l)        PASTE4(AF_, t, i, _CTS_PIN)(l)
#define RTS_PORT(t, i, l)       PASTE4(AF_, t, i, _RTS_PORT)(l)
#define RTS_PIN(t, i, l)        PASTE4(AF_, t, i, _RTS_PIN)(l)
#define CS_PORT(t, i,  l)       PASTE4(AF_, t, i, _CS_PORT)(l)
#define CS_PIN(t, i, l )        PASTE4(AF_, t, i, _CS_PIN)(l)
#define HFPERCLKEN0_MASK(t, i)  PASTE3(CMU_HFPERCLKEN0_, t, i)


static struct usart_context_s {
  rtems_termios_device_context base; /* must be first */
  const char *name;
  USART_TypeDef *regs;
  rtems_vector_number rx_irq;
  rtems_vector_number tx_irq;
  int tx_pin_loc;
  int tx_port;
  int tx_pin;
  int rx_pin_loc;
  int rx_port;
  int rx_pin;
  int cts_pin_loc;
  int cts_port;
  int cts_pin;
  int rts_pin_loc;
  int rts_port;
  int rts_pin;
  int cs_pin_loc;
  int cs_port;
  int cs_pin;
  uint32_t ctrl;
  uint32_t timing;
  uint32_t hfperclken0_mask;
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  bool transmitting;
#endif
} usart_instances[] = {
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("console"),
    .name = CONSOLE_DEVICE_NAME,
    .regs = REGS(EFM32GG11_CONSOLE_TYPE, EFM32GG11_CONSOLE_DEVICE_INDEX),
    .rx_irq = RX_IRQN(EFM32GG11_CONSOLE_TYPE, EFM32GG11_CONSOLE_DEVICE_INDEX),
    .tx_irq = TX_IRQN(EFM32GG11_CONSOLE_TYPE, EFM32GG11_CONSOLE_DEVICE_INDEX),
    .tx_pin_loc = EFM32GG11_CONSOLE_TX_LOC,
    .tx_port = TX_PORT(EFM32GG11_CONSOLE_TYPE,
                       EFM32GG11_CONSOLE_DEVICE_INDEX,
                       EFM32GG11_CONSOLE_TX_LOC),
    .tx_pin = TX_PIN(EFM32GG11_CONSOLE_TYPE,
                     EFM32GG11_CONSOLE_DEVICE_INDEX,
                     EFM32GG11_CONSOLE_TX_LOC),
    .rx_pin_loc = EFM32GG11_CONSOLE_RX_LOC,
    .rx_port = RX_PORT(EFM32GG11_CONSOLE_TYPE,
                       EFM32GG11_CONSOLE_DEVICE_INDEX,
                       EFM32GG11_CONSOLE_RX_LOC),
    .rx_pin = RX_PIN(EFM32GG11_CONSOLE_TYPE,
                     EFM32GG11_CONSOLE_DEVICE_INDEX,
                     EFM32GG11_CONSOLE_RX_LOC),
    .cts_pin_loc = EFM32GG11_CONSOLE_CTS_LOC,
    .cts_port = CTS_PORT(EFM32GG11_CONSOLE_TYPE,
                         EFM32GG11_CONSOLE_DEVICE_INDEX,
                         EFM32GG11_CONSOLE_CTS_LOC),
    .cts_pin = CTS_PIN(EFM32GG11_CONSOLE_TYPE,
                       EFM32GG11_CONSOLE_DEVICE_INDEX,
                       EFM32GG11_CONSOLE_CTS_LOC),
    .rts_pin_loc = EFM32GG11_CONSOLE_RTS_LOC,
    .rts_port = RTS_PORT(EFM32GG11_CONSOLE_TYPE,
                         EFM32GG11_CONSOLE_DEVICE_INDEX,
                         EFM32GG11_CONSOLE_RTS_LOC),
    .rts_pin = RTS_PIN(EFM32GG11_CONSOLE_TYPE,
                       EFM32GG11_CONSOLE_DEVICE_INDEX,
                       EFM32GG11_CONSOLE_RTS_LOC),
    .cs_pin_loc = EFM32GG11_CONSOLE_CS_LOC,
    .cs_port = CS_PORT(EFM32GG11_CONSOLE_TYPE,
                       EFM32GG11_CONSOLE_DEVICE_INDEX,
                       EFM32GG11_CONSOLE_CS_LOC),
    .cs_pin = CS_PIN(EFM32GG11_CONSOLE_TYPE,
                     EFM32GG11_CONSOLE_DEVICE_INDEX,
                     EFM32GG11_CONSOLE_CS_LOC),
    .ctrl = EFM32GG11_CONSOLE_CTRL,
    .timing = EFM32GG11_CONSOLE_TIMING,
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_CONSOLE_TYPE,
                                         EFM32GG11_CONSOLE_DEVICE_INDEX)
  },
#if defined(EFM32GG11_TTYS1_DEVICE_INDEX) && EFM32GG11_TTYS1_DEVICE_INDEX >= 0
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("ttyS1"),
    .name = "/dev/ttyS1",
    .regs = REGS(EFM32GG11_TTYS1_TYPE, EFM32GG11_TTYS1_DEVICE_INDEX),
    .rx_irq = RX_IRQN(EFM32GG11_TTYS1_TYPE, EFM32GG11_TTYS1_DEVICE_INDEX),
    .tx_irq = TX_IRQN(EFM32GG11_TTYS1_TYPE, EFM32GG11_TTYS1_DEVICE_INDEX),
    .tx_pin_loc = EFM32GG11_TTYS1_TX_LOC,
    .tx_port = TX_PORT(EFM32GG11_TTYS1_TYPE,
                       EFM32GG11_TTYS1_DEVICE_INDEX,
                       EFM32GG11_TTYS1_TX_LOC),
    .tx_pin = TX_PIN(EFM32GG11_TTYS1_TYPE,
                     EFM32GG11_TTYS1_DEVICE_INDEX,
                     EFM32GG11_TTYS1_TX_LOC),
    .rx_pin_loc = EFM32GG11_TTYS1_RX_LOC,
    .rx_port = RX_PORT(EFM32GG11_TTYS1_TYPE,
                       EFM32GG11_TTYS1_DEVICE_INDEX,
                       EFM32GG11_TTYS1_RX_LOC),
    .rx_pin = RX_PIN(EFM32GG11_TTYS1_TYPE,
                     EFM32GG11_TTYS1_DEVICE_INDEX,
                     EFM32GG11_TTYS1_RX_LOC),
    .cts_pin_loc = EFM32GG11_TTYS1_CTS_LOC,
    .cts_port = CTS_PORT(EFM32GG11_TTYS1_TYPE,
                         EFM32GG11_TTYS1_DEVICE_INDEX,
                         EFM32GG11_TTYS1_CTS_LOC),
    .cts_pin = CTS_PIN(EFM32GG11_TTYS1_TYPE,
                       EFM32GG11_TTYS1_DEVICE_INDEX,
                       EFM32GG11_TTYS1_CTS_LOC),
    .rts_pin_loc = EFM32GG11_TTYS1_RTS_LOC,
    .rts_port = RTS_PORT(EFM32GG11_TTYS1_TYPE,
                         EFM32GG11_TTYS1_DEVICE_INDEX,
                         EFM32GG11_TTYS1_RTS_LOC),
    .rts_pin = RTS_PIN(EFM32GG11_TTYS1_TYPE,
                       EFM32GG11_TTYS1_DEVICE_INDEX,
                       EFM32GG11_TTYS1_RTS_LOC),
    .cs_pin_loc = EFM32GG11_TTYS1_CS_LOC,
    .cs_port = CS_PORT(EFM32GG11_TTYS1_TYPE,
                       EFM32GG11_TTYS1_DEVICE_INDEX,
                       EFM32GG11_TTYS1_CS_LOC),
    .cs_pin = CS_PIN(EFM32GG11_TTYS1_TYPE,
                     EFM32GG11_TTYS1_DEVICE_INDEX,
                     EFM32GG11_TTYS1_CS_LOC),
    .ctrl = EFM32GG11_TTYS1_CTRL,
    .timing = EFM32GG11_TTYS1_TIMING,
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_TTYS1_TYPE,
                                         EFM32GG11_TTYS1_DEVICE_INDEX)
  },
#endif
#if defined(EFM32GG11_TTYS2_DEVICE_INDEX) && EFM32GG11_TTYS2_DEVICE_INDEX >= 0
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("ttyS2"),
    .name = "/dev/ttyS2",
    .regs = REGS(EFM32GG11_TTYS2_TYPE, EFM32GG11_TTYS2_DEVICE_INDEX),
    .rx_irq = RX_IRQN(EFM32GG11_TTYS2_TYPE, EFM32GG11_TTYS2_DEVICE_INDEX),
    .tx_irq = TX_IRQN(EFM32GG11_TTYS2_TYPE, EFM32GG11_TTYS2_DEVICE_INDEX),
    .tx_pin_loc = EFM32GG11_TTYS2_TX_LOC,
    .tx_port = TX_PORT(EFM32GG11_TTYS2_TYPE,
                       EFM32GG11_TTYS2_DEVICE_INDEX,
                       EFM32GG11_TTYS2_TX_LOC),
    .tx_pin = TX_PIN(EFM32GG11_TTYS2_TYPE,
                     EFM32GG11_TTYS2_DEVICE_INDEX,
                     EFM32GG11_TTYS2_TX_LOC),
    .rx_pin_loc = EFM32GG11_TTYS2_RX_LOC,
    .rx_port = RX_PORT(EFM32GG11_TTYS2_TYPE,
                       EFM32GG11_TTYS2_DEVICE_INDEX,
                       EFM32GG11_TTYS2_RX_LOC),
    .rx_pin = RX_PIN(EFM32GG11_TTYS2_TYPE,
                     EFM32GG11_TTYS2_DEVICE_INDEX,
                     EFM32GG11_TTYS2_RX_LOC),
    .cts_pin_loc = EFM32GG11_TTYS2_CTS_LOC,
    .cts_port = CTS_PORT(EFM32GG11_TTYS2_TYPE,
                         EFM32GG11_TTYS2_DEVICE_INDEX,
                         EFM32GG11_TTYS2_CTS_LOC),
    .cts_pin = CTS_PIN(EFM32GG11_TTYS2_TYPE,
                       EFM32GG11_TTYS2_DEVICE_INDEX,
                       EFM32GG11_TTYS2_CTS_LOC),
    .rts_pin_loc = EFM32GG11_TTYS2_RTS_LOC,
    .rts_port = RTS_PORT(EFM32GG11_TTYS2_TYPE,
                         EFM32GG11_TTYS2_DEVICE_INDEX,
                         EFM32GG11_TTYS2_RTS_LOC),
    .rts_pin = RTS_PIN(EFM32GG11_TTYS2_TYPE,
                       EFM32GG11_TTYS2_DEVICE_INDEX,
                       EFM32GG11_TTYS2_RTS_LOC),
    .cs_pin_loc = EFM32GG11_TTYS2_CS_LOC,
    .cs_port = CS_PORT(EFM32GG11_TTYS2_TYPE,
                       EFM32GG11_TTYS2_DEVICE_INDEX,
                       EFM32GG11_TTYS2_CS_LOC),
    .cs_pin = CS_PIN(EFM32GG11_TTYS2_TYPE,
                     EFM32GG11_TTYS2_DEVICE_INDEX,
                     EFM32GG11_TTYS2_CS_LOC),
    .ctrl = EFM32GG11_TTYS2_CTRL,
    .timing = EFM32GG11_TTYS2_TIMING,
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_TTYS2_TYPE,
                                         EFM32GG11_TTYS2_DEVICE_INDEX)
  },
#endif
#if defined(EFM32GG11_TTYS3_DEVICE_INDEX) && EFM32GG11_TTYS3_DEVICE_INDEX >= 0
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("ttyS3"),
    .name = "/dev/ttyS3",
    .regs = REGS(EFM32GG11_TTYS3_TYPE, EFM32GG11_TTYS3_DEVICE_INDEX),
    .rx_irq = RX_IRQN(EFM32GG11_TTYS3_TYPE, EFM32GG11_TTYS3_DEVICE_INDEX),
    .tx_irq = TX_IRQN(EFM32GG11_TTYS3_TYPE, EFM32GG11_TTYS3_DEVICE_INDEX),
    .tx_pin_loc = EFM32GG11_TTYS3_TX_LOC,
    .tx_port = TX_PORT(EFM32GG11_TTYS3_TYPE,
                       EFM32GG11_TTYS3_DEVICE_INDEX,
                       EFM32GG11_TTYS3_TX_LOC),
    .tx_pin = TX_PIN(EFM32GG11_TTYS3_TYPE,
                     EFM32GG11_TTYS3_DEVICE_INDEX,
                     EFM32GG11_TTYS3_TX_LOC),
    .rx_pin_loc = EFM32GG11_TTYS3_RX_LOC,
    .rx_port = RX_PORT(EFM32GG11_TTYS3_TYPE,
                       EFM32GG11_TTYS3_DEVICE_INDEX,
                       EFM32GG11_TTYS3_RX_LOC),
    .rx_pin = RX_PIN(EFM32GG11_TTYS3_TYPE,
                     EFM32GG11_TTYS3_DEVICE_INDEX,
                     EFM32GG11_TTYS3_RX_LOC),
    .cts_pin_loc = EFM32GG11_TTYS3_CTS_LOC,
    .cts_port = CTS_PORT(EFM32GG11_TTYS3_TYPE,
                         EFM32GG11_TTYS3_DEVICE_INDEX,
                         EFM32GG11_TTYS3_CTS_LOC),
    .cts_pin = CTS_PIN(EFM32GG11_TTYS3_TYPE,
                       EFM32GG11_TTYS3_DEVICE_INDEX,
                       EFM32GG11_TTYS3_CTS_LOC),
    .rts_pin_loc = EFM32GG11_TTYS3_RTS_LOC,
    .rts_port = RTS_PORT(EFM32GG11_TTYS3_TYPE,
                         EFM32GG11_TTYS3_DEVICE_INDEX,
                         EFM32GG11_TTYS3_RTS_LOC),
    .rts_pin = RTS_PIN(EFM32GG11_TTYS3_TYPE,
                       EFM32GG11_TTYS3_DEVICE_INDEX,
                       EFM32GG11_TTYS3_RTS_LOC),
    .cs_pin_loc = EFM32GG11_TTYS3_CS_LOC,
    .cs_port = CS_PORT(EFM32GG11_TTYS3_TYPE,
                       EFM32GG11_TTYS3_DEVICE_INDEX,
                       EFM32GG11_TTYS3_CS_LOC),
    .cs_pin = CS_PIN(EFM32GG11_TTYS3_TYPE,
                     EFM32GG11_TTYS3_DEVICE_INDEX,
                     EFM32GG11_TTYS3_CS_LOC),
    .ctrl = EFM32GG11_TTYS3_CTRL,
    .timing = EFM32GG11_TTYS3_TIMING,
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_TTYS3_TYPE,
                                         EFM32GG11_TTYS3_DEVICE_INDEX)
  },
#endif
#if defined(EFM32GG11_TTYS4_DEVICE_INDEX) && EFM32GG11_TTYS4_DEVICE_INDEX >= 0
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("ttyS4"),
    .name = "/dev/ttyS4",
    .regs = REGS(EFM32GG11_TTYS4_TYPE, EFM32GG11_TTYS4_DEVICE_INDEX),
    .rx_irq = RX_IRQN(EFM32GG11_TTYS4_TYPE, EFM32GG11_TTYS4_DEVICE_INDEX),
    .tx_irq = TX_IRQN(EFM32GG11_TTYS4_TYPE, EFM32GG11_TTYS4_DEVICE_INDEX),
    .tx_pin_loc = EFM32GG11_TTYS4_TX_LOC,
    .tx_port = TX_PORT(EFM32GG11_TTYS4_TYPE,
                       EFM32GG11_TTYS4_DEVICE_INDEX,
                       EFM32GG11_TTYS4_TX_LOC),
    .tx_pin = TX_PIN(EFM32GG11_TTYS4_TYPE,
                     EFM32GG11_TTYS4_DEVICE_INDEX,
                     EFM32GG11_TTYS4_TX_LOC),
    .rx_pin_loc = EFM32GG11_TTYS4_RX_LOC,
    .rx_port = RX_PORT(EFM32GG11_TTYS4_TYPE,
                       EFM32GG11_TTYS4_DEVICE_INDEX,
                       EFM32GG11_TTYS4_RX_LOC),
    .rx_pin = RX_PIN(EFM32GG11_TTYS4_TYPE,
                     EFM32GG11_TTYS4_DEVICE_INDEX,
                     EFM32GG11_TTYS4_RX_LOC),
    .cts_pin_loc = EFM32GG11_TTYS4_CTS_LOC,
    .cts_port = CTS_PORT(EFM32GG11_TTYS4_TYPE,
                         EFM32GG11_TTYS4_DEVICE_INDEX,
                         EFM32GG11_TTYS4_CTS_LOC),
    .cts_pin = CTS_PIN(EFM32GG11_TTYS4_TYPE,
                       EFM32GG11_TTYS4_DEVICE_INDEX,
                       EFM32GG11_TTYS4_CTS_LOC),
    .rts_pin_loc = EFM32GG11_TTYS4_RTS_LOC,
    .rts_port = RTS_PORT(EFM32GG11_TTYS4_TYPE,
                         EFM32GG11_TTYS4_DEVICE_INDEX,
                         EFM32GG11_TTYS4_RTS_LOC),
    .rts_pin = RTS_PIN(EFM32GG11_TTYS4_TYPE,
                       EFM32GG11_TTYS4_DEVICE_INDEX,
                       EFM32GG11_TTYS4_RTS_LOC),
    .cs_pin_loc = EFM32GG11_TTYS4_CS_LOC,
    .cs_port = CS_PORT(EFM32GG11_TTYS4_TYPE,
                       EFM32GG11_TTYS4_DEVICE_INDEX,
                       EFM32GG11_TTYS4_CS_LOC),
    .cs_pin = CS_PIN(EFM32GG11_TTYS4_TYPE,
                     EFM32GG11_TTYS4_DEVICE_INDEX,
                     EFM32GG11_TTYS4_CS_LOC),
    .ctrl = EFM32GG11_TTYS4_CTRL,
    .timing = EFM32GG11_TTYS4_TIMING,
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_TTYS4_TYPE,
                                         EFM32GG11_TTYS4_DEVICE_INDEX)
  },
#endif
#if defined(EFM32GG11_TTYS5_DEVICE_INDEX) && EFM32GG11_TTYS5_DEVICE_INDEX >= 0
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("ttyS5"),
    .name = "/dev/ttyS5",
    .regs = REGS(EFM32GG11_TTYS5_TYPE, EFM32GG11_TTYS5_DEVICE_INDEX),
    .rx_irq = RX_IRQN(EFM32GG11_TTYS5_TYPE, EFM32GG11_TTYS5_DEVICE_INDEX),
    .tx_irq = TX_IRQN(EFM32GG11_TTYS5_TYPE, EFM32GG11_TTYS5_DEVICE_INDEX),
    .tx_pin_loc = EFM32GG11_TTYS5_TX_LOC,
    .tx_port = TX_PORT(EFM32GG11_TTYS5_TYPE,
                       EFM32GG11_TTYS5_DEVICE_INDEX,
                       EFM32GG11_TTYS5_TX_LOC),
    .tx_pin = TX_PIN(EFM32GG11_TTYS5_TYPE,
                     EFM32GG11_TTYS5_DEVICE_INDEX,
                     EFM32GG11_TTYS5_TX_LOC),
    .rx_pin_loc = EFM32GG11_TTYS5_RX_LOC,
    .rx_port = RX_PORT(EFM32GG11_TTYS5_TYPE,
                       EFM32GG11_TTYS5_DEVICE_INDEX,
                       EFM32GG11_TTYS5_RX_LOC),
    .rx_pin = RX_PIN(EFM32GG11_TTYS5_TYPE,
                     EFM32GG11_TTYS5_DEVICE_INDEX,
                     EFM32GG11_TTYS5_RX_LOC),
    .cts_pin_loc = EFM32GG11_TTYS5_CTS_LOC,
    .cts_port = CTS_PORT(EFM32GG11_TTYS5_TYPE,
                         EFM32GG11_TTYS5_DEVICE_INDEX,
                         EFM32GG11_TTYS5_CTS_LOC),
    .cts_pin = CTS_PIN(EFM32GG11_TTYS5_TYPE,
                       EFM32GG11_TTYS5_DEVICE_INDEX,
                       EFM32GG11_TTYS5_CTS_LOC),
    .rts_pin_loc = EFM32GG11_TTYS5_RTS_LOC,
    .rts_port = RTS_PORT(EFM32GG11_TTYS5_TYPE,
                         EFM32GG11_TTYS5_DEVICE_INDEX,
                         EFM32GG11_TTYS5_RTS_LOC),
    .rts_pin = RTS_PIN(EFM32GG11_TTYS5_TYPE,
                       EFM32GG11_TTYS5_DEVICE_INDEX,
                       EFM32GG11_TTYS5_RTS_LOC),
    .cs_pin_loc = EFM32GG11_TTYS5_CS_LOC,
    .cs_port = CS_PORT(EFM32GG11_TTYS5_TYPE,
                       EFM32GG11_TTYS5_DEVICE_INDEX,
                       EFM32GG11_TTYS5_CS_LOC),
    .cs_pin = CS_PIN(EFM32GG11_TTYS5_TYPE,
                     EFM32GG11_TTYS5_DEVICE_INDEX,
                     EFM32GG11_TTYS5_CS_LOC),
    .ctrl = EFM32GG11_TTYS5_CTRL,
    .timing = EFM32GG11_TTYS5_TIMING,
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_TTYS5_TYPE,
                                         EFM32GG11_TTYS5_DEVICE_INDEX)
  },
#endif
#if defined(EFM32GG11_TTYS6_DEVICE_INDEX) && EFM32GG11_TTYS6_DEVICE_INDEX >= 0
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("ttyS6"),
    .name = "/dev/ttyS6",
    .regs = REGS(EFM32GG11_TTYS6_TYPE, EFM32GG11_TTYS6_DEVICE_INDEX),
    .rx_irq = RX_IRQN(EFM32GG11_TTYS6_TYPE, EFM32GG11_TTYS6_DEVICE_INDEX),
    .tx_irq = TX_IRQN(EFM32GG11_TTYS6_TYPE, EFM32GG11_TTYS6_DEVICE_INDEX),
    .tx_pin_loc = EFM32GG11_TTYS6_TX_LOC,
    .tx_port = TX_PORT(EFM32GG11_TTYS6_TYPE,
                       EFM32GG11_TTYS6_DEVICE_INDEX,
                       EFM32GG11_TTYS6_TX_LOC),
    .tx_pin = TX_PIN(EFM32GG11_TTYS6_TYPE,
                     EFM32GG11_TTYS6_DEVICE_INDEX,
                     EFM32GG11_TTYS6_TX_LOC),
    .rx_pin_loc = EFM32GG11_TTYS6_RX_LOC,
    .rx_port = RX_PORT(EFM32GG11_TTYS6_TYPE,
                       EFM32GG11_TTYS6_DEVICE_INDEX,
                       EFM32GG11_TTYS6_RX_LOC),
    .rx_pin = RX_PIN(EFM32GG11_TTYS6_TYPE,
                     EFM32GG11_TTYS6_DEVICE_INDEX,
                     EFM32GG11_TTYS6_RX_LOC),
    .cts_pin_loc = EFM32GG11_TTYS6_CTS_LOC,
    .cts_port = CTS_PORT(EFM32GG11_TTYS6_TYPE,
                         EFM32GG11_TTYS6_DEVICE_INDEX,
                         EFM32GG11_TTYS6_CTS_LOC),
    .cts_pin = CTS_PIN(EFM32GG11_TTYS6_TYPE,
                       EFM32GG11_TTYS6_DEVICE_INDEX,
                       EFM32GG11_TTYS6_CTS_LOC),
    .rts_pin_loc = EFM32GG11_TTYS6_RTS_LOC,
    .rts_port = RTS_PORT(EFM32GG11_TTYS6_TYPE,
                         EFM32GG11_TTYS6_DEVICE_INDEX,
                         EFM32GG11_TTYS6_RTS_LOC),
    .rts_pin = RTS_PIN(EFM32GG11_TTYS6_TYPE,
                       EFM32GG11_TTYS6_DEVICE_INDEX,
                       EFM32GG11_TTYS6_RTS_LOC),
    .cs_pin_loc = EFM32GG11_TTYS6_CS_LOC,
    .cs_port = CS_PORT(EFM32GG11_TTYS6_TYPE,
                       EFM32GG11_TTYS6_DEVICE_INDEX,
                       EFM32GG11_TTYS6_CS_LOC),
    .cs_pin = CS_PIN(EFM32GG11_TTYS6_TYPE,
                     EFM32GG11_TTYS6_DEVICE_INDEX,
                     EFM32GG11_TTYS6_CS_LOC),
    .ctrl = EFM32GG11_TTYS6_CTRL,
    .timing = EFM32GG11_TTYS6_TIMING,
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_TTYS6_TYPE,
                                         EFM32GG11_TTYS6_DEVICE_INDEX)
  },
#endif
#if defined(EFM32GG11_TTYS7_DEVICE_INDEX) && EFM32GG11_TTYS7_DEVICE_INDEX >= 0
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("ttyS7"),
    .name = "/dev/ttyS7",
    .regs = REGS(EFM32GG11_TTYS7_TYPE, EFM32GG11_TTYS7_DEVICE_INDEX),
    .rx_irq = RX_IRQN(EFM32GG11_TTYS7_TYPE, EFM32GG11_TTYS7_DEVICE_INDEX),
    .tx_irq = TX_IRQN(EFM32GG11_TTYS7_TYPE, EFM32GG11_TTYS7_DEVICE_INDEX),
    .tx_pin_loc = EFM32GG11_TTYS7_TX_LOC,
    .tx_port = TX_PORT(EFM32GG11_TTYS7_TYPE,
                       EFM32GG11_TTYS7_DEVICE_INDEX,
                       EFM32GG11_TTYS7_TX_LOC),
    .tx_pin = TX_PIN(EFM32GG11_TTYS7_TYPE,
                     EFM32GG11_TTYS7_DEVICE_INDEX,
                     EFM32GG11_TTYS7_TX_LOC),
    .rx_pin_loc = EFM32GG11_TTYS7_RX_LOC,
    .rx_port = RX_PORT(EFM32GG11_TTYS7_TYPE,
                       EFM32GG11_TTYS7_DEVICE_INDEX,
                       EFM32GG11_TTYS7_RX_LOC),
    .rx_pin = RX_PIN(EFM32GG11_TTYS7_TYPE,
                     EFM32GG11_TTYS7_DEVICE_INDEX,
                     EFM32GG11_TTYS7_RX_LOC),
    .cts_pin_loc = EFM32GG11_TTYS7_CTS_LOC,
    .cts_port = CTS_PORT(EFM32GG11_TTYS7_TYPE,
                         EFM32GG11_TTYS7_DEVICE_INDEX,
                         EFM32GG11_TTYS7_CTS_LOC),
    .cts_pin = CTS_PIN(EFM32GG11_TTYS7_TYPE,
                       EFM32GG11_TTYS7_DEVICE_INDEX,
                       EFM32GG11_TTYS7_CTS_LOC),
    .rts_pin_loc = EFM32GG11_TTYS7_RTS_LOC,
    .rts_port = RTS_PORT(EFM32GG11_TTYS7_TYPE,
                         EFM32GG11_TTYS7_DEVICE_INDEX,
                         EFM32GG11_TTYS7_RTS_LOC),
    .rts_pin = RTS_PIN(EFM32GG11_TTYS7_TYPE,
                       EFM32GG11_TTYS7_DEVICE_INDEX,
                       EFM32GG11_TTYS7_RTS_LOC),
    .cs_pin_loc = EFM32GG11_TTYS7_CS_LOC,
    .cs_port = CS_PORT(EFM32GG11_TTYS7_TYPE,
                       EFM32GG11_TTYS7_DEVICE_INDEX,
                       EFM32GG11_TTYS7_CS_LOC),
    .cs_pin = CS_PIN(EFM32GG11_TTYS7_TYPE,
                     EFM32GG11_TTYS7_DEVICE_INDEX,
                     EFM32GG11_TTYS7_CS_LOC),
    .ctrl = EFM32GG11_TTYS7_CTRL,
    .timing = EFM32GG11_TTYS7_TIMING,
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_TTYS7_TYPE,
                                         EFM32GG11_TTYS7_DEVICE_INDEX)
  },
#endif
};


#ifdef BSP_CONSOLE_USE_INTERRUPTS
static void usart_rx_interrupt(void *arg)
{
  rtems_termios_tty *tty = (rtems_termios_tty *) arg;
  struct usart_context_s *ctx = rtems_termios_get_device_context(tty);
  USART_TypeDef *regs = ctx->regs;

  while (regs->STATUS & USART_STATUS_RXDATAV)
  {
    char data = regs->RXDATA;
    rtems_termios_enqueue_raw_characters(tty, &data, sizeof(data));
  }
}

static void usart_tx_interrupt(void *arg)
{
  rtems_termios_tty *tty = (rtems_termios_tty *) arg;
  struct usart_context_s *ctx = rtems_termios_get_device_context(tty);
  USART_TypeDef *regs = ctx->regs;

  while (ctx->transmitting && (regs->STATUS & USART_STATUS_TXBL))
    rtems_termios_dequeue_characters(tty, 1);
}
#endif /* BSP_CONSOLE_USE_INTERRUPTS */

static void attribs_init(
  struct usart_context_s *ctx,
  rtems_termios_baud_t baud,
  tcflag_t c_cflag)
{
  USART_TypeDef *regs = ctx->regs;
  uint32_t r;

  r = (regs == USART2) ? efm32gg11_clock.hfperbclk : efm32gg11_clock.hfperclk;
  r = (16 * r + baud / 2) / baud - 256;
  /* There seems to be a discrepancy of a factor of 8 between rev 1.2 of the
     EFM32 Giant Gecko 11 Family Reference Manual and reality. */
  r = (r + 4) / 8;

  r <<= _USART_CLKDIV_DIV_SHIFT;
  regs->CLKDIV = r;

  switch (c_cflag & CSIZE) {
  case CS5:
    r = USART_FRAME_DATABITS_FIVE;
    break;
  case CS6:
    r = USART_FRAME_DATABITS_SIX;
    break;
  case CS7:
    r = USART_FRAME_DATABITS_SEVEN;
    break;
  case CS8:
  default:
    r = USART_FRAME_DATABITS_EIGHT;
    break;
  }

  if (c_cflag & PARENB) {
    if (c_cflag & PARODD)
      r |= USART_FRAME_PARITY_ODD;
    else
      r |= USART_FRAME_PARITY_EVEN;
  } else {
      r |= USART_FRAME_PARITY_NONE;
  }

  if (c_cflag & CSTOPB)
    r |= USART_FRAME_STOPBITS_TWO;
  else
    r |= USART_FRAME_STOPBITS_ONE;

  regs->FRAME = r;

  regs->CTRLX = (c_cflag & CRTSCTS) ? USART_CTRLX_CTSEN : 0;

  regs->TIMING = ctx->timing;

  regs->CMD = USART_CMD_TXEN |
              ((c_cflag & CREAD) ? USART_CMD_RXEN : USART_CMD_RXDIS);

}

static bool usart_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  attribs_init((struct usart_context_s *) base,
               rtems_termios_baud_to_number(term->c_ospeed),
               term->c_cflag);

  return true;
}

static void pins_init(struct usart_context_s *ctx)
{
  USART_TypeDef *regs = ctx->regs;
  rtems_interrupt_level level;
  uint32_t r;

  rtems_interrupt_disable(level);
  CMU->HFPERCLKEN0 |= ctx->hfperclken0_mask;
  rtems_interrupt_enable(level);

  regs->IEN = 0;
  regs->CTRL = USART_CTRL_TXBIL | USART_CTRL_OVS_X16 | ctx->ctrl;
  regs->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX |
              USART_CMD_RXDIS | USART_CMD_TXDIS;

  regs->ROUTELOC0 = ((ctx->tx_pin_loc << _USART_ROUTELOC0_TXLOC_SHIFT) &
                     _USART_ROUTELOC0_TXLOC_MASK) |
                    ((ctx->rx_pin_loc << _USART_ROUTELOC0_RXLOC_SHIFT) &
                     _USART_ROUTELOC0_RXLOC_MASK);
  regs->ROUTELOC1 = ((ctx->cts_pin_loc << _USART_ROUTELOC1_CTSLOC_SHIFT) &
                     _USART_ROUTELOC1_CTSLOC_MASK) |
                    ((ctx->rts_pin_loc << _USART_ROUTELOC1_RTSLOC_SHIFT) &
                     _USART_ROUTELOC1_RTSLOC_MASK);

  r = 0;
  if (ctx->tx_pin_loc >= 0) {
    efm32gg11_gpio_clear_set(ctx->tx_port, 0, 1 << ctx->tx_pin);
    efm32gg11_gpio_mode(ctx->tx_port, ctx->tx_pin,
                        GPIO_P_MODEL_MODE0_PUSHPULL);
    r |= USART_ROUTEPEN_TXPEN;
  }
  if (ctx->rx_pin_loc >= 0) {
    efm32gg11_gpio_clear_set(ctx->rx_port, 0, 1 << ctx->rx_pin);
    efm32gg11_gpio_mode(ctx->rx_port, ctx->rx_pin,
                        GPIO_P_MODEL_MODE0_INPUTPULL);
    r |= USART_ROUTEPEN_RXPEN;
  }
  if (ctx->cts_pin_loc >= 0) {
    efm32gg11_gpio_clear_set(ctx->cts_port, 0, 1 << ctx->cts_pin);
    efm32gg11_gpio_mode(ctx->cts_port, ctx->cts_pin,
                        GPIO_P_MODEL_MODE0_INPUTPULL);
    r |= USART_ROUTEPEN_CTSPEN;
  }
  if (ctx->rts_pin_loc >= 0) {
    efm32gg11_gpio_clear_set(ctx->rts_port, 0, 1 << ctx->rts_pin);
    efm32gg11_gpio_mode(ctx->rts_port, ctx->rts_pin,
                        GPIO_P_MODEL_MODE0_PUSHPULL);
    r |= USART_ROUTEPEN_RTSPEN;
  }
  if (ctx->cs_pin_loc >= 0) {
    if (ctx->ctrl & USART_CTRL_CSINV)
      efm32gg11_gpio_clear_set(ctx->cs_port, 1 << ctx->cs_pin, 0);
    else
      efm32gg11_gpio_clear_set(ctx->cs_port, 0, 1 << ctx->cs_pin);
    efm32gg11_gpio_mode(ctx->cs_port, ctx->cs_pin,
                        GPIO_P_MODEL_MODE0_PUSHPULL);
    r |= USART_ROUTEPEN_CSPEN;
  }
  regs->ROUTEPEN = r;

#if EFM32GG11_CONSOLE_VCOM_ENABLE
  if (ctx == &usart_instances[0]) {
    /* connect usart to onboard segger */
    efm32gg11_gpio_clear_set(4, 0, (1 << 1));
    efm32gg11_gpio_mode(4, 1, GPIO_P_MODEL_MODE0_PUSHPULL);
  }
#endif
}

static bool usart_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  (void) args;
  struct usart_context_s *ctx = (struct usart_context_s *) base;
  USART_TypeDef *regs = ctx->regs;

  pins_init(ctx);

  rtems_termios_set_best_baud(term, BSP_CONSOLE_BAUD);
  usart_set_attributes(base, term);

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  rtems_interrupt_handler_install(ctx->rx_irq, "USART",
                                  RTEMS_INTERRUPT_UNIQUE,
                                  usart_rx_interrupt, tty);
  rtems_interrupt_handler_install(ctx->tx_irq, "USART",
                                  RTEMS_INTERRUPT_UNIQUE,
                                  usart_tx_interrupt, tty);
  regs->IEN |= USART_IEN_RXDATAV;
#else
  (void) regs;
  (void) tty;
#endif

  return true;
}

static void usart_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  (void) args;
  struct usart_context_s *ctx = (struct usart_context_s *) base;
  USART_TypeDef *regs = ctx->regs;

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  regs->IEN = 0;
  __DSB();
  rtems_interrupt_handler_remove(ctx->tx_irq, usart_tx_interrupt, tty);
  rtems_interrupt_handler_remove(ctx->rx_irq, usart_rx_interrupt, tty);
#else
  (void) tty;
  (void) regs;
#endif
}

static void usart_write(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len)
{
  struct usart_context_s *ctx = (struct usart_context_s *) base;
  USART_TypeDef *regs = ctx->regs;

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  if (len > 0) {
    ctx->transmitting = true;
    regs->TXDATA = buf[0] & 0xff;
    regs->IEN |= USART_IEN_TXBL;
  } else {
    ctx->transmitting = false;
    regs->IEN &= ~USART_IEN_TXBL;
  }
  __DSB();
#else
  while (len--) {
    while ((regs->STATUS & USART_STATUS_TXBL) == 0)
      ;
    regs->TXDATA = *buf++ & 0xff;
  }
#endif
}

#ifndef BSP_CONSOLE_USE_INTERRUPTS
static int usart_read(rtems_termios_device_context *base)
{
  struct usart_context_s *ctx = (struct usart_context_s *) base;
  USART_TypeDef *regs = ctx->regs;
  int data;

  if (regs->STATUS & USART_STATUS_RXDATAV)
    data = regs->RXDATA;
  else
    data = -1;

  return data;
}
#endif

static const rtems_termios_device_handler usart_handler = {
  .first_open = usart_first_open,
  .last_close = usart_last_close,
  .write = usart_write,
  .set_attributes = usart_set_attributes,
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  .mode = TERMIOS_IRQ_DRIVEN,
#else
  .poll_read = usart_read,
  .mode = TERMIOS_POLLED
#endif
};


rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg)
{
  size_t i;
  (void) major;
  (void) minor;
  (void) arg;

  rtems_termios_initialize();

  for (i = 0; i < RTEMS_ARRAY_SIZE(usart_instances); i++) {
    rtems_termios_device_install(usart_instances[i].name, &usart_handler,
                                 NULL, &usart_instances[i].base);
  }

  return RTEMS_SUCCESSFUL;
}


static void output_char(char c)
{
  USART_TypeDef *regs = usart_instances[0].regs;

  while ((regs->STATUS & USART_STATUS_TXBL) == 0)
    ;
  regs->TXDATA = (uint32_t) c & 0xff;
}

static void debug_console_init(void)
{
  if (BSP_output_char != output_char) {
    pins_init(&usart_instances[0]);
    attribs_init(&usart_instances[0], BSP_CONSOLE_BAUD, CS8 | CREAD);
    BSP_output_char = output_char;
  }
}

static void output_char_early(char c)
{
  debug_console_init();
  output_char(c);
}

BSP_output_char_function_type BSP_output_char = output_char_early;

BSP_polling_getchar_function_type BSP_poll_char = NULL;

RTEMS_SYSINIT_ITEM(
  debug_console_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
