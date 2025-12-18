/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMEFM32GG11
 *
 * @brief EFM32GG11 TRNG getentropy() Implementation
 */

/*
 * Copyright (C) 2025 Allan N. Hessenflow
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
#include <bsp/processor.h>
#include <unistd.h>
#include <stdint.h>
#include <rtems/sysinit.h>
#include <rtems/bspIo.h>

static void efm32gg11_trng_enable(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TRNG0;
  rtems_interrupt_enable(level);

  TRNG0->CONTROL = TRNG_CONTROL_ENABLE;

  while (TRNG0->FIFOLEVEL < 4)
    ;
  TRNG0->KEY0 = TRNG0->FIFO;
  TRNG0->KEY1 = TRNG0->FIFO;
  TRNG0->KEY2 = TRNG0->FIFO;
  TRNG0->KEY3 = TRNG0->FIFO;

  TRNG0->CONTROL = TRNG_CONTROL_SOFTRESET | TRNG_CONTROL_ENABLE;
  TRNG0->CONTROL = TRNG_CONTROL_ENABLE;
}

int getentropy(void *ptr, size_t n)
{
  union {
    uint8_t b[4];
    uint32_t g;
  } v;
  int i;

  while (n) {
    /* How fast is the TRNG?  Maybe we should block until the FIFO is
       full (64 x 32 bits) (the only data available related interrupt
       for the module is fifo full), or maybe delay for a tick inside
       the wait loop */
    while (TRNG0->FIFOLEVEL == 0)
      ;
    v.g = TRNG0->FIFO;
    for (i = 0; i < 4 && n; i++, n--)
      *(uint8_t *)ptr++ = v.b[i];
  }

  return 0;
}

RTEMS_SYSINIT_ITEM(
  efm32gg11_trng_enable,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
