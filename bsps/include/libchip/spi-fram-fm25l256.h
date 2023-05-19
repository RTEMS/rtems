/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * SPI driver for FM25L256 like spi fram device
 *
 * FIXME: currently, this driver only supports read/write accesses
 * erase accesses are to be completed
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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

#ifndef _LIBCHIP_SPI_FRAM_FM25L256_H
#define _LIBCHIP_SPI_FRAM_FM25L256_H

#include <libchip/spi-memdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * pass one of these descriptor pointers to rtems_libi2c_register_drv
 */
extern rtems_libi2c_drv_t *spi_fram_fm25l256_rw_driver_descriptor;

extern rtems_libi2c_drv_t *spi_fram_fm25l256_ro_driver_descriptor;

#ifdef __cplusplus
}
#endif

#endif /* _LIBCHIP_SPI_FRAM_FM25L256_H */
