/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the bsp_reset() implementation.
 */

/*
 * Copyright (C) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#include <bsp/bootcard.h>
#include <bsp/tms570.h>
#include <bsp/tms570-pom.h>

static void handle_esm_errors(uint32_t esm_irq_channel)
{
   /* ESMR3 errors don't generate interrupts. */
   if (esm_irq_channel < 0x20u) {
     TMS570_ESM.SR[0] = 1 << esm_irq_channel;
   } else if (esm_irq_channel < 0x40u) {
     TMS570_ESM.SR[1] = 1 << (esm_irq_channel - 32u);
   } else if (esm_irq_channel < 0x60u) {
     TMS570_ESM.SR4 = 1 << (esm_irq_channel - 64u);
   }
}

void bsp_reset(void)
{
   rtems_interrupt_level level;
   uint32_t esm_irq_channel;

   rtems_interrupt_disable(level);
   (void) level;

   tms570_pom_initialize_and_clear();
   esm_irq_channel = TMS570_ESM.IOFFHR - 1;

   if (esm_irq_channel) {
     handle_esm_errors(esm_irq_channel);
   }

   /* Reset the board */
   /* write of value other than 1 cause system reset */
   TMS570_SYS1.SYSECR = TMS570_SYS1_SYSECR_RESET(2);
}
