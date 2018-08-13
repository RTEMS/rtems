/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdint.h>
#include <rtems.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/x86_64.h>
#include <rtems/score/cpuimpl.h>
#include <bsp/irq-generic.h>
#include <pic.h>

void pic_remap(uint8_t offset1, uint8_t offset2)
{
  uint8_t a1, a2;

  /* save masks */
  a1 = inport_byte(PIC1_DATA);
  a2 = inport_byte(PIC2_DATA);

  /* start the initialization sequence in cascade mode */
  outport_byte(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
  stub_io_wait();
  outport_byte(PIC2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
  stub_io_wait();
  /* ICW2: Master PIC vector offset */
  outport_byte(PIC1_DATA, offset1);
  stub_io_wait();
  /* ICW2: Slave PIC vector offset */
  outport_byte(PIC2_DATA, offset2);
  stub_io_wait();
  /* ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100) */
  outport_byte(PIC1_DATA, 4);
  stub_io_wait();
  /* ICW3: tell Slave PIC its cascade identity (0000 0010) */
  outport_byte(PIC2_DATA, 2);
  stub_io_wait();

  outport_byte(PIC1_DATA, PIC_ICW4_8086);
  stub_io_wait();
  outport_byte(PIC2_DATA, PIC_ICW4_8086);
  stub_io_wait();

  /* restore saved masks. */
  outport_byte(PIC1_DATA, a1);
  outport_byte(PIC2_DATA, a2);
}

void pic_disable(void)
{
  /* Mask all lines on both master and slave PIC to disable */
  outport_byte(PIC1_DATA, 0xff);
  outport_byte(PIC2_DATA, 0xff);
}
