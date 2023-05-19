/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_sparse_disk
 *
 * @brief Sparse disk block device API.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#ifndef SPARSE_DISK_H
#define SPARSE_DISK_H

#include <stddef.h>
#include <stdint.h>
#include <rtems.h>
#include <rtems/diskdevs.h>
#include <rtems/thread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup rtems_sparse_disk Sparse Disk Device
 *
 * @ingroup rtems_blkdev
 *
 */
/**@{**/

typedef struct {
  rtems_blkdev_bnum  block;
  void              *data;
} rtems_sparse_disk_key;

typedef struct rtems_sparse_disk rtems_sparse_disk;

typedef void (*rtems_sparse_disk_delete_handler)(rtems_sparse_disk *sparse_disk);

struct rtems_sparse_disk {
  rtems_mutex                      mutex;
  rtems_blkdev_bnum                blocks_with_buffer;
  size_t                           used_count;
  uint32_t                         media_block_size;
  rtems_sparse_disk_delete_handler delete_handler;
  uint8_t                          fill_pattern;
  rtems_sparse_disk_key           *key_table;
};

/**
 * @brief Creates and registers a sparse disk.
 *
 * @param[in] device_file_name The device file name path.
 * @param[in] media_block_size The media block size in bytes.
 * @param[in] blocks_with_buffer Blocks of the device with a buffer.  Other
 * blocks can store only fill pattern value bytes.
 * @param[in] block_count The media block count of the device.  It is the sum
 * of blocks with buffer and blocks that contain only fill pattern value bytes.
 * @param[in] fill_pattern The fill pattern specifies the byte value of blocks
 * without a buffer.  It is also the initial value for blocks with a buffer.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_NUMBER Media block size or media block count is not
 * positive.  The blocks with buffer count is greater than the media block count.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_TOO_MANY Cannot create semaphore.
 * @retval RTEMS_UNSATISFIED Cannot create generic device node.
 *
 * @see rtems_sparse_disk_register().
 */
rtems_status_code rtems_sparse_disk_create_and_register(
  const char        *device_file_name,
  uint32_t           media_block_size,
  rtems_blkdev_bnum  blocks_with_buffer,
  rtems_blkdev_bnum  media_block_count,
  uint8_t            fill_pattern
);

/**
 * @brief Frees a sparse disk.
 *
 * Calls free() on the sparse disk pointer.
 */
void rtems_sparse_disk_free( rtems_sparse_disk *sparse_disk );

/**
 * @brief Initializes and registers a sparse disk.
 *
 * This will create one semaphore for mutual exclusion.
 *
 * @param[in] device_file_name The device file name path.
 * @param[in, out] sparse_disk The sparse disk.
 * @param[in] media_block_size The media block size in bytes.
 * @param[in] blocks_with_buffer Blocks of the device with a buffer.  Other
 * blocks can store only fill pattern value bytes.
 * @param[in] block_count The media block count of the device.  It is the sum
 * of blocks with buffer and blocks that contain only fill pattern value bytes.
 * @param[in] fill_pattern The fill pattern specifies the byte value of blocks
 * without a buffer.  It is also the initial value for blocks with a buffer.
 * @param[in] sparse_disk_delete The sparse disk delete handler.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_NUMBER Media block size or media block count is not
 * positive.  The blocks with buffer count is greater than the media block count.
 * @retval RTEMS_INVALID_ADDRESS Invalid sparse disk address.
 * @retval RTEMS_TOO_MANY Cannot create semaphore.
 * @retval RTEMS_UNSATISFIED Cannot create generic device node.
 */
rtems_status_code rtems_sparse_disk_register(
  const char                       *device_file_name,
  rtems_sparse_disk                *sparse_disk,
  uint32_t                          media_block_size,
  rtems_blkdev_bnum                 blocks_with_buffer,
  rtems_blkdev_bnum                 media_block_count,
  uint8_t                           fill_pattern,
  rtems_sparse_disk_delete_handler  sparse_disk_delete
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SPARSE_DISK_H */
