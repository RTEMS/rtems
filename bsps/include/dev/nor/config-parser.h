/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
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

#ifndef _DEV_NOR_CONFIG_FLASH_H
#define _DEV_NOR_CONFIG_FLASH_H

#include <rtems/rtems/status.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSGenericFlashParsers Generic Flash Parsers
 *
 * @ingroup RTEMSGenericFlashAPI
 *
 * @brief SFDP and CFI parsers.
 *
 * @{
 */

/**
 * @brief This struct holds the information parsed from the Common Flash Memory
 * Information (CFI) read from a flash chip.
 */
typedef struct rtems_flash_NOR_config_data {
  /* @brief JEDEC ID */
  uint32_t jedec_id;
  /* @brief Sector size in bytes */
  uint32_t sector_size;
  /* @brief Number of sectors */
  uint32_t num_sectors;
  /* @brief Page size in bytes. This will default to 256 if unavailable. */
  uint16_t page_size;
  /* @brief This is the size of the flash device */
  uint64_t device_size;
  /* @brief Alternative Sector size in bytes. This will be 0 if the alternative
   * sector size is unsupported or unavailable.
   */
  uint32_t alt_sector_size;
  /* @brief The command byte to erase sectors in the alternative size */
  uint8_t alt_sector_cmd;
  /* @brief Number of sectors for the alternative sector size */
  uint32_t num_alt_sectors;
} rtems_flash_NOR_config_data;

/**
 *
 * @brief This function parses the provided buffer of CFI data into a
 * rtems_flash_NOR_config_data structure.
 *
 * @param  cfi_raw is a buffer containing CFI data.
 * @param  cfi_raw_len is the length of the data in cfi_raw.
 * @param  data is a pointer to a rtems_flash_NOR_config_data struct to
 *         be filled with data about the flash chip.
 *
 * @return RTEMS_SUCCESSFUL if successful.
 */
rtems_status_code rtems_flash_CFI_parse_from_buffer(
  uint8_t *cfi_raw,
  size_t cfi_raw_len,
  rtems_flash_NOR_config_data *data
);

/**
 *
 * @brief This function parses the provided buffer of SFDP data into a
 * rtems_flash_NOR_config_data structure.
 *
 * @param  sfdp_raw is a buffer containing SFDP data.
 * @param  sfdp_raw_len is the length of the data in sfdp_raw.
 * @param  data is a pointer to a rtems_flash_NOR_config_data struct to
 *         be filled with data about the flash chip.
 *
 * @return RTEMS_SUCCESSFUL if successful.
 */
rtems_status_code rtems_flash_SFDP_parse_from_buffer(
  uint8_t *sfdp_raw,
  size_t sfdp_raw_len,
  rtems_flash_NOR_config_data *data
);

/**
 * @brief acquire data from the NOR chip.
 *
 * @param[in] context The context data used to retrieve bytes.
 * @param[in] offset The offset to read from the flash begin in bytes.
 * @param[in] length The size of the buffer in bytes.
 *
 * @retval NULL on failure.
 * @retval pointer to the requested data on success.
 */
typedef uint8_t *(*rtems_flash_NOR_config_resource_acquire)(
  void *context,
  uint32_t offset,
  size_t length
);

/**
 * @brief release data acquired from the NOR chip.
 *
 * @param[in] context The context data used to retrieve bytes.
 * @param[in] data The data previously acquired.
 */
typedef void (*rtems_flash_NOR_config_resource_release)(
  void *context,
  uint8_t *data
);

/**
 * @brief This struct allows access to the flash data.
 */
typedef struct rtems_flash_NOR_config_accessor {
  /* @brief The context provided to the acquire and release functions */
  void *context;
  /* @brief The function used to acquire a block of data */
  rtems_flash_NOR_config_resource_acquire acquire;
  /* @brief The function used to release a block of data */
  rtems_flash_NOR_config_resource_release release;
} rtems_flash_NOR_config_accessor;

/**
 *
 * @brief This function parses a SFDP configuration space into a
 * rtems_flash_NOR_config_data structure.
 *
 * @param  accessor is the accessor for the SFDP configuration space.
 * @param  data is a pointer to a NOR_config_data struct to be filled with data
 *         about the flash chip.
 *
 * @return RTEMS_SUCCESSFUL if successful.
 */
rtems_status_code rtems_flash_SFDP_parse(
  rtems_flash_NOR_config_accessor *accessor,
  rtems_flash_NOR_config_data *data
);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _DEV_NOR_CONFIG_FLASH_H */
