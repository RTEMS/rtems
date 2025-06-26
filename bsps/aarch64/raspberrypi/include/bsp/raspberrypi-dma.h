/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Raspberrypi4
 *
 * @brief Raspberry Pi specific DMA definitions.
 */

/*
 * Copyright (C) 2025 Shaunak Datar
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
#ifndef LIBBSP_AARCH64_RASPBERRYPI_DMA_H
#define LIBBSP_AARCH64_RASPBERRYPI_DMA_H

#include <bsp/raspberrypi.h>
#include <bsp/rpi-gpio.h>
#include <bsp/utility.h>
#include <rtems/malloc.h>
#include <rtems/rtems/cache.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/cpu.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief DMA channel identifiers for BCM2711 (0–6 full, 7–10 lite, 11–14 DMA4).
 */
typedef enum {
  DMA_CHANNEL_0,
  DMA_CHANNEL_1,
  DMA_CHANNEL_2,
  DMA_CHANNEL_3,
  DMA_CHANNEL_4,
  DMA_CHANNEL_5,
  DMA_CHANNEL_6,
  DMA_LITE_CHANNEL_7,
  DMA_LITE_CHANNEL_8,
  DMA_LITE_CHANNEL_9,
  DMA_LITE_CHANNEL_10,
  DMA4_CHANNEL_11,
  DMA4_CHANNEL_12,
  DMA4_CHANNEL_13,
  DMA4_CHANNEL_14,
} rpi_dma_channel;
/**
 * @brief Start a previously initialized DMA transfer on @a channel.
 *
 * Expects the channel's CONBLK_AD to point to a valid
 * control block. Sets ACTIVE and required priorities.
 *
 * @param channel DMA channel to start.
 * @retval RTEMS_SUCCESSFUL on success.
 * @retval RTEMS_INVALID_NUMBER if @a channel is invalid.
 */
rtems_status_code rpi_dma_start_transfer( rpi_dma_channel channel );

/**
 * @brief Block until the current DMA transfer on @a channel completes.
 *
 * Busy-waits for ACTIVE to clear and frees the control block previously
 * programmed into CONBLK_AD (including DMA4 address unshifting). Does **not**
 * invalidate/flush user buffers beyond setup done by the init helpers.
 *
 * @param channel DMA channel to wait on.
 * @retval RTEMS_SUCCESSFUL on success.
 * @retval RTEMS_INVALID_NUMBER if @a channel is invalid.
 * @retval RTEMS_UNSATISFIED if the channel reports an error.
 */

rtems_status_code rpi_dma_wait( rpi_dma_channel channel );

/**
 * @brief Initialize a memcpy-style DMA transfer from @a source_address to
 *        @a destination_address of @a transfer_length bytes on @a channel.
 *
 * Allocates and prepares the channel-specific control block, performs required
 * cache maintenance (flush control block and source; invalidate destination),
 * issues CS reset/abort, and writes CONBLK_AD (with DMA4 address packing via
 * DMA4_AD_SHIFT). This function does **not** start the transfer; call
 * ::rpi_dma_start_transfer() and then ::rpi_dma_wait().
 *
 * @param channel              DMA channel to use (0–6 noarmal DMA, 7–10 lite,
 *                             11–14 DMA4).
 * @param source_address       Source buffer (must be CPU_CACHE_LINE_BYTES
 *                             aligned).
 * @param destination_address  Destination buffer (must be CPU_CACHE_LINE_BYTES
 *                             aligned).
 * @param transfer_length      Number of bytes to copy.
 * @retval RTEMS_SUCCESSFUL        on success.
 * @retval RTEMS_INVALID_NUMBER    if @a channel is invalid/unsupported or has
 *                                 no base address.
 * @retval RTEMS_INVALID_ADDRESS   if @a source_address or @a
 *                                 destination_address are misaligned.
 * @retval RTEMS_NO_MEMORY         if control block allocation failed.
 */
rtems_status_code rpi_dma_mem_to_mem_init(
  rpi_dma_channel channel,
  void           *source_address,
  void           *destination_address,
  uint32_t        transfer_length
);

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_AARCH64_RASPBERRYPI_DMA_H */