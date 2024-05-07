/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023, 2023 Aaron Nyholm
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

#ifndef XILINX_XQSPI_FLASH_H
#define XILINX_XQSPI_FLASH_H

#include <dev/flash/flashdev.h>
#include <dev/spi/xqspipsu.h>

#define XQSPI_FLASH_MAX_REGIONS ((size_t)32)

/*
 * @brief Initializes a flash device using Xilinx's xqspi flash
 * driver. The flash device is not registered in this call.
 * If an rtems_flashdev is created using xqspi_flash_init it must be
 * destroyed using xqspi_flash_destroy.
 *
 * @param[in] xQspiDev A configured XQspiPsu device to initialise.
 *
 * @retval A pointer to the rtems_flashdev.
 * @retval NULL on failure.
*/
rtems_flashdev* xqspi_flash_init(XQspiPsu *xQspiDev);

/*
 * @brief Destroys a rtems_flashdev initialised with xqspi_flash_init.
 * If an rtems_flashdev is created using xqspi_flash_init it must be
 * destroyed using xqspi_flash_destroy. The XQspiPsu originally passed in
 * is untouched.
 *
 * @param[in] flash The flashdev to destroy
*/
void xqspi_flash_destroy(rtems_flashdev* flash);

/*
 * @brief Struct allocating memory space for flash regions. Used by
 * rtems_flashdev to store region allocations.
 */
typedef struct xqspi_flash_region_table {
  rtems_flashdev_region xqspi_flash_regions[XQSPI_FLASH_MAX_REGIONS];
  uint32_t xqspi_flash_bit_allocator;
} xqspi_flash_region_table;

#endif /* XILINX_XQSPI_FLASH_H */
