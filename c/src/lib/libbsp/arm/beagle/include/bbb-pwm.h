/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief BeagleBone Black BSP definitions.
 */

/**
 * Copyright (c) 2016 Punit Vara <punitvara at gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/** Some constants are taken from 
 * https://github.com/VegetableAvenger/BBBIOlib/blob/master/BBBio_lib/BBBiolib_PWMSS.h
 */

#ifndef LIBBSP_ARM_BEAGLE_BBB_PWM_H
#define LIBBSP_ARM_BEAGLE_BBB_PWM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief  BeagleBone Black PWM Macros.
 */
#define BBB_CONTROL_CONF_GPMC_AD(n)   (0x800 + (n * 4))
#define BBB_CONTROL_CONF_LCD_DATA(n)   (0x8a0 + (n * 4))

#define BBB_PWMSS_COUNT       3
#define BBB_PWMSS0  0
#define BBB_PWMSS1  1  
#define BBB_PWMSS2  2

#define BBB_P8_13_2B  3
#define BBB_P8_19_2A  4
#define BBB_P8_45_2A  5
#define BBB_P8_46_2B  6
#define BBB_P8_34_1B  7
#define BBB_P8_36_1A  8
#define BBB_P9_14_1A  9
#define BBB_P9_16_1B  10
#define BBB_P9_21_0B  11
#define BBB_P9_22_0A  12
#define BBB_P9_29_0B  13
#define BBB_P9_31_0A  14

#define BBB_MUX0      0
#define BBB_MUX1      1
#define BBB_MUX2      2
#define BBB_MUX3      3
#define BBB_MUX4      4
#define BBB_MUX5      5
#define BBB_MUX6      6
#define BBB_MUX7      7

#define BBB_EPWM1     1
#define BBB_EPWM2     2
#define BBB_EPWM0     0

/**
 * @brief  BeagleBone Black PWM API.
 */

/**
 * @brief This function intilize clock and pinmuxing for pwm sub system.
 *
 * @param PWMSS_ID It is the instance number of EPWM of pwm sub system.
 * 
 * @return true if successful 
 * @return false if not successful
 *
 **/
bool beagle_pwm_init(uint32_t pwmss_id);

/* PWMSS setting
 *      set pulse argument of epwm module
 *
 *      @param pwm_id    : EPWMSS number , 0~2
 *      @param pwm_freq : frequency to be generated
 *      @param dutyA    : Duty Cycle in ePWM A
 *      @param dutyB    : Duty Cycle in ePWM B
 *
 *      @return         : 1 for success
 *      @return         : 0 for failed
 *
 *      @example        :  PWMSS_Setting(0 , 50.0f , 50.0f , 25.0f);      // Generate 50HZ pwm in PWM0 ,
 *                                                                              // duty cycle is 50% for ePWM0A , 25% for ePWM0B
 *
 *      @Note :
 *              find an number nearst 65535 for TBPRD , to improve duty precision,
 *
 *              Using big TBPRD can increase the range of CMPA and CMPB ,
 *              and it means we can get better precision on duty cycle.
 *
 *              EX : 20.25% duty cycle
 *                  on TBPRD = 62500 , CMPA = 12656.25 ( .25 rejection) , real duty : 20.2496% (12656 /62500)
 *                  on TBPRD = 6250  , CMPA = 1265.625 ( .625 rejection), real duty : 20.24%   (1265 6250)
 *                  on TBPRD = 500   , CMPA = 101.25   ( .25 rejection) , real duty : 20.2%    (101/500)
 *
 *              Divisor = CLKDIV * HSPCLKDIV
 *                      1 TBPRD : 10 ns (default)
 *                      65535 TBPRD : 655350 ns
 *                      65535 TBPRD : 655350 * Divisor ns  = X TBPRD : Cyclens
 *
 *              accrooding to that , we must find a Divisor value , let X nearest 65535 .
 *              so , Divisor must  Nearest Cyclens/655350
 */
int beagle_pwmss_setting(uint32_t pwm_id, float pwm_freq, float dutyA, float dutyB);

/**
 * @brief   This API enables the particular PWM module.
 *
 * @param   pwmid  It is the instance number of EPWM of pwm sub system.
 *
 * @return  true if successful
 * @return  false if fail
 *
 **/
bool beagle_ehrpwm_enable(uint32_t pwmid);

/**
 * @brief   This API disables the HR sub-module.
 *
 * @param   pwmid  It is the instance number of EPWM of pwm sub system.
 *
 * @return  true if successful
 * @return  false if fail
 *
 **/
bool beagle_ehrpwm_disable(uint32_t pwmid);

/**
 * @brief   This function Enables pinmuxing for PWM module.
 *
 * @param   pin_no  It is individual pin at which freuqency need to be generated.
 *                  It should be according to pwm sub system.
 *
 * @param   pwm_id  It is the instance number of EPWM of pwmsubsystem.
 *
 * @return  true if successful
 * @return  false if fail
 *
 **/
bool beagle_epwm_pinmux_setup(uint32_t pin_no, uint32_t pwm_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_BBB_PWM_H */
