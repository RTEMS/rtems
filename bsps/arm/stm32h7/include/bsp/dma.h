/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_STM32H7_BSP_DMA_H
#define LIBBSP_ARM_STM32H7_BSP_DMA_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#include <stm32h7xx_hal_dma.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Acquire a DMA channel
 *
 * This function returns a DMA channel that can be used for memory-to-memory,
 * memory-to-peripheral, or peripheral-to-memory transactions. The returned DMA
 * channel is reserved for exclusive use until it is released using
 * stm32h7_dma_free().
 *
 * @param parent A pointer to the parent structure that will temporarily own the
 *               allocated DMA channel. This should be a *_HandleTypeDef pointer
 *               such as SPI_HandleTypeDef
 * @param request_type A DMA_Request_selection value defining the endpoints of
 *                     the transfer
 * @param direction A DMA_Data_transfer_direction value defining the direction
 *                  of the transfer
 * @param priority A DMA_Priority_level value defining the priority of the
 *                 transfer
 * @return A configured DMA channel handle pointer or NULL or error
 */
DMA_HandleTypeDef *stm32h7_dma_alloc(void *parent, uint32_t request_type,
  uint32_t direction, uint32_t priority);

/**
 * @brief Release a DMA channel
 *
 * This function tears down and releases a DMA channel allocated using
 * stm32h7_dma_alloc().
 *
 * @param dma_channel A DMA channel handle pointer
 */
void stm32h7_dma_free(DMA_HandleTypeDef *dma_channel);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_ARM_STM32H7_BSP_DMA_H */
