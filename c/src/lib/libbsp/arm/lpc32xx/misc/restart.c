/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Restart implementation.
 */

/*
 * Copyright (c) 2010, 2011 embedded brains GmbH.  All rights reserved.
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
 *
 * $Id$
 */

#include <rtems.h>

#include <libcpu/arm-cp15.h>

#include <bsp.h>

void bsp_restart(void *addr)
{
  ARM_SWITCH_REGISTERS;
  rtems_interrupt_level level;
  uint32_t ctrl = 0;

  rtems_interrupt_disable(level);

  arm_cp15_data_cache_test_and_clean();
  arm_cp15_instruction_cache_invalidate();

  ctrl = arm_cp15_get_control();
  ctrl &= ~(ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M);
  arm_cp15_set_control(ctrl);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mov pc, %[addr]\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [addr] "r" (addr)
  );
}
