/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
