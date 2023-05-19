/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * SPI driver for spi memory devices
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

#ifndef _LIBCHIP_SPI_MEMDRV_H
#define _LIBCHIP_SPI_MEMDRV_H

#include <rtems/libi2c.h>

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code spi_memdrv_write
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   write a block of data to memory                                         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_major_number minor,        /* minor device number            */
 void                      *arg          /* ptr to write argument struct   */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code spi_memdrv_read
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   read a block of data from memory                                        |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_major_number minor,        /* minor device number            */
 void                      *arg          /* ptr to read argument struct    */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/

  typedef struct {
    uint32_t baudrate;           /* tfr rate, bits per second     */
    bool     erase_before_program;
    uint32_t empty_state;        /* value of erased cells         */
    uint32_t page_size;          /* programming page size in byte */
    uint32_t sector_size;        /* erase sector size in byte     */
    uint32_t mem_size;           /* total capacity in byte        */
  } spi_memdrv_param_t;

  typedef struct {
    rtems_libi2c_drv_t  libi2c_drv_entry;  /* general i2c/spi params */
    spi_memdrv_param_t  spi_memdrv_param;  /* private parameters     */
  } spi_memdrv_t;

  extern rtems_driver_address_table spi_memdrv_rw_ops;
  extern rtems_driver_address_table spi_memdrv_ro_ops;

#ifdef __cplusplus
}
#endif

#endif /* _LIBCHIP_SPI_MEMDRV_H */
