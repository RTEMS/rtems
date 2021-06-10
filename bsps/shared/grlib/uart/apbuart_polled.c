/*
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <grlib/apbuart.h>

#include <rtems/score/cpuimpl.h>

void apbuart_outbyte_wait(const struct apbuart_regs *regs)
{
  while ( (regs->status & APBUART_STATUS_TE) == 0 ) {
    /* Lower bus utilization while waiting for UART */
    _CPU_Instruction_no_operation();
    _CPU_Instruction_no_operation();
    _CPU_Instruction_no_operation();
    _CPU_Instruction_no_operation();
    _CPU_Instruction_no_operation();
    _CPU_Instruction_no_operation();
    _CPU_Instruction_no_operation();
    _CPU_Instruction_no_operation();
  }
}

void apbuart_outbyte_polled(struct apbuart_regs *regs, char ch)
{
  apbuart_outbyte_wait(regs);
  regs->data = (uint8_t) ch;
}

int apbuart_inbyte_nonblocking(struct apbuart_regs *regs)
{
  /* Clear errors */
  regs->status = ~APBUART_STATUS_ERR;

  if ((regs->status & APBUART_STATUS_DR) == 0) {
    return -1;
  }

  return (uint8_t) regs->data;
}
