/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup raspberrypi_4_spi
 *
 * @brief Raspberry Pi specific SPI definitions.
 */

/*
 * Copyright (C) 2024 Ning Yang
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

#ifndef LIBBSP_AARCH64_RASPBERRYPI_4_SPI_H
#define LIBBSP_AARCH64_RASPBERRYPI_4_SPI_H

#include <bsp/utility.h>
#include <bsp/rpi-gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
  uint32_t spics;
#define RPI_SPICS_LEN_LONG             BSP_BIT32(25)
#define RPI_SPICS_DMA_LEN              BSP_BIT32(24)
#define RPI_SPICS_CSPOL2               BSP_BIT32(23)
#define RPI_SPICS_CSPOL1               BSP_BIT32(22)
#define RPI_SPICS_CSPOL0               BSP_BIT32(21)
#define RPI_SPICS_RXF                  BSP_BIT32(20)
#define RPI_SPICS_RXR                  BSP_BIT32(19)
#define RPI_SPICS_TXD                  BSP_BIT32(18)
#define RPI_SPICS_RXD                  BSP_BIT32(17)
#define RPI_SPICS_DONE                 BSP_BIT32(16)
#define RPI_SPICS_LEN                  BSP_BIT32(13)
#define RPI_SPICS_REN                  BSP_BIT32(12)
#define RPI_SPICS_ADCS                 BSP_BIT32(11)
#define RPI_SPICS_INTR                 BSP_BIT32(10)
#define RPI_SPICS_INTD                 BSP_BIT32(9)
#define RPI_SPICS_DMAEN                BSP_BIT32(8)
#define RPI_SPICS_TA                   BSP_BIT32(7)
#define RPI_SPICS_CSPOL                BSP_BIT32(6)
#define RPI_SPICS_CLEAR_TX             BSP_BIT32(5)
#define RPI_SPICS_CLEAR_RX             BSP_BIT32(4)
#define RPI_SPICS_CPOL                 BSP_BIT32(3)
#define RPI_SPICS_CPHA                 BSP_BIT32(2)
#define RPI_SPICS_CS(val)              BSP_FLD32(val, 0, 1)
#define RPI_SPICS_CS_SET(reg,val)      BSP_FLD32SET(reg, val, 0, 1)
  uint32_t spififo;
#define RPI_SPIFIFO_DATA(val)          BSP_FLD32(val, 0, 31)
#define RPI_SPIFIFO_DATA_GET(reg)      BSP_FLD32GET(reg, 0, 31)
#define RPI_SPIFIFO_DATA_SET(reg, val) BSP_FLD32SET(reg, val, 0, 31)
  uint32_t spiclk;
#define RPI_SPICLK_CDIV(val)           BSP_FLD32(val, 0, 15)
#define RPI_SPICLK_CDIV_GET(reg)       BSP_FLD32GET(reg, 0, 15)
#define RPI_SPICLK_CDIV_SET(reg, val)  BSP_FLD32SET(reg, val, 0, 15)
  uint32_t spidlen;
#define RPI_SPIDLEN_LEN(val)           BSP_FLD32(val, 0, 15)
#define RPI_SPIDLEN_LEN_GET(reg)       BSP_FLD32GET(reg, 0, 15)
#define RPI_SPIDLEN_LEN_SET(reg, val)  BSP_FLD32SET(reg, val, 0, 15)
  uint32_t spiltoh;
#define RPI_SPILTOH_TOH(val)           BSP_FLD32(val, 0, 3)
#define RPI_SPILTOH_TOH_GET(reg)       BSP_FLD32GET(reg, 0, 3)
#define RPI_SPILTOH_TOH_SET(reg, val)  BSP_FLD32SET(reg, val, 0, 3)
  uint32_t spidc;
#define RPI_SPIDC_RPANIC(val)          BSP_FLD32(val, 24, 31)
#define RPI_SPIDC_RPANIC_GET(reg)      BSP_FLD32GET(reg, 24, 31)
#define RPI_SPIDC_RPANIC_SET(reg, val) BSP_FLD32SET(reg, val, 24, 31)
#define RPI_SPIDC_RDREQ(val)           BSP_FLD32(val, 16, 23)
#define RPI_SPIDC_RDREQ_GET(reg)       BSP_FLD32GET(reg, 16, 23)
#define RPI_SPIDC_RDREQ_SET(reg, val)  BSP_FLD32SET(reg, val, 16, 23)
#define RPI_SPIDC_TPANIC(val)          BSP_FLD32(val, 8, 15)
#define RPI_SPIDC_TPANIC_GET(reg)      BSP_FLD32GET(reg, 8, 15)
#define RPI_SPIDC_TPANIC_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define RPI_SPIDC_TDREQ(val)           BSP_FLD32(val, 0, 7)
#define RPI_SPIDC_TDREQ_GET(reg)       BSP_FLD32GET(reg, 0, 7)
#define RPI_SPIDC_TDREQ_SET(reg, val)  BSP_FLD32SET(reg, val, 0, 7)
} raspberrypi_spi;

typedef enum {
  raspberrypi_SPI0,
  raspberrypi_SPI3,
  raspberrypi_SPI4,
  raspberrypi_SPI5,
  raspberrypi_SPI6
} raspberrypi_spi_device;

/**
 * @brief Register a spi device.
 *
 * @param device The optional devices are raspberrypi_SPI0, raspberrypi_SPI3,
 *               raspberrypi_SPI4, raspberrypi_SPI5, raspberrypi_SPI6.
 *
 * @retval RTEMS_SUCCESSFUL Successfully registered SPI device.
 * @retval RTEMS_INVALID_NUMBER This status code indicates that a specified
 *         number was invalid.
 * @retval RTEMS_UNSATISFIED This status code indicates that the request was
 *         not satisfied.
 */
rtems_status_code raspberrypi_spi_init(raspberrypi_spi_device device);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AARCH64_RASPBERRYPI_4_SPI_H */