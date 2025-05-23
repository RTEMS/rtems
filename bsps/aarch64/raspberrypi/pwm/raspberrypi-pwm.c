/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Raspberrypi4
 *
 * @brief PWM Support
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

#include "bsp/raspberrypi-pwm.h"

// Clock manager macros
#define BCM2711_CM_PWM( x ) BCM2835_REG( BCM2711_CM_PWM_BASE + ( x ) )
#define CM_PWM_CTL_ENABLE_OSC \
  ( CM_PWM_PASSWD | CM_PWM_CTL_ENAB | CM_PWM_CTL_SRC_OSC )
#define CM_PWM_CTL_DISABLE ( CM_PWM_PASSWD | CM_PWM_CTL_SRC_OSC )

static inline bool rpi_pwm_validate(
  raspberrypi_pwm_master  master,
  raspberrypi_pwm_channel channel
)
{
  return ( ( master == raspberrypi_pwm_master0 ) ||
           ( master == raspberrypi_pwm_master1 ) ) &&
         ( ( channel == raspberrypi_pwm0 ) || ( channel == raspberrypi_pwm1 ) );
}

rtems_status_code rpi_pwm_set_clock( uint32_t divisor )
{
  if ( !( divisor > 0 && divisor < 4096 ) ) {
    return RTEMS_INVALID_NUMBER;
  }

  /* Stop Clock */
  BCM2711_CM_PWM( BCM2711_CM_PWM_CTL ) = CM_PWM_CTL_DISABLE;
  while ( BCM2711_CM_PWM( BCM2711_CM_PWM_CTL ) & CM_PWM_CTL_BUSY );

  /* Set divisor */
  divisor                              &= CM_PWM_DIV_MASK;
  BCM2711_CM_PWM( BCM2711_CM_PWM_DIV )  = CM_PWM_PASSWD | ( divisor << 12 );

  /* Select src = osc(1) and enable */
  BCM2711_CM_PWM( BCM2711_CM_PWM_CTL ) = CM_PWM_CTL_ENABLE_OSC;
  while ( !( BCM2711_CM_PWM( BCM2711_CM_PWM_CTL ) & CM_PWM_CTL_BUSY ) );

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code rpi_pwm_set_control(
  raspberrypi_pwm_master  master,
  raspberrypi_pwm_channel channel
)
{
  uint32_t pwm_base    = ( master == raspberrypi_pwm_master0 ) ?
                           BCM2711_PWM0_BASE :
                           BCM2711_PWM1_BASE;
  uint32_t control_reg = pwm_base + BCM2711_PWM_CONTROL;
  uint32_t control     = BCM2835_REG( control_reg );

  if ( channel == raspberrypi_pwm0 ) {
    control &= ~( C_MODE1 | C_POLA1 | C_SBIT1 | C_RPTL1 | C_USEF1 );
    control |= ( C_PWEN1 | C_CLRF | C_MSEN1 );
  } else {
    control &= ~( C_MODE2 | C_POLA2 | C_SBIT2 | C_RPTL2 | C_USEF2 );
    control |= ( C_PWEN2 | C_CLRF | C_MSEN2 );
  }
  BCM2835_REG( control_reg ) = control;
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code rpi_pwm_set_range(
  raspberrypi_pwm_master  master,
  raspberrypi_pwm_channel channel,
  uint32_t                range
)
{
  uint32_t pwm_base     = ( master == raspberrypi_pwm_master0 ) ?
                            BCM2711_PWM0_BASE :
                            BCM2711_PWM1_BASE;
  uint32_t range_offset = ( channel == raspberrypi_pwm0 ) ? BCM2711_PWM_RNG1 :
                                                            BCM2711_PWM_RNG2;

  BCM2835_REG( pwm_base + range_offset ) = range;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rpi_pwm_set_data(
  raspberrypi_pwm_master  master,
  raspberrypi_pwm_channel channel,
  uint32_t                data
)
{
  if ( !( rpi_pwm_validate( master, channel ) ) || data == 0 ) {
    return RTEMS_INVALID_NUMBER;
  }

  uint32_t pwm_base     = ( master == raspberrypi_pwm_master0 ) ?
                            BCM2711_PWM0_BASE :
                            BCM2711_PWM1_BASE;
  uint32_t range_offset = ( channel == raspberrypi_pwm0 ) ? BCM2711_PWM_RNG1 :
                                                            BCM2711_PWM_RNG2;

  if ( data > BCM2835_REG( pwm_base + range_offset ) ) {
    return RTEMS_INVALID_NUMBER;
  }

  uint32_t data_offset = ( channel == raspberrypi_pwm0 ) ? BCM2711_PWM_DAT1 :
                                                           BCM2711_PWM_DAT2;

  BCM2835_REG( pwm_base + data_offset ) = data;
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code rpi_pwm_set_gpio(
  raspberrypi_pwm_master  master,
  raspberrypi_pwm_channel channel
)
{
  rtems_status_code sc;

  if ( master == raspberrypi_pwm_master0 ) {
    if ( channel == raspberrypi_pwm0 ) {
      sc = raspberrypi_gpio_set_function( 18, GPIO_AF5 );
    } else {
      sc = raspberrypi_gpio_set_function( 19, GPIO_AF5 );
    }
  } else {
    if ( channel == raspberrypi_pwm0 ) {
      sc = raspberrypi_gpio_set_function( 40, GPIO_AF0 );
    } else {
      sc = raspberrypi_gpio_set_function( 41, GPIO_AF0 );
    }
  }

  return sc;
}

rtems_status_code rpi_pwm_init(
  raspberrypi_pwm_master  master,
  raspberrypi_pwm_channel channel,
  uint32_t                range,
  uint32_t                data
)
{
  rtems_status_code sc;

  if ( !( rpi_pwm_validate( master, channel ) ) || range == 0 ) {
    return RTEMS_INVALID_NUMBER;
  }

  sc = rpi_pwm_set_gpio( master, channel );
  if ( sc != RTEMS_SUCCESSFUL ) {
    return sc;
  }

  sc = rpi_pwm_set_range( master, channel, range );
  if ( sc != RTEMS_SUCCESSFUL ) {
    return sc;
  }

  sc = rpi_pwm_set_data( master, channel, data );
  if ( sc != RTEMS_SUCCESSFUL ) {
    return sc;
  }

  sc = rpi_pwm_set_control( master, channel );
  if ( sc != RTEMS_SUCCESSFUL ) {
    return sc;
  }

  return RTEMS_SUCCESSFUL;
}