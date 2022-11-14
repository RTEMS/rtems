/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
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
