/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup Flash
 *
 * @brief Generic Flash API
 */

/*
 * Copyright (C) 2023 Aaron Nyholm
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

#ifndef _DEV_FLASHDEV_H
#define _DEV_FLASHDEV_H

#include <rtems/thread.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct rtems_flashdev rtems_flashdev;

/**
 * @defgroup Generic Flash API
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief Generic Flash API to wrap specific device drivers.
 *
 * @{
 */

/* IOCTL Calls */

/**
 * @brief Obtains the flash device.
 *
 * This command has no argument.
 */
#define RTEMS_FLASHDEV_IOCTL_OBTAIN 0
/**
 * @brief Releases the flash device.
 *
 * This command has no argument.
 */
#define RTEMS_FLASHDEV_IOCTL_RELEASE 1
/**
 * @brief Returns the JEDEC ID of the flash device. This IOCTL call
 * is informational only.
 *
 * @param[out] jedec_id Pointer to uint32_t in which the JEDEC ID is
 * returned in.
 */
#define RTEMS_FLASHDEV_IOCTL_JEDEC_ID 2
/**
 * @brief Erases flash device.
 *
 * @param[in] erase_args Pointer to rtems_flashdev_region struct
 * containing offset and size of erase to be performed.
 */
#define RTEMS_FLASHDEV_IOCTL_ERASE 3
/**
 * @brief Set a region that limits read, write and erase calls to within it.
 * Regions are file descriptor specific and limited to a single region per
 * file descriptor and 32 regions total per flash device. Regions can be
 * changed or updated by calling this IOCTL again.
 *
 * @param[in] region Pointer to rtems_flashdev_region struct containing
 * base and length of defined region.
 */
#define RTEMS_FLASHDEV_IOCTL_REGION_SET 4
/**
 * @brief Removes the set region on the file descriptor.
 *
 * This command has no argument.
 */
#define RTEMS_FLASHDEV_IOCTL_REGION_UNSET 5
/**
 * @brief Returns the type of flash device (e.g. NOR or NAND).
 *
 * @param[out] flash_type Pointer to integer which is set to the flash
 * type macro value.
 */
#define RTEMS_FLASHDEV_IOCTL_TYPE 6

/**
 * @brief Get the size and address of flash page at given offset
 *
 * The offset ignores the region limiting. To find page of region
 * limited offset add the base of the region to the desired offset.
 *
 * @param[in,out] rtems_flashdev_ioctl_page_info arg Pointer to struct
 * with offset and space for return values.
 */
#define RTEMS_FLASHDEV_IOCTL_PAGEINFO_BY_OFFSET 7

/**
 * @brief Get the size and address of nth flash page where n is index passed in.
 *
 * The index ignores the region limiting.
 *
 * @param[in,out] rtems_flashdev_ioctl_page_info arg Pointer to struct
 * with index and space for return values.
 */
#define RTEMS_FLASHDEV_IOCTL_PAGEINFO_BY_INDEX 8

/**
 * @brief Get the number of pages in flash device.
 *
 * @param[out] count Integer containing the number of pages.
 */
#define RTEMS_FLASHDEV_IOCTL_PAGE_COUNT 9

/**
 * @brief Get the minimum write size supported by the driver.
 *
 * @param[out] count Integer containing the minimum write size.
 */
#define RTEMS_FLASHDEV_IOCTL_WRITE_BLOCK_SIZE 10

/**
 * @brief The maximum number of region limited file descriptors
 * allowed to be open at once.
 */
#define RTEMS_FLASHDEV_MAX_REGIONS 32

/**
 * @brief Enum for flash type returned from IOCTL call.
 */
typedef enum rtems_flashdev_flash_type {
  /**
   * @brief The flash device is NOR flash.
   **/
  RTEMS_FLASHDEV_NOR,
  /**
   * @brief The flash device is NAND flash.
   */
  RTEMS_FLASHDEV_NAND
} rtems_flashdev_flash_type;

/**
 * @brief General definition for on flash device.
 */
typedef struct rtems_flashdev_region {
  /**
   * @brief Base of region.
   */
  off_t offset;
  /**
   * @brief Length of region.
   */
  size_t size;
} rtems_flashdev_region;

/**
 * @brief Struct holding region definitions
 */
typedef struct rtems_flashdev_region_table {
  /**
   * @brief The maximum regions that can be defined at once.
   */
  int max_regions;

  /**
   * @brief Pointer to array of rtems_flashdev_region of length
   * max_regions
   */
  rtems_flashdev_region* regions;

  /**
   * @brief Array of uint32_t acting as bit allocator
   * for regions array.
   */
  uint32_t *bit_allocator;
} rtems_flashdev_region_table;

/**
 * @brief Page information returned from IOCTL calls.
 */
typedef struct rtems_flashdev_ioctl_page_info {
  /**
   * @brief Offset or index to find page at.
   */
  off_t location;

  /**
   * @brief Information returned about the page. Including the
   * base offset and size of page.
   */
  rtems_flashdev_region page_info;
} rtems_flashdev_ioctl_page_info;

/**
 * @brief Flash device.
 */
struct rtems_flashdev {
  /**
   * @brief Call to the device driver to read the flash device.
   *
   * @param[in] flash Pointer to flash device.
   * @param[in] offset Address to read from.
   * @param[in] count Number of bytes to read.
   * @param[out] buffer Buffer for data to be read into.
   *
   * @retval 0 Successful operation.
   * @retval 1 Failed operation.
   */
  int ( *read )(
    rtems_flashdev *flash,
    uintptr_t offset,
    size_t count,
    void *buffer
  );

  /**
   * @brief Call to the device driver to write to the flash device.
   *
   * @param[in] flash Pointer to flash device.
   * @param[in] offset Address to write to.
   * @param[in] count Number of bytes to read.
   * @param[in] buffer Buffer for data to be written from.
   *
   * @retval 0 Successful operation.
   * @retval 1 Failed operation.
   */
  int ( *write )(
    rtems_flashdev *flash,
    uintptr_t offset,
    size_t count,
    const void *buffer
  );

  /**
   * @brief Call to the device driver to erase the flash device.
   *
   * @param[in] flash Pointer to flash device.
   * @param[in] offset Address to erase at.
   * @param[in] count Number of bytes to erase.
   *
   * @retval 0 Successful operation.
   * @retval 1 Failed operation.
   */
  int ( *erase )(
    rtems_flashdev *flash,
    uintptr_t offset,
    size_t count
  );

  /**
   * @brief Call to the device driver to return the JEDEC ID.
   *
   * @param[in] flash The flash device.
   *
   * @retval JEDEC ID.
   */
  uint32_t ( *jedec_id )(
    rtems_flashdev *flash
  );

  /**
   * @brief Call to the device driver to return the flash type.
   *
   * @param[in] flash The flash device.
   * @param[out] type The type of flash device.
   *
   * @retval 0 Success
   * @retbal Other Failure
   */
  int ( *flash_type )(
    rtems_flashdev *flash,
    rtems_flashdev_flash_type *type
  );

  /**
   * @brief Call to device driver to get size and offset of page at
   * given offset.
   *
   * @param[in] flash The flash device
   * @param[in] search_offset The offset of the page which info is to be
   * returned.
   * @param[out] page_offset The offset of the start of the page
   * @param[out] page_size The size of the page
   *
   * @retval 0 Success.
   * @retval non-zero Failed.
   */
  int ( *page_info_by_offset )(
    rtems_flashdev *flash,
    off_t search_offset,
    off_t *page_offset,
    size_t *page_size
  );

  /**
   * @brief Call to device driver to get size and offset of page at
   * given index.
   *
   * @param[in] flash The flash device
   * @param[in] search_index The index of the page which info is to be returned.
   * @param[out] page_offset The offset of the start of the page
   * @param[out] page_size The size of the page
   *
   * @retval 0 Success.
   * @retval non-zero Failed.
   */
  int ( *page_info_by_index )(
    rtems_flashdev *flashdev,
    off_t search_index,
    off_t *page_offset,
    size_t *page_size
  );

  /**
   * @brief Call to device driver to return the number of pages on the flash
   * device.
   *
   * @param[out] page_count The number of pages on the flash device.
   *
   * @retval 0 Success.
   * @retval non-zero Failed.
   */
  int ( *page_count )(
    rtems_flashdev *flashdev,
    int *page_count
  );

  /**
   * @brief Call to device driver to return the minimum write size of the
   * flash device.
   *
   * @param[out] write_block_size The minimum write size of the flash device.
   *
   * @retval 0 Success.
   * @retval non-zero Failed.
   */
  int ( *write_block_size )(
    rtems_flashdev *flashdev,
    size_t *write_block_size
  );

  /**
   * @brief Destroys the flash device.
   *
   * @param[in] flash Pointer to flash device.
   */
  void ( *destroy )(
    rtems_flashdev *flashdev
  );

  /**
   * @brief Pointer to device driver.
   */
  void *driver;

  /**
   * @brief Mutex to protect the flash device access.
   */
  rtems_recursive_mutex mutex;

  /**
   * @brief Region table defining size and memory for region allocations
   */
  rtems_flashdev_region_table *region_table;
};

/**
 * @brief Allocate and initialize the flash device.
 *
 * After a successful allocation and initialization of the flash device
 * it must be destroyed via rtems_flashdev_destroy_and_free().
 *
 * @param[in] size The number of bytes to allocate.
 *
 * @retval NULL Failed to set up flash device.
 * @retval non-NULL The new flash device.
 */
rtems_flashdev *rtems_flashdev_alloc_and_init(
  size_t size
);

/**
 * @brief Initialize the flash device.
 *
 * After a successful initialization of the flash device it must be
 * destroyed via rtems_flashdev_destory().
 *
 * After initialization and before registration read, write, erase, jedec_id
 * and flash_type functions need to be set in the flashdev.
 *
 * @param[in] flash The flash device to initialize.
 *
 * @retval 1 Failed to set up flash device.
 * @retval 0 Successful set up of flash device.
 */
int rtems_flashdev_init(
  rtems_flashdev *flash
);

/**
 * @brief Register the flash device.
 *
 * This function always claims ownership of the flash device.
 *
 * After initialization and before registration read, write, erase, jedec_id
 * and flash_type functions need to be set in the flashdev.
 *
 * @param[in] flash The flash device.
 * @param[in] flash_path The path to the flash device file.
 *
 * @retval 0 Successful operation.
 * @retval non-zero Failed operation.
 */
int rtems_flashdev_register(
  rtems_flashdev *flash,
  const char *flash_path
);

/**
 * @brief Destroys the flash device.
 *
 * @param[in] flash The flash device.
 */
void rtems_flashdev_destroy(
  rtems_flashdev *flash
);

/**
 * @brief Destroys the flash device and frees its memory.
 *
 * @param[in] flash The flash device.
 */
void rtems_flashdev_destroy_and_free(
  rtems_flashdev *flash
);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _DEV_FLASHDEV_H */
