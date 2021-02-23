/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief Shared PWMSS module functions used by PWM, eQEP and eCAP (when added).
 */

/**
 * Copyright (c) 2020 James Fitzsimons <james.fitzsimons@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *.
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
