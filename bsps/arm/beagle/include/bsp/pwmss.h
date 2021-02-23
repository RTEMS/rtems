/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief Shared PWMSS module functions used by PWM, eQEP and eCAP (when added).
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020, 2021 James Fitzsimons <james.fitzsimons@gmail.com>
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

#ifndef LIBBSP_ARM_BEAGLE_PWMSS_H
#define LIBBSP_ARM_BEAGLE_PWMSS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* The following definitions are bitmasks for the clk control registers for
 * the PWMSS module clocks. All three modules have the same clock control
 * hence the EPMSSx to signify these values are consistent across all
 * EPWMSS instances. */
#define AM335X_CM_PER_EPWMSSx_CLKCTRL_MODULEMODE_ENABLE   (0x2u)
#define AM335X_CM_PER_EPWMSSx_CLKCTRL_MODULEMODE   (0x00000003u)
#define AM335X_CM_PER_EPWMSSx_CLKCTRL_IDLEST_FUNC  (0x0u)
#define AM335X_CM_PER_EPWMSSx_CLKCTRL_IDLEST_SHIFT (0x00000010u)
#define AM335X_CM_PER_EPWMSSx_CLKCTRL_IDLEST       (0x00030000u)

/**
 * @brief The set of possible PWM subsystem module
 *
 * Enumerated type to define various instance of pwm module.
 */
typedef enum {
  BBB_PWMSS0 = 0,
  BBB_PWMSS1,
  BBB_PWMSS2,
  BBB_PWMSS_COUNT
} BBB_PWMSS;


rtems_status_code pwmss_module_clk_config(BBB_PWMSS pwmss_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_PWMSS_H */
