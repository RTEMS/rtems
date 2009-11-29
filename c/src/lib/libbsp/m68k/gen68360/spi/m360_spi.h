/*===============================================================*\
| Project: RTEMS support for MC68360                              |
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
| this file contains the MC68360 SPI driver declarations          |
\*===============================================================*/
#ifndef _M360_SPIDRV_H
#define _M360_SPIDRV_H

#include <rtems/m68k/m68360.h>
#include <rtems/libi2c.h>
#include <rtems/irq.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct m360_spi_softc {
  int                     initialized;
  rtems_id                irq_sema_id;
  rtems_isr_entry         old_handler;
  m360BufferDescriptor_t *rx_bd;
  m360BufferDescriptor_t *tx_bd;
} m360_spi_softc_t ;

typedef struct {
  rtems_libi2c_bus_t  bus_desc;
  m360_spi_softc_t softc;
} m360_spi_desc_t;

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code m360_spi_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the driver                                                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh                  /* bus specifier structure        */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int m360_spi_read_bytes
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   receive some bytes from SPI device                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *buf,                     /* buffer to store bytes          */
 int len                                 /* number of bytes to receive     */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    number of bytes received or (negative) error code                      |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int m360_spi_write_bytes
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   send some bytes to SPI device                                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *buf,                     /* buffer to send                 */
 int len                                 /* number of bytes to send        */

 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    number of bytes sent or (negative) error code                          |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code m360_spi_set_tfr_mode
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   set SPI to desired baudrate/clock mode/character mode                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 const rtems_libi2c_tfr_mode_t *tfr_mode /* transfer mode info             */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int m360_spi_ioctl
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   perform selected ioctl function for SPI                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 int                 cmd,                /* ioctl command code             */
 void               *arg                 /* additional argument array      */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/

#ifdef __cplusplus
}
#endif


#endif /* _M360_SPIDRV_H */
