/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Raspberrypi4
 *
 * @brief Raspberry Pi specific PWM definitions.
 */

/*
 * Copyright (C) 2025 Shaunak Datar
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

#ifndef LIBBSP_AARCH64_RASPBERRYPI_4_PWM_H
#define LIBBSP_AARCH64_RASPBERRYPI_4_PWM_H

#include "bsp/raspberrypi.h"
#include "bsp/rpi-gpio.h"
#include "bsp/utility.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { raspberrypi_pwm0, raspberrypi_pwm1 } raspberrypi_pwm_channel;

typedef enum {
  raspberrypi_pwm_master0,
  raspberrypi_pwm_master1
} raspberrypi_pwm_master;

/**
 * @name PWM_CONTROL register bits
 * @{
 */
#define C_MSEN2 BSP_BIT32( 15 ) /**< Channel 2 Mark-Space enable  */
#define C_USEF2 BSP_BIT32( 13 ) /**< Channel 2 use FIFO           */
#define C_POLA2 BSP_BIT32( 12 ) /**< Channel 2 invert polarity    */
#define C_SBIT2 BSP_BIT32( 11 ) /**< Channel 2 silence bit high   */
#define C_RPTL2 BSP_BIT32( 10 ) /**< Channel 2 repeat on underrun */
#define C_MODE2 BSP_BIT32( 9 )  /**< Channel 2 serializer mode    */
#define C_PWEN2 BSP_BIT32( 8 )  /**< Channel 2 enable output      */
#define C_MSEN1 BSP_BIT32( 7 )  /**< Channel 1 Mark-Space enable  */
#define C_CLRF BSP_BIT32( 6 )   /**< Clear FIFO              */
#define C_USEF1 BSP_BIT32( 5 )  /**< Channel 1 use FIFO           */
#define C_POLA1 BSP_BIT32( 4 )  /**< Channel 1 invert polarity    */
#define C_SBIT1 BSP_BIT32( 3 )  /**< Channel 1 silence bit high   */
#define C_RPTL1 BSP_BIT32( 2 )  /**< Channel 1 repeat underrun    */
#define C_MODE1 BSP_BIT32( 1 )  /**< Channel 1 serializer mode    */
#define C_PWEN1 BSP_BIT32( 0 )  /**< Channel 1 enable output      */
/** @} */

/**
 * @brief Set PWM clock divider.
 * @param divisor  1 – 4095; PWMCLK = 19.2 MHz / @p divisor.
 * @retval RTEMS_SUCCESSFUL        OK
 * @retval RTEMS_INVALID_NUMBER    0 or >4095
 */
rtems_status_code rpi_pwm_set_clock( uint32_t divisor );

/**
 * @brief Update duty-cycle register.
 * @param master   Selects the hardware instance to be used
 * (raspberrypi_pwm_master0 = PWM0, raspberrypi_pwm_master1 = PWM1)
 * @param channel  Selects the channel for @p master (raspberrypi_pwm0 = PWMx_0,
 * raspberrypi_pwm1 = PWMx_1)
 * @param data     Initial duty count, 1 – current range value.(0 rejected)
 */
rtems_status_code rpi_pwm_set_data(
  raspberrypi_pwm_master  master,
  raspberrypi_pwm_channel channel,
  uint32_t                data
);

/**
 * @brief Main PWM initialization function. This functions sets up the PWM
 * master, channel, duty cycle and GPIO pin.
 * @param master   Selects the hardware instance to be used
 * (raspberrypi_pwm_master0 = PWM0, raspberrypi_pwm_master1 = PWM1)
 * @param channel  Selects the channel for @p master (raspberrypi_pwm0 = PWMx_0,
 * raspberrypi_pwm1 = PWMx_1)
 * @param range    Period register value (> 0)
 * @param data     Initial duty count, 1 – @p range (0 rejected)
 */
rtems_status_code rpi_pwm_init(
  raspberrypi_pwm_master  master,
  raspberrypi_pwm_channel channel,
  uint32_t                range,
  uint32_t                data
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AARCH64_RASPBERRYPI_4_PWM_H */