/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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
#include <bsp/bootcard.h>
#include <leon.h>

#ifdef RTEMS_SMP

void bsp_reset(void)
{
  uint32_t self_cpu = rtems_smp_get_current_processor();

  if (self_cpu == 0) {
    volatile struct irqmp_regs *irqmp = LEON3_IrqCtrl_Regs;

    if (irqmp != NULL) {
      /*
       * Value was choosen to get something in the magnitude of 1ms on a 200MHz
       * processor.
       */
      uint32_t max_wait = 1234567;

      uint32_t cpu_count = rtems_get_processor_count();
      uint32_t halt_mask = 0;
      uint32_t i;

      for (i = 0; i < cpu_count; ++i) {
        if (i != self_cpu) {
          halt_mask |= UINT32_C(1) << i;
        }
      }

      /* Wait some time for secondary processors to halt */
      i = 0;
      while ((irqmp->mpstat & halt_mask) != halt_mask && i < max_wait) {
        ++i;
      }
    }

    __asm__ volatile (
      "mov 1, %g1\n"
      "ta 0\n"
      "nop"
    );
  }

  leon3_power_down_loop();
}

#endif /* RTEMS_SMP */
