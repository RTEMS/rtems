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

#include <bsp/raspberrypi-dma.h>
#define DMA4_AD_SHIFT( addr ) ( addr >> 5 )
#define DMA4_AD_UNSHIFT( addr ) ( addr << 5 )
#define ADDRESS_LOW( addr ) ( (uintptr_t) ( addr ) & 0xFFFFFFFF )
#define ADDRESS_HIGH( addr ) ( ( (uintptr_t) ( addr ) >> 32 ) & 0xFF )
#define BUS_ADDR( addr ) ( ( ( addr ) & ~0xC0000000 ) | 0xC0000000 )

typedef struct {
  uint32_t transfer_info;    /**< Control register */
  uint32_t source_addr;      /**< Source address register */
  uint32_t destination_addr; /**< Destination address register */
  uint32_t transfer_length;  /**< Transfer length register */
  uint32_t mode_2d_stride;   /**< Stride register */
  uint32_t next_cb;          /**< Next control block address register */
  uint32_t reserved[ 2 ];    /**< Reserved */
} rpi_dma_control_block;

typedef struct {
  uint32_t transfer_info;    /**< Control register */
  uint32_t source_addr;      /**< Source address register */
  uint32_t destination_addr; /**< Destination address register */
  uint32_t transfer_length;  /**< Transfer length register */
  uint32_t reserved_bit;     /**< Reserved */
  uint32_t next_cb;          /**< Next control block address register */
  uint32_t reserved[ 2 ];    /**< Reserved */
} rpi_dma_lite_control_block;

typedef struct {
  uint32_t transfer_info;    /**< Control register */
  uint32_t source_addr;      /**< Source address register */
  uint32_t source_info;      /**< Source information */
  uint32_t destination_addr; /**< Destination address register */
  uint32_t destination_info; /**< Destination information */
  uint32_t transfer_length;  /**< Transfer length register */
  uint32_t next_cb;          /**< Next control block address register */
  uint32_t reserved;         /**< Reserved */
} rpi_dma4_control_block;

static const uint32_t dma_base_addresses[] = {
  BCM2711_DMA0_BASE,
  BCM2711_DMA1_BASE,
  BCM2711_DMA2_BASE,
  BCM2711_DMA3_BASE,
  BCM2711_DMA4_BASE,
  BCM2711_DMA5_BASE,
  BCM2711_DMA6_BASE,
  BCM2711_DMA7_BASE,
  BCM2711_DMA8_BASE,
  BCM2711_DMA9_BASE,
  BCM2711_DMA10_BASE,
  BCM2711_DMA11_BASE,
  BCM2711_DMA12_BASE,
  BCM2711_DMA13_BASE,
  BCM2711_DMA14_BASE
};

static inline uint32_t get_base_address( rpi_dma_channel channel )
{
  if ( channel >= 0 && channel <= DMA4_CHANNEL_14 ) {
    return dma_base_addresses[ channel ];
  }
  return 0;
}

static rpi_dma_control_block *rpi_dma_init_cb(
  void    *source_address,
  void    *destination_address,
  uint32_t transfer_length
)
{
  rpi_dma_control_block *cb = (rpi_dma_control_block *)
    rtems_heap_allocate_aligned_with_boundary(
      sizeof( rpi_dma_control_block ),
      CPU_CACHE_LINE_BYTES,
      0
    );
  if ( cb == NULL ) {
    return NULL;
  }

  cb->source_addr      = BUS_ADDR( (uint32_t) (uintptr_t) source_address );
  cb->destination_addr = BUS_ADDR( (uint32_t) (uintptr_t) destination_address );
  cb->transfer_length  = transfer_length;
  cb->transfer_info    = ( TI_DEST_INC | TI_SRC_INC );
  cb->mode_2d_stride   = 0;
  cb->next_cb          = 0;
  cb->reserved[ 0 ]    = 0;
  cb->reserved[ 1 ]    = 0;

  return cb;
}

static rpi_dma_lite_control_block *rpi_dma_lite_init_cb(
  void    *source_address,
  void    *destination_address,
  uint32_t transfer_length
)
{
  rpi_dma_lite_control_block *cb = (rpi_dma_lite_control_block *)
    rtems_heap_allocate_aligned_with_boundary(
      sizeof( rpi_dma_control_block ),
      CPU_CACHE_LINE_BYTES,
      0
    );
  if ( cb == NULL ) {
    return NULL;
  }

  cb->source_addr      = BUS_ADDR( (uint32_t) (uintptr_t) source_address );
  cb->destination_addr = BUS_ADDR( (uint32_t) (uintptr_t) destination_address );
  cb->transfer_length  = transfer_length;
  cb->transfer_info    = ( TI_DEST_INC | TI_SRC_INC );
  cb->next_cb          = 0;
  cb->reserved_bit     = 0;
  cb->reserved[ 0 ]    = 0;
  cb->reserved[ 1 ]    = 0;

  return cb;
}

static rpi_dma4_control_block *rpi_dma4_init_cb(
  void    *source_address,
  void    *destination_address,
  uint32_t transfer_length
)
{
  rpi_dma4_control_block *cb = (rpi_dma4_control_block *)
    rtems_heap_allocate_aligned_with_boundary(
      sizeof( rpi_dma4_control_block ),
      CPU_CACHE_LINE_BYTES,
      0
    );
  if ( cb == NULL ) {
    return NULL;
  }

  cb->source_addr      = (uint32_t) ADDRESS_LOW( source_address );
  cb->source_info      = SI_SRC_INC | ADDRESS_HIGH( source_address );
  cb->destination_addr = (uint32_t) ADDRESS_LOW( destination_address );
  cb->destination_info = DI_DEST_INC | ADDRESS_HIGH( destination_address );
  cb->transfer_length  = transfer_length;
  cb->transfer_info    = 0;
  cb->next_cb          = 0;
  cb->reserved         = 0;

  return cb;
}

static inline void rpi_dma_free_control_block(
  rpi_dma_channel channel,
  uint32_t        base_address
)
{
  uint32_t cb_ad_reg = BCM2835_REG( base_address + CONBLK_AD_OFFSET );
  if ( cb_ad_reg == 0 ) {
    return;
  }

  uintptr_t cb_addr;
  if ( channel >= DMA4_CHANNEL_11 && channel <= DMA4_CHANNEL_14 ) {
    cb_addr = DMA4_AD_UNSHIFT( cb_ad_reg );
  } else {
    cb_addr = (uintptr_t) cb_ad_reg;
  }

  if ( cb_addr != 0 ) {
    void *cb = (void *) cb_addr;
    free( cb );
    BCM2835_REG( base_address + CONBLK_AD_OFFSET ) = 0;
  }
}

rtems_status_code rpi_dma_start_transfer( rpi_dma_channel channel )
{
  uint32_t base_address = get_base_address( channel );
  if ( !base_address ) {
    return RTEMS_INVALID_NUMBER;
  }
  BCM2835_REG( base_address + CS_OFFSET ) = CS_WAIT_FOR_OUTSTANDING_WRITES |
                                            CS_PANIC_PRIORITY_SHIFT |
                                            CS_PRIORITY_SHIFT;
  BCM2835_REG( base_address + CS_OFFSET ) |= CS_ACTIVE;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rpi_dma_wait( rpi_dma_channel channel )
{
  uint32_t base_address = get_base_address( channel );
  if ( !base_address ) {
    return RTEMS_INVALID_NUMBER;
  }
  while (( BCM2835_REG( base_address + CS_OFFSET ) & CS_ACTIVE ));

  rpi_dma_free_control_block( channel, base_address );
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rpi_dma_mem_to_mem_init(
  rpi_dma_channel channel,
  void           *source_address,
  void           *destination_address,
  uint32_t        transfer_length
)
{
  uint32_t base_address = get_base_address( channel );
  if ( !base_address ) {
    return RTEMS_INVALID_NUMBER;
  }

  if ( ( (uintptr_t) source_address % CPU_CACHE_LINE_BYTES ) != 0 ||
       ( (uintptr_t) destination_address % CPU_CACHE_LINE_BYTES ) != 0 ) {
    return RTEMS_INVALID_ADDRESS;
  }

  void  *control_block = NULL;
  size_t cb_size       = 0;

  if ( channel < DMA_LITE_CHANNEL_7 ) {
    control_block = rpi_dma_init_cb(
      source_address,
      destination_address,
      transfer_length
    );
    cb_size = sizeof( rpi_dma_control_block );
  } else if ( channel > DMA_CHANNEL_6 && channel < DMA4_CHANNEL_11 ) {
    control_block = rpi_dma_lite_init_cb(
      source_address,
      destination_address,
      transfer_length
    );
    cb_size = sizeof( rpi_dma_lite_control_block );
  } else if ( channel > DMA_LITE_CHANNEL_10 && channel <= DMA4_CHANNEL_14 ) {
    control_block = rpi_dma4_init_cb(
      source_address,
      destination_address,
      transfer_length
    );
    cb_size = sizeof( rpi_dma4_control_block );
  } else {
    return RTEMS_INVALID_NUMBER;
  }

  if ( control_block == NULL ) {
    return RTEMS_NO_MEMORY;
  }

  BCM2835_REG( base_address + CS_OFFSET ) = CS_RESET | CS_ABORT;
  rtems_cache_flush_multiple_data_lines( control_block, cb_size );
  rtems_cache_flush_multiple_data_lines( source_address, transfer_length );
  rtems_cache_invalidate_multiple_data_lines(
    destination_address,
    transfer_length
  );

  if ( ( channel >= DMA4_CHANNEL_11 ) && ( channel <= DMA4_CHANNEL_14 ) ) {
    uint32_t cb_addr      = (uint32_t) (uintptr_t) control_block;
    uint32_t dma4_cb_addr = DMA4_AD_SHIFT( cb_addr );
    BCM2835_REG( base_address + CONBLK_AD_OFFSET ) = dma4_cb_addr;
  } else {
    BCM2835_REG( base_address + CONBLK_AD_OFFSET ) = (uint32_t) (uintptr_t
    ) control_block;
  }

  return RTEMS_SUCCESSFUL;
}