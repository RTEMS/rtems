/*===============================================================*\
| Project: SPI driver for M25P40 like spi flash device            |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
\*===============================================================*/

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
