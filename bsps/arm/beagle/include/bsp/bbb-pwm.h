/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief BeagleBone Black PWM support definitions.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2016 Punit Vara <punitvara@gmail.com>
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


typedef enum{
  BBB_P8_13_2B = 3,
  BBB_P8_19_2A,
  BBB_P8_45_2A,
  BBB_P8_46_2B,
  BBB_P8_34_1B,
  BBB_P8_36_1A,
  BBB_P9_14_1A,
  BBB_P9_16_1B,
  BBB_P9_21_0B,
  BBB_P9_22_0A,
  BBB_P9_29_0B,
  BBB_P9_31_0A
}bbb_pwm_pin_t;

#define BBB_P8_13_MUX_PWM 4 
#define BBB_P8_19_MUX_PWM 4
#define BBB_P8_45_MUX_PWM 3
#define BBB_P8_46_MUX_PWM 3
#define BBB_P8_34_MUX_PWM 2
#define BBB_P8_36_MUX_PWM 2
#define BBB_P9_14_MUX_PWM 6
#define BBB_P9_16_MUX_PWM 6
#define BBB_P9_21_MUX_PWM 3
#define BBB_P9_22_MUX_PWM 3
#define BBB_P9_29_MUX_PWM 1
#define BBB_P9_31_MUX_PWM 1
#define BBB_PWM_FREQ_THRESHOLD 0.5f

/**
 * @brief  BeagleBone Black PWM API.
 */

/**
 * @brief This function intilizes clock for pwm sub system.
 *
 * @param PWMSS_ID It is the instance number of EPWM of pwm sub system.
 * 
 * @return true if successful 
 * @return false if not successful
 *
 **/
bool beagle_pwm_init(BBB_PWMSS pwmss_id);

/* PWMSS setting
 *      set pulse argument of epwm module
 *
 *      @param pwm_id    : EPWMSS number , 0~2
 *      @param pwm_freq : frequency to be generated
 *      @param dutyA    : Duty Cycle(in percentage) in PWM channel A
 *      @param dutyB    : Duty Cycle(in percentage) in PWM channel B
 *
 *      @return         : 1 for success
 *      @return         : 0 for failed
 *
 *      @example        :  beagle_pwm_configure(0 , 50.0f , 50.0f , 25.0f);      // Generate 50HZ pwm in PWM0 ,
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
 *                      65535 TBPRD : 655350 * Divisor ns  = X TBPRD : Cycle
 *
 *              accrooding to that , we must find a Divisor value , let X nearest 65535 .
 *              so , Divisor must  Nearest Cycle/655350
 */
int beagle_pwm_configure(BBB_PWMSS pwm_id, float pwm_freq, float duty_a, float duty_b);

/**
 * @brief   This API enables the particular PWM module.
 *
 * @param   pwmid  It is the instance number of EPWM of pwm sub system.
 *
 * @return  true if successful
 * @return  false if fail
 *
 **/
bool beagle_pwm_enable(BBB_PWMSS pwmid);

/**
 * @brief   This API disables the particular PWM module.
 *
 * @param   pwmid  It is the instance number of EPWM of pwm sub system.
 *
 * @return  true if successful
 * @return  false if fail
 *
 **/
bool beagle_pwm_disable(BBB_PWMSS pwmid);

/**
 * @brief   This function enables pinmuxing for PWM module.
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
bool beagle_pwm_pinmux_setup(bbb_pwm_pin_t pin_no, BBB_PWMSS pwm_id);

/**
 * @brief   This function determines whether PWMSS-wide clocks enabled or not.
 *
 * @param   pwmss_id  It is the instance number of PWMSS which clocks need to be
 *                    checked.
 * 
 * @return  true if successful
 * @return  false if fail
 *
 **/
bool beagle_pwmss_is_running(unsigned int pwmss_id);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_BBB_PWM_H */
