/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
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

#include <bsp/rcc.h>

#include <rtems.h>

static void rcc_set(
  stm32f4_rcc_index index,
  bool set,
  volatile uint32_t *regs
)
{
  int reg = index >> 5;
  uint32_t one = 1;
  uint32_t bit = one << (index & 0x1f);
  rtems_interrupt_level level;
  uint32_t val;

  rtems_interrupt_disable(level);
  val = regs [reg];
  if (set) {
    val |= bit;
  } else {
    val &= ~bit;
  }
  regs [reg] = val;
  rtems_interrupt_enable(level);
}

void stm32f4_rcc_reset(stm32f4_rcc_index index)
{
  stm32f4_rcc_set_reset(index, true);
  stm32f4_rcc_set_reset(index, false);
}

void stm32f4_rcc_set_reset(stm32f4_rcc_index index, bool set)
{
  volatile stm32f4_rcc *rcc = STM32F4_RCC;

  rcc_set(index, set, &rcc->ahbrstr [0]);
}

void stm32f4_rcc_set_clock(stm32f4_rcc_index index, bool set)
{
  volatile stm32f4_rcc *rcc = STM32F4_RCC;

  rcc_set(index, set, &rcc->ahbenr [0]);
}

void stm32f4_rcc_set_low_power_clock(stm32f4_rcc_index index, bool set)
{
  volatile stm32f4_rcc *rcc = STM32F4_RCC;

  rcc_set(index, set, &rcc->ahblpenr [0]);
}
