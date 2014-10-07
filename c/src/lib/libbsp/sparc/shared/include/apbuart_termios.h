/*
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef APBUART_TERMIOS_H
#define APBUART_TERMIOS_H

#include <rtems/termiostypes.h>
#include <grlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct apbuart_context {
  rtems_termios_device_context base;
  struct apbuart_regs *regs;
  unsigned int freq_hz;
  rtems_vector_number irq;
  volatile int sending;
  char *buf;
};

const rtems_termios_device_handler apbuart_handler_interrupt;

const rtems_termios_device_handler apbuart_handler_polled;

/*
 *  apbuart_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */
void apbuart_outbyte_polled(
  struct apbuart_regs *regs,
  unsigned char ch,
  int do_cr_on_newline,
  int wait_sent
);

/*
 *  apbuart_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */
int apbuart_inbyte_nonblocking(struct apbuart_regs *regs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* APBUART_TERMIOS_H */
