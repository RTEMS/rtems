/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup Flash
 *
 * @brief Flashdev backend simulating NAND in memory
 */

/*
 * Copyright (C) 2025 On-Line Applications Research (OAR) Corporation
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
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

#ifndef NAND_SIM_FLASHDEV_H
#define NAND_SIM_FLASHDEV_H

#include <dev/flash/flashdev.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Configuration options for NAND simulation */
struct flash_sim_flashdev_attributes {
  /** @brief The minimum delay in nanoseconds for a read operation */
  uint64_t read_delay_ns;
  /** @brief The minimum delay in nanoseconds for a write operation */
  uint64_t write_delay_ns;
  /** @brief The minimum delay in nanoseconds for a erase operation */
  uint64_t erase_delay_ns;
  /** @brief The number of bytes in each page */
  uint64_t page_size_bytes;
  /** @brief The number of spare/out of band (OOB) bytes per page */
  uint64_t page_oob_bytes;
  /** @brief The number of pages in each sector */
  uint64_t pages_per_sector;
  /** @brief The total number of sectors in the simulated device */
  uint64_t total_sectors;
  /** @brief The type of flash that is being simulated */
  rtems_flashdev_flash_type type;
  /**
   * @brief Allocator for simulated flash, but not other allocations.
   * malloc() will be used if NULL
   */
  void *(*alloc)(size_t size);
  /**
   * @brief Free function for simulated flash, but not other allocations.
   * free() will be used if NULL
   */
  void (*free)(void *ptr);
};

/**
 * @brief Initializes a simulated flash device. The flash device is not
 * registered in this call. The returned object must be destroyed with
 * rtems_flashdev_destroy if it has not been registered.
 *
 * @param[in] attr A pointer to a flash_sim_flashdev_attributes describing
 * the features of the simulated flash. This parameter may be NULL.
 *
 * @retval A pointer to the rtems_flashdev.
 * @retval NULL on failure.
 */
rtems_flashdev *flash_sim_flashdev_init(
  struct flash_sim_flashdev_attributes *attr
);

#ifdef __cplusplus
}
#endif

#endif /* NAND_SIM_FLASHDEV_H */
