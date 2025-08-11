/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012 Sebastian Huber
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

#include <bsp/rcc.h>
#include <bsp/stm32f4.h>

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

#ifdef STM32F4_FAMILY_F4XXXX
  rcc_set(index, set, &rcc->ahbrstr [0]);
#endif/* STM32F4_FAMILY_F4XXXX */
#ifdef STM32F4_FAMILY_F10XXX
  /* The first register is missing for the reset-block */
  rcc_set(index, set, &rcc->cir);
#endif /* STM32F4_FAMILY_F10XXX */
}

void stm32f4_rcc_set_clock(stm32f4_rcc_index index, bool set)
{
  volatile stm32f4_rcc *rcc = STM32F4_RCC;

  rcc_set(index, set, &rcc->ahbenr [0]);
}

#ifdef STM32F4_FAMILY_F4XXXX
void stm32f4_rcc_set_low_power_clock(stm32f4_rcc_index index, bool set)
{
  volatile stm32f4_rcc *rcc = STM32F4_RCC;

  rcc_set(index, set, &rcc->ahblpenr [0]);
}
#endif /* STM32F4_FAMILY_F4XXXX */
