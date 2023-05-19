/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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
#include <bsp/irq.h>

#define ICR(lvl, prio) (MCF548X_INTC_ICRn_IL(lvl) | MCF548X_INTC_ICRn_IP(prio))

const uint8_t mcf548x_intc_icr_init_values[64] = {
  [MCF548X_IRQ_SLT0] = ICR(4, 7),
  [MCF548X_IRQ_SLT1] = ICR(4, 6),
  [MCF548X_IRQ_PSC0] = ICR(3, 7),
  [MCF548X_IRQ_PSC1] = ICR(3, 6),
  [MCF548X_IRQ_PSC2] = ICR(3, 5),
  [MCF548X_IRQ_PSC3] = ICR(3, 4),
  [MCF548X_IRQ_FEC0] = ICR(2, 7),
  [MCF548X_IRQ_FEC1] = ICR(2, 6)
};
