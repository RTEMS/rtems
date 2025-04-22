/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32H7
 *
 * @brief This source file contains the implementation of the DMA allocator.
 */

/*
 * Copyright (C) 2024 On-Line Applications Research (OAR) Corporation
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <bsp/dma.h>
#include <bsp/fatal.h>
#include <inttypes.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <stdio.h>
#include <stdlib.h>

#include <stm32h7/hal.h>

/*
 * Each bit in this uint16_t corresponds to a DMA channel in the two main DMA
 * peripherals.
 */
static uint16_t dma_channels = 0;

static rtems_mutex dma_lock = RTEMS_MUTEX_INITIALIZER("DMA Allocator");
static uintptr_t dma_channel_size = DMA1_Stream1_BASE - DMA1_Stream0_BASE;

static rtems_vector_number stm32h7_dma_get_irq(uintptr_t base)
{
  switch (base) {
  case DMA1_Stream0_BASE:
    return DMA1_Stream0_IRQn;
  case DMA1_Stream1_BASE:
    return DMA1_Stream1_IRQn;
  case DMA1_Stream2_BASE:
    return DMA1_Stream2_IRQn;
  case DMA1_Stream3_BASE:
    return DMA1_Stream3_IRQn;
  case DMA1_Stream4_BASE:
    return DMA1_Stream4_IRQn;
  case DMA1_Stream5_BASE:
    return DMA1_Stream5_IRQn;
  case DMA1_Stream6_BASE:
    return DMA1_Stream6_IRQn;
  case DMA1_Stream7_BASE:
    return DMA1_Stream7_IRQn;
  case DMA2_Stream0_BASE:
    return DMA2_Stream0_IRQn;
  case DMA2_Stream1_BASE:
    return DMA2_Stream1_IRQn;
  case DMA2_Stream2_BASE:
    return DMA2_Stream2_IRQn;
  case DMA2_Stream3_BASE:
    return DMA2_Stream3_IRQn;
  case DMA2_Stream4_BASE:
    return DMA2_Stream4_IRQn;
  case DMA2_Stream5_BASE:
    return DMA2_Stream5_IRQn;
  case DMA2_Stream6_BASE:
    return DMA2_Stream6_IRQn;
  case DMA2_Stream7_BASE:
    return DMA2_Stream7_IRQn;
  }

  return 0;
}

typedef struct stm32h7_dma_context {
  DMA_HandleTypeDef dma;
  rtems_interrupt_entry dma_entry;
} stm32h7_dma_context;

static void stm32h7_dma_free_nolock(DMA_HandleTypeDef *dma_channel)
{
  stm32h7_dma_context *ctx = RTEMS_CONTAINER_OF(dma_channel, stm32h7_dma_context, dma);
  uintptr_t dma_channel_offset = (uintptr_t) dma_channel->Instance;
  uint8_t index = 0;
  uint16_t dma_mask = 0;
  rtems_status_code sc;

  if (dma_channel_offset > DMA2_BASE) {
    index += 8;
    dma_channel_offset -= DMA2_Stream0_BASE;
  } else {
    dma_channel_offset -= DMA1_Stream0_BASE;
  }

  index += dma_channel_offset / dma_channel_size;
  dma_mask = 1 << index;

  sc = rtems_interrupt_entry_remove(
    stm32h7_dma_get_irq((uintptr_t) dma_channel->Instance),
    &ctx->dma_entry
  );
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  dma_channels &= ~dma_mask;

  HAL_DMA_DeInit(dma_channel);
  free(ctx);
}

static void stm32h7_dma_irq_handler(void *arg)
{
  stm32h7_dma_context *ctx = arg;

  HAL_DMA_IRQHandler(&ctx->dma);
}

DMA_HandleTypeDef *stm32h7_dma_alloc(
  void *parent,
  uint32_t request_type,
  uint32_t direction,
  uint32_t priority
)
{
  DMA_HandleTypeDef *dma_channel = NULL;
  stm32h7_dma_context *dma_context = NULL;

  rtems_mutex_lock(&dma_lock);
  for (int i = 0; i < 16; i++) {
    uint16_t dma_mask = 1 << i;
    uintptr_t dma_channel_offset = 0;
    stm32h7_module_index index;
    rtems_status_code sc;

    if ((dma_channels & dma_mask) != 0) {
      continue;
    }
    /* Mark DMA channel as consumed */
    dma_channels |= dma_mask;

    /* Allocate and initialize the DMA channel structure */
    dma_context = calloc(1, sizeof(stm32h7_dma_context));
    if (dma_context == NULL) {
      /* Allocation failed, unmark channel */
      dma_channels &= ~dma_mask;
      break;
    }
    dma_channel = &dma_context->dma;

    /*
     * Set the DMA channel offset base using stream 0 depending on which
     * peripheral it is on.
     */
    if (i > 7) {
      index = STM32H7_MODULE_DMA2;
      dma_channel_offset = DMA2_Stream0_BASE;
      i -= 8;
    } else {
      index = STM32H7_MODULE_DMA1;
      dma_channel_offset = DMA1_Stream0_BASE;
    }

    dma_channel_offset += i * dma_channel_size;

    dma_channel->Instance = (void *) dma_channel_offset;
    dma_channel->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    dma_channel->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    dma_channel->Init.MemBurst = DMA_MBURST_INC4;
    dma_channel->Init.PeriphBurst = DMA_PBURST_INC4;
    dma_channel->Init.PeriphInc = DMA_PINC_DISABLE;
    dma_channel->Init.MemInc = DMA_MINC_ENABLE;
    dma_channel->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    dma_channel->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_channel->Init.Mode = DMA_NORMAL;
    dma_channel->Init.Request = request_type;
    dma_channel->Init.Direction = direction;
    dma_channel->Init.Priority = priority;
    dma_channel->Parent = parent;

    rtems_interrupt_entry_initialize(
      &dma_context->dma_entry,
      stm32h7_dma_irq_handler,
      dma_context,
      "DMA"
    );
    sc = rtems_interrupt_entry_install(
      stm32h7_dma_get_irq(dma_channel_offset),
      RTEMS_INTERRUPT_UNIQUE,
      &dma_context->dma_entry
    );
    if (sc != RTEMS_SUCCESSFUL) {
      stm32h7_dma_free_nolock(dma_channel);
      dma_channel = NULL;
      break;
    }

    stm32h7_clk_enable(index);
    HAL_DMA_Init(dma_channel);
    break;
  }
  rtems_mutex_unlock(&dma_lock);

  return dma_channel;
}

void stm32h7_dma_free(DMA_HandleTypeDef *dma_channel)
{
  rtems_mutex_lock(&dma_lock);
  stm32h7_dma_free_nolock(dma_channel);
  rtems_mutex_unlock(&dma_lock);
}
