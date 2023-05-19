/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X_dma
 *
 * @brief Direct memory access (DMA) support.
 */

/*
 * Copyright (C) 2008, 2009 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_LPC176X_DMA_H
#define LIBBSP_ARM_LPC176X_DMA_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc176x_dma DMA Support
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Direct memory access (DMA) support.
 *
 * @{
 */

/**
 * @brief Initializes the general purpose DMA.
 */
void lpc176x_dma_initialize( void );

/**
 * @brief Tries to obtain the DMA channel @a channel.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid channel number.
 * @retval RTEMS_RESOURCE_IN_USE Channel already occupied.
 */
rtems_status_code lpc176x_dma_channel_obtain( unsigned channel );

/**
 * @brief Releases the DMA channel @a channel.
 *
 * You must have obtained this channel with lpc176x_dma_channel_obtain()
 * previously.
 *
 * If the channel number @a channel is out of range nothing will happen.
 */
void lpc176x_dma_channel_release( unsigned channel );

/**
 * @brief Disables the DMA channel @a channel.
 *
 * If @a force is @c false the channel will be halted and disabled when the
 * channel is inactive otherwise it will be disabled immediately.
 *
 * If the channel number @a channel is out of range nothing will happen.
 */
void lpc176x_dma_channel_disable(
  unsigned channel,
  bool     force
);

rtems_status_code lpc176x_dma_copy_initialize( void );

rtems_status_code lpc176x_dma_copy_release( void );

rtems_status_code lpc176x_dma_copy(
  unsigned    channel,
  const void *dest,
  const void *src,
  size_t      n,
  size_t      width
);

rtems_status_code lpc176x_dma_copy_wait( unsigned channel );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC176X_DMA_H */
