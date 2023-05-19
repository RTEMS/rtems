/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * SPI driver for M25P40 like spi flash device.
 */

/*
 * Copyright (c) 2007 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <rtems/libi2c.h>

#include <libchip/spi-flash-m25p40.h>
#include <rtems/libio.h>


static spi_memdrv_t spi_flash_m25p40_rw_drv_t = {
  {/* public fields */
    .ops =         &spi_memdrv_rw_ops, /* operations of general memdrv */
    .size =        sizeof (spi_flash_m25p40_rw_drv_t),
  },
  { /* our private fields */
    .baudrate =             2000000,
    .erase_before_program = true,
    .empty_state =          0xff,
    .page_size =            256,     /* programming page size in bytes */
    .sector_size =          0x10000, /* 64K - erase sector size in bytes */
    .mem_size =             0x80000, /* 512K - total capacity in bytes */
  }
};

rtems_libi2c_drv_t *spi_flash_m25p40_rw_driver_descriptor =
&spi_flash_m25p40_rw_drv_t.libi2c_drv_entry;

static spi_memdrv_t spi_flash_m25p40_ro_drv_t = {
  {/* public fields */
    .ops =         &spi_memdrv_ro_ops, /* operations of general memdrv */
    .size =        sizeof (spi_flash_m25p40_ro_drv_t),
  },
  { /* our private fields */
    .baudrate =             2000000,
    .erase_before_program = true,
    .empty_state =          0xff,
    .page_size =            256,    /* programming page size in bytes */
    .sector_size =          0x10000, /* 64K erase sector size in bytes */
    .mem_size =             0x80000, /* 512K total capacity in bytes */
  }
};

rtems_libi2c_drv_t *spi_flash_m25p40_ro_driver_descriptor =
&spi_flash_m25p40_ro_drv_t.libi2c_drv_entry;
