/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RaspberryPi
 *
 * @brief Watchdog Driver
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

#include <bsp/raspberrypi.h>
#include <bsp/watchdog.h>

#define PM_WDOG BCM2835_REG(BCM2711_PM_WDOG)
#define PM_RSTC BCM2835_REG(BCM2711_PM_RSTC)

uint32_t raspberrypi_watchdog_timeout;

void raspberrypi_watchdog_init()
{
  raspberrypi_watchdog_timeout = 0;
}

void raspberrypi_watchdog_start(uint32_t timeout_ms)
{
  raspberrypi_watchdog_timeout = timeout_ms;

  PM_WDOG = BCM2711_PM_PASSWD_MAGIC |
            ((timeout_ms * 65536 / 1000) & BCM2711_PM_WDOG_MASK);

  PM_RSTC &= BCM2711_PM_RSTC_WRCFG_CLR;
  PM_RSTC = (BCM2711_PM_PASSWD_MAGIC | BCM2711_PM_RSTC_WRCFG_FULL);
}

void raspberrypi_watchdog_stop()
{
  PM_RSTC = BCM2711_PM_PASSWD_MAGIC | BCM2711_PM_RSTC_RESET;
}

void raspberrypi_watchdog_reload()
{
  raspberrypi_watchdog_start(raspberrypi_watchdog_timeout);
}

uint32_t raspberrypi_watchdog_get_remaining_time()
{
  return (PM_WDOG & BCM2711_PM_WDOG_MASK)*1000/65536;
}