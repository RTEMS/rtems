/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup DCAN
 *
 * @brief This source file provides AM335x-specific DCAN support.
 */

/*
 * Copyright (C) 2026 Ning Zhang <riverquiet66@gmail.com>
 * Copyright (C) 2026 Michal Lenc <michallenc@seznam.cz>
 * Copyright (C) 2026 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 * Based on DCAN driver code by Prashanth S <fishesprashanth@gmail.com>
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

#include <bsp/dcan_am335x.h>

#include <errno.h>
#include <stdint.h>

#define SOC_CM_PER_REGS 0x44E00000u
#define CM_PER_DCAN0_CLKCTRL 0xc0u
#define CM_PER_DCAN1_CLKCTRL 0xc4u

#define AM335X_PADCONF_BASE 0x44E10800u
#define AM335X_CONF_UART1_CTSN 0x178u
#define AM335X_CONF_UART1_RTSN 0x17cu
#define AM335X_CONF_UART1_RXD 0x180u
#define AM335X_CONF_UART1_TXD 0x184u

#define AM335X_PIN_RX_DISABLE ( 0u << 5 )
#define AM335X_PIN_RX_ENABLE ( 1u << 5 )
#define AM335X_PIN_PULL_ENABLE ( 0u << 3 )
#define AM335X_PIN_PULL_UP ( ( 1u << 4 ) | AM335X_PIN_PULL_ENABLE )
#define AM335X_PIN_MODE_2 2u

#define AM335X_MODULEMODE_MASK 0x3u
#define AM335X_MODULEMODE_ENABLE 0x2u

static uint32_t am335x_read_reg( uintptr_t address )
{
  return *( volatile uint32_t * ) address;
}

static void am335x_write_reg(
  uintptr_t address,
  uint32_t value
)
{
  *( volatile uint32_t * ) address = value;
}

/*
 * Configure BBB pins P9.19 / P9.20 for DCAN0.
 *
 * P9.20, UART1_CTSN mode 2: DCAN0_TX
 * P9.19, UART1_RTSN mode 2: DCAN0_RX
 */
static void dcan0_pinmux( void )
{
  am335x_write_reg(
    AM335X_PADCONF_BASE + AM335X_CONF_UART1_CTSN,
    AM335X_PIN_PULL_UP |
      AM335X_PIN_RX_DISABLE |
      AM335X_PIN_MODE_2
  );

  am335x_write_reg(
    AM335X_PADCONF_BASE + AM335X_CONF_UART1_RTSN,
    AM335X_PIN_PULL_UP |
      AM335X_PIN_RX_ENABLE |
      AM335X_PIN_MODE_2
  );
}

/*
 * Configure BBB pins P9.24 / P9.26 for DCAN1.
 *
 * P9.26, UART1_RXD mode 2: DCAN1_TX
 * P9.24, UART1_TXD mode 2: DCAN1_RX
 */
static void dcan1_pinmux( void )
{
  am335x_write_reg(
    AM335X_PADCONF_BASE + AM335X_CONF_UART1_RXD,
    AM335X_PIN_PULL_UP |
      AM335X_PIN_RX_DISABLE |
      AM335X_PIN_MODE_2
  );

  am335x_write_reg(
    AM335X_PADCONF_BASE + AM335X_CONF_UART1_TXD,
    AM335X_PIN_PULL_UP |
      AM335X_PIN_RX_ENABLE |
      AM335X_PIN_MODE_2
  );
}

/**
 * @brief Enables an AM335x DCAN peripheral clock.
 *
 * @param clkctrl_offset Offset of the DCAN clock control register
 *        relative to the CM_PER register base.
 *
 * @return 0 on success, negated ETIMEDOUT otherwise.
 */
static int dcan_clock_enable( uint32_t clkctrl_offset )
{
  uintptr_t clkctrl_address;
  uint32_t clkctrl;
  int retries;

  clkctrl_address = SOC_CM_PER_REGS + clkctrl_offset;
  retries = 100;

  am335x_write_reg(
    clkctrl_address,
    AM335X_MODULEMODE_ENABLE
  );

  while ( retries > 0 ) {
    clkctrl = am335x_read_reg( clkctrl_address );

    if (
      ( clkctrl & AM335X_MODULEMODE_MASK ) ==
        AM335X_MODULEMODE_ENABLE
    ) {
      return 0;
    }

    --retries;
  }

  return -ETIMEDOUT;
}

void rtems_am335x_dcan_initialize( void )
{
  int ret;

  ret = dcan_clock_enable( CM_PER_DCAN1_CLKCTRL );
  if ( ret < 0 ) {
    return;
  }

  dcan1_pinmux();

 /*
  * Initialize DCAN0.
  */
  ret = dcan_clock_enable( CM_PER_DCAN0_CLKCTRL );
  if ( ret < 0 ) {
    return;
  }

  dcan0_pinmux();

}
