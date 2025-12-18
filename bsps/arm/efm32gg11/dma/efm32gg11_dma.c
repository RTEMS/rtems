/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMEFM32GG11
 *
 * @brief EFM32GG11 LDMA Support
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
#include <bsp/efm32gg11_dma.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>


static struct dma_context_s {
  struct {
    void (*cb)(void *);
    void *arg;
  } ch[DMA_CHAN_COUNT];
} dma_context;


static void dma_interrupt(void *arg)
{
  uint32_t iflags;
  uint32_t mask;
  int i;

  (void) arg;
  iflags = LDMA->IF & LDMA->IEN;
  LDMA->IFC = iflags;
  mask = (iflags & _LDMA_IEN_DONE_MASK) >> _LDMA_IEN_DONE_SHIFT;
  i = 0;
  for (i = 0; mask && i < DMA_CHAN_COUNT; mask >>= 1, i++) {
    if ((mask & 1) && dma_context.ch[i].cb)
      dma_context.ch[i].cb(dma_context.ch[i].arg);
  }
}

void efm32gg11_dma_register(unsigned int channel,
                            void (*done_int)(void *), void *arg)
{
  if (channel < DMA_CHAN_COUNT) {
    dma_context.ch[channel].arg = arg;
    dma_context.ch[channel].cb = done_int;
  }
}

void efm32gg11_dma_int_enable(unsigned int channel, bool enable)
{
  rtems_interrupt_level level;
  uint32_t mask;

  if (channel < DMA_CHAN_COUNT) {
    mask = (uint32_t) 1 << (channel + _LDMA_IEN_DONE_SHIFT);
    rtems_interrupt_disable(level);
    if (enable)
      LDMA->IEN |= mask;
    else
      LDMA->IEN &= ~mask;
    __DSB();
    rtems_interrupt_enable(level);
  }
}

static void efm32gg11_dma_init(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_LDMA;
  rtems_interrupt_enable(level);

  rtems_interrupt_handler_install(LDMA_IRQn, "DMA",
                                  RTEMS_INTERRUPT_UNIQUE,
                                  dma_interrupt, NULL);
}

RTEMS_SYSINIT_ITEM(
  efm32gg11_dma_init,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
