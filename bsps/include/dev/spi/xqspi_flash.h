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

#ifdef __cplusplus
extern "C" {
#endif

#define XQSPI_FLASH_MAX_REGIONS ((size_t)32)

/*
 * @brief Initializes a flash device using Xilinx's xqspi flash driver.
 *
 * The flash device is not registered in this call. The returned object must be
 * destroyed with rtems_flashdev_destroy_and_free if it has not been registered.
 *
 * @param[in] xQspiDev A configured XQspiPsu device to initialise.
 *
 * @retval A pointer to the rtems_flashdev.
 * @retval NULL on failure.
*/
rtems_flashdev* xqspi_flash_init(XQspiPsu *xQspiDev);

#ifdef __cplusplus
}
#endif

#endif /* XILINX_XQSPI_FLASH_H */
