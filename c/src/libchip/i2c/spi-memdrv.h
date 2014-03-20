/*===============================================================*\
| Project: SPI driver for spi memory devices                      |
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
| http://www.rtems.org/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
\*===============================================================*/


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
