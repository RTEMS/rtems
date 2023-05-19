/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS support for MPC8xx
 *
 * This file contains the MPC8xx SPI driver declarations.
 */

/*
 * Copyright (c) 2009 embedded brains GmbH & Co. KG
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

#ifndef _M8XX_SPIDRV_H
#define _M8XX_SPIDRV_H

#include <mpc8xx.h>
#include <rtems/libi2c.h>
#include <rtems/irq.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct m8xx_spi_softc {
  int                     initialized;
  rtems_id                irq_sema_id;
  rtems_isr_entry         old_handler;
  m8xxBufferDescriptor_t *rx_bd;
  m8xxBufferDescriptor_t *tx_bd;
} m8xx_spi_softc_t ;

typedef struct {
  rtems_libi2c_bus_t  bus_desc;
  m8xx_spi_softc_t softc;
} m8xx_spi_desc_t;

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code m8xx_spi_init
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
int m8xx_spi_read_bytes
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
int m8xx_spi_write_bytes
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
rtems_status_code m8xx_spi_set_tfr_mode
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
int m8xx_spi_ioctl
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


#endif /* _M8XX_SPIDRV_H */
