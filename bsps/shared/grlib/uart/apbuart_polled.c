/*
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <grlib/apbuart.h>

void apbuart_outbyte_polled(
  struct apbuart_regs *regs,
  unsigned char ch,
  int do_cr_on_newline,
  int wait_sent
)
{
send:
  while ( (regs->status & APBUART_STATUS_TE) == 0 ) {
    /* Lower bus utilization while waiting for UART */
    __asm__ volatile ("nop"::); __asm__ volatile ("nop"::);
    __asm__ volatile ("nop"::); __asm__ volatile ("nop"::);
    __asm__ volatile ("nop"::); __asm__ volatile ("nop"::);
    __asm__ volatile ("nop"::); __asm__ volatile ("nop"::);
  }

  if ((ch == '\n') && do_cr_on_newline) {
    regs->data = (unsigned int) '\r';
    do_cr_on_newline = 0;
    goto send;
  }
  regs->data = (unsigned int) ch;

  /* Wait until the character has been sent? */
  if (wait_sent) {
    while ((regs->status & APBUART_STATUS_TE) == 0)
      ;
  }
}

int apbuart_inbyte_nonblocking(struct apbuart_regs *regs)
{
  /* Clear errors */
  if (regs->status & APBUART_STATUS_ERR)
    regs->status = ~APBUART_STATUS_ERR;

  if ((regs->status & APBUART_STATUS_DR) == 0)
    return -1;
  else
    return (int) regs->data;
}
