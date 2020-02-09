/**
 *  @file
 *  
 */

/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *  THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 *  AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 *  You are hereby granted permission to use, copy, modify, and distribute
 *  this file, provided that this notice, plus the above copyright notice
 *  and disclaimer, appears in all copies. Radstone Technology will provide
 *  no support for this code.
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _NS16550_H_
#define _NS16550_H_

#include <rtems/termiostypes.h>
#include <libchip/serial.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Driver function table
 */

extern const console_fns ns16550_fns;
extern const console_fns ns16550_fns_polled;

/*
 * Flow control function tables
 */

extern const console_flow ns16550_flow_RTSCTS;
extern const console_flow ns16550_flow_DTRCTS;

/*
 *  Helpers for printk
 */
void ns16550_outch_polled(console_tbl *c, char out);
int ns16550_inch_polled(console_tbl *c);

/* Alternative NS16550 driver using the Termios device context */

typedef uint8_t (*ns16550_get_reg)(uintptr_t port, uint8_t reg);

typedef void (*ns16550_set_reg)(uintptr_t port, uint8_t reg, uint8_t value);

typedef struct ns16550_context ns16550_context;

typedef uint32_t (*ns16550_calculate_baud_divisor)(ns16550_context *ctx, uint32_t baud);

struct ns16550_context{
  rtems_termios_device_context base;
  ns16550_get_reg get_reg;
  ns16550_set_reg set_reg;
  uintptr_t port;
  rtems_vector_number irq;
  uint32_t clock;
  uint32_t initial_baud;
  bool has_fractional_divider_register;
  bool has_precision_clock_synthesizer;
  uint8_t modem_control;
  uint8_t line_control;
  uint32_t baud_divisor;
  size_t out_total;
  size_t out_remaining;
  size_t out_current;
  const char *out_buf;
  rtems_termios_tty *tty;
  ns16550_calculate_baud_divisor calculate_baud_divisor;
};

extern const rtems_termios_device_handler ns16550_handler_interrupt;
extern const rtems_termios_device_handler ns16550_handler_polled;
extern const rtems_termios_device_handler ns16550_handler_task;

extern const rtems_termios_device_flow ns16550_flow_rtscts;
extern const rtems_termios_device_flow ns16550_flow_dtrcts;

void ns16550_polled_putchar(rtems_termios_device_context *base, char out);

int ns16550_polled_getchar(rtems_termios_device_context *base);

bool ns16550_probe(rtems_termios_device_context *base);

#ifdef __cplusplus
}
#endif

#endif /* _NS16550_H_ */
