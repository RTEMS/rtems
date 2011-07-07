/*===============================================================*\
| Project: SPI driver for FM25L256 like spi fram device           |
+-----------------------------------------------------------------+
|                    Copyright (c) 2008                           |
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

#include <libchip/spi-fram-fm25l256.h>
#include <rtems/libio.h>


static spi_memdrv_t spi_fram_fm25l256_rw_drv_t = {
  {/* public fields */
    .ops =         &spi_memdrv_rw_ops, /* operations of general memdrv */
    .size =        sizeof (spi_fram_fm25l256_rw_drv_t),
  },
  { /* our private fields */
    .baudrate =             2000000,
    .erase_before_program = false,
    .empty_state =          0xff,
    .page_size =            0x8000, /* 32K programming page size in bytes */
    .sector_size =          1,      /* erase sector size in bytes */
    .mem_size =             0x8000, /* 32K total capacity in bytes */
  }
};

rtems_libi2c_drv_t *spi_fram_fm25l256_rw_driver_descriptor =
&spi_fram_fm25l256_rw_drv_t.libi2c_drv_entry;

static spi_memdrv_t spi_fram_fm25l256_ro_drv_t = {
  {/* public fields */
    .ops =         &spi_memdrv_ro_ops, /* operations of general memdrv */
    .size =        sizeof (spi_fram_fm25l256_ro_drv_t),
  },
  { /* our private fields */
    .baudrate =             2000000,
    .erase_before_program = false,
    .empty_state =          0xff,
    .page_size =            0x8000, /* 32k programming page size in bytes */
    .sector_size =          1,      /* erase sector size in bytes */
    .mem_size =             0x8000, /* 32k total capacity in bytes */
  }
};

rtems_libi2c_drv_t *spi_fram_fm25l256_ro_driver_descriptor =
&spi_fram_fm25l256_ro_drv_t.libi2c_drv_entry;
