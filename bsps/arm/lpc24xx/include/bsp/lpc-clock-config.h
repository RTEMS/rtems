/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 *
 * @brief Clock driver configuration.
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

#ifndef LIBBSP_ARM_LPC24XX_LPC_CLOCK_CONFIG_H
#define LIBBSP_ARM_LPC24XX_LPC_CLOCK_CONFIG_H

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/lpc24xx.h>
#include <bsp/io.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LPC_CLOCK_INTERRUPT LPC24XX_IRQ_TIMER_0

#define LPC_CLOCK_TIMER_BASE TMR0_BASE_ADDR

#define LPC_CLOCK_TIMECOUNTER_BASE TMR1_BASE_ADDR

#define LPC_CLOCK_REFERENCE LPC24XX_PCLK

#define LPC_CLOCK_MODULE_ENABLE() \
  lpc24xx_module_enable(LPC24XX_MODULE_TIMER_0, LPC24XX_MODULE_PCLK_DEFAULT)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_LPC_CLOCK_CONFIG_H */
