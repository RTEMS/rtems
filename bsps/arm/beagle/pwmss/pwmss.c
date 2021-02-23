/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief Support for eQEP for the BeagleBone Black.
 */

/**
 * Copyright (c) 2020 James Fitzsimons <james.fitzsimons@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <libcpu/am335x.h>
#include <stdio.h>
#include <bsp.h>
#include <bsp/pwmss.h>
#include <bsp/beagleboneblack.h>


/**
 * @brief   This function configures the L3 and L4_PER system clocks.
 *          It also configures the system clocks for the specified ePWMSS
 *          instance.
 *
 * @param   pwmss_id    The instance number of ePWMSS whose system clocks
 *                         have to be configured.
 *
 * 'pwmss_id' can take one of the following values:
 * (0 <= pwmss_id <= 2)
 *
 * @return  True if successful
 *          False if Unsuccessful
 */
rtems_status_code pwmss_module_clk_config(BBB_PWMSS pwmss_id)
{
    uint32_t clkctrl;

    /* calculate the address of the clock control register for the PWMSS
     * module we are configuring */
    if(pwmss_id == BBB_PWMSS0) {
        clkctrl = AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS0_CLKCTRL;
    } else if(pwmss_id == BBB_PWMSS1) {
        clkctrl = AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS1_CLKCTRL;
    } else if(pwmss_id == BBB_PWMSS2) {
        clkctrl = AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS2_CLKCTRL;
    }

    /* when the module is functional the IDLEST bits (16 -17) of the
     * CM_PER_EPWMSSx_CLKCTRL register will be 0x0. */
    const uint32_t is_functional = 0x0;
    const uint32_t idle_bits = AM335X_CM_PER_EPWMSSx_CLKCTRL_IDLEST;
    const uint32_t is_enable = AM335X_CM_PER_EPWMSSx_CLKCTRL_MODULEMODE_ENABLE;
    const uint32_t module_mode = AM335X_CM_PER_EPWMSSx_CLKCTRL_MODULEMODE;

    REG(clkctrl) |= is_enable;
    while((REG(clkctrl) & module_mode) != is_enable);
    while((REG(clkctrl) & idle_bits) != is_functional);

    return RTEMS_SUCCESSFUL;
}
