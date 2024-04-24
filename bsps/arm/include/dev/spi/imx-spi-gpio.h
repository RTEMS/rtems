/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceIMXSPIGPIO
 *
 * @brief This header file provides the interfaces of
 *   @ref RTEMSDeviceIMXSPIGPIO.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_DEV_SPI_IMX_SPI_GPIO_H
#define _RTEMS_DEV_SPI_IMX_SPI_GPIO_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSDeviceIMXSPIGPIO
 *
 * @ingroup RTEMSDeviceSPIGPIO
 *
 * @brief A SPI bus driver that uses the i.MX/i.MXRT GPIO pins
 *
 * @{
 */

/**
 * @brief Register all compatible nodes from the device tree.
 *
 * Search nodes in the device tree that are compatible to 'spi-gpio' and try to
 * register the driver for them. Expects a node that looks (for example) like
 * the following:
 *
 * ```
 * gpiospi: spi {
 * 	status = "okay";
 * 	compatible = "spi-gpio";
 * 	#address-cells = <0x1>;
 * 	#size-cells = <0x0>;
 * 
 * 	sck-gpios = <&gpio2 27 0>;
 * 	miso-gpios = <&gpio2 25 0>;
 * 	mosi-gpios = <&gpio4 14 0>;
 * 	cs-gpios = <&gpio4 10 1>;
 * 	num-chipselects = <1>;
 * 
 * 	rtems,path = "/dev/gpiospi";
 * };
 * ```
 */
rtems_status_code imx_spi_gpio_init(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_DEV_SPI_IMX_SPI_GPIO_H */
