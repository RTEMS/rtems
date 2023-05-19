/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <bsp/vectors.h>

int ppc_exc_alignment_handler(BSP_Exception_frame *frame, unsigned excNum)
{
  unsigned opcode = *(unsigned *) frame->EXC_SRR0;

  /* Do we have a dcbz instruction? */
  if ((opcode & 0xffe007ff) == 0x7c0007ec) {
    unsigned clsz = rtems_cache_get_data_line_size();
    unsigned a = (opcode >> 16) & 0x1f;
    unsigned b = (opcode >> 11) & 0x1f;
    PPC_GPR_TYPE *regs = &frame->GPR0;
    unsigned *current = (unsigned *)
      (((a == 0 ? 0 : (unsigned) regs[a]) + (unsigned) regs[b]) & (clsz - 1));
    unsigned *end = current + clsz / sizeof(*current);

    while (current != end) {
      *current = 0;
      ++current;
    }

    frame->EXC_SRR0 += 4;

    return 0;
  } else {
    return -1;
  }
}
