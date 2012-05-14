/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp/vectors.h>

int ppc_exc_alignment_handler(BSP_Exception_frame *frame, unsigned excNum)
{
  unsigned opcode = *(unsigned *) frame->EXC_SRR0;

  /* Do we have a dcbz instruction? */
  if ((opcode & 0xffe007ff) == 0x7c0007ec) {
    unsigned clsz = (unsigned) rtems_cache_get_data_line_size();
    unsigned a = (opcode >> 16) & 0x1f;
    unsigned b = (opcode >> 11) & 0x1f;
    unsigned *regs = &frame->GPR0;
    unsigned *current = (unsigned *)
      (((a == 0 ? 0 : regs [a]) + regs [b]) & (clsz - 1));
    unsigned *end = current + clsz / 4;

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
