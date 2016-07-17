/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief Support for PWM for the BeagleBone Black.
 */

/**
 * Copyright (c) 2016 Punit Vara <punitvara@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/** This file is based on 
  * https://github.com/VegetableAvenger/BBBIOlib/blob/master/BBBio_lib/BBBiolib_PWMSS.c
  */

#include <libcpu/am335x.h>
#include <stdio.h>
#include <bsp/gpio.h>
#include <bsp/bbb-gpio.h>
#include <bsp.h>
#include <bsp/bbb-pwm.h>
#include <bsp/beagleboneblack.h>

/* Currently these definitions are for BeagleBone Black board only
 * Later on Beagle-xM board support can be added in this code.
 * After support gets added if condition should be removed
 */
#if IS_AM335X

/*
 * @brief This function selects EPWM module to be enabled
 * 
 * @param pwm_id It is the instance number of EPWM of pwm sub system.
 * 
 * @return Base Address of respective pwm instant.
*/
static uint32_t select_pwm(BBB_PWMSS pwm_id)
{
  uint32_t baseAddr=0;
  
  if (pwm_id == BBB_PWMSS0) {
    baseAddr = AM335X_EPWM_0_REGS;
  } else if (pwm_id == BBB_PWMSS1) {
    baseAddr = AM335X_EPWM_1_REGS;
  } else if (pwm_id == BBB_PWMSS2) {
    baseAddr = AM335X_EPWM_2_REGS;
  } else {
    baseAddr = 0;
  }
  return baseAddr;	
}

/*
 * @brief This function selects PWM Sub system to be enabled
 *  
 * @param pwmss_id  The instance number of ePWMSS whose system clocks
 *                  have to be configured.
 * 
 * @return Base Address of respective pwmss instant.
*/
static uint32_t select_pwmss(BBB_PWMSS pwmss_id)
{
  uint32_t baseAddr=0;
  
  if (pwmss_id == BBB_PWMSS0) {
    baseAddr = AM335X_PWMSS0_MMAP_ADDR;
  } else if (pwmss_id == BBB_PWMSS1) {
    baseAddr = AM335X_PWMSS1_MMAP_ADDR;
  } else if (pwmss_id == BBB_PWMSS2) {
    baseAddr = AM335X_PWMSS2_MMAP_ADDR;
  } else {
    baseAddr = 0;
  }
  return baseAddr;
}

bool beagle_pwm_pinmux_setup(bbb_pwm_pin_t pin_no, BBB_PWMSS pwm_id)
{
  bool is_valid = true;
  
  if(pwm_id == BBB_PWMSS0) {
    if (pin_no == BBB_P9_21_0B) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_SPI0_D0) = BBB_MUXMODE(BBB_P9_21_MUX_PWM);
    } else if (pin_no == BBB_P9_22_0A) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_SPI0_SCLK) = BBB_MUXMODE(BBB_P9_22_MUX_PWM);
    } else if (pin_no == BBB_P9_29_0B) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_MCASP0_FSX) = BBB_MUXMODE(BBB_P9_29_MUX_PWM);
    } else if (pin_no == BBB_P9_31_0A) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_MCASP0_ACLKX) = BBB_MUXMODE(BBB_P9_31_MUX_PWM);
    } else { 
      is_valid = false;
    }
    
    } else if (pwm_id == BBB_PWMSS1) {
      	if (pin_no == BBB_P8_34_1B) {
      	  REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_LCD_DATA(11)) = BBB_MUXMODE(BBB_P8_34_MUX_PWM);
	} else if (pin_no == BBB_P8_36_1A) {
	  REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_LCD_DATA(10)) = BBB_MUXMODE(BBB_P8_36_MUX_PWM);
	} else if (pin_no == BBB_P9_14_1A) {
	  REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_GPMC_AD(2)) = BBB_MUXMODE(BBB_P9_14_MUX_PWM);
	} else if (pin_no == BBB_P9_16_1B) {
	  REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_GPMC_AD(3)) = BBB_MUXMODE(BBB_P9_16_MUX_PWM);
	} else { 
	  is_valid = false;
        }
   } else if (pwm_id == BBB_PWMSS2) {
	if (pin_no == BBB_P8_13_2B) {
	  REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_GPMC_AD(9)) = BBB_MUXMODE(BBB_P8_13_MUX_PWM);
	} else if (pin_no == BBB_P8_19_2A) {
	  REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_GPMC_AD(8)) = BBB_MUXMODE(BBB_P8_19_MUX_PWM);
	} else if (pin_no == BBB_P8_45_2A) {
	  REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_LCD_DATA(0)) = BBB_MUXMODE(BBB_P8_45_MUX_PWM);
	} else if (pin_no == BBB_P8_46_2B) {
	  REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_LCD_DATA(1)) = BBB_MUXMODE(BBB_P8_46_MUX_PWM);
	} else {
	  is_valid = false;
        }
  } else {
	is_valid = false;
  }
  return is_valid;
}

/**
 * @brief   This function Enables TBCLK(Time Base Clock) for specific
 *          EPWM instance of pwmsubsystem.
 *
 * @param   instance  It is the instance number of EPWM of pwmsubsystem.
 *
 * @return  true if successful
 *          false if unsuccessful
 **/
static bool pwmss_tbclk_enable(BBB_PWMSS instance)
{
  uint32_t enable_bit;
  bool is_valid = true;
  
  if (instance == BBB_PWMSS0)  {
    enable_bit = AM335X_PWMSS_CTRL_PWMSS0_TBCLKEN;
  }  else if (instance == BBB_PWMSS1)  {
       enable_bit = AM335X_PWMSS_CTRL_PWMSS1_TBCLKEN;
  }  else if (instance == BBB_PWMSS2)  {
       enable_bit = AM335X_PWMSS_CTRL_PWMSS2_TBCLKEN;
  }  else  {
       is_valid = false;
  }

  if (is_valid)
  {
    REG(AM335X_PADCONF_BASE + AM335X_PWMSS_CTRL) |= enable_bit;
  }

  return is_valid;
 }

/**
 * @brief   This functions enables clock for EHRPWM module in PWMSS subsystem.
 *
 * @param   pwm_id  It is the instance number of EPWM of pwm sub system.
 *
 * @return  true if successful
 *          false if unsuccessful 
 *
 **/
static bool pwm_clock_enable(BBB_PWMSS pwm_id)
{
  const bool id_is_valid = pwm_id < BBB_PWMSS_COUNT;	
  bool status = true;
  
  if (id_is_valid) {
    const uint32_t baseAddr = select_pwmss(pwm_id);
    REG(baseAddr + AM335X_PWMSS_CLKCONFIG) |= AM335X_PWMSS_CLK_EN_ACK;
  }  else  {
       status = false;
  }
  return status;
}

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
static bool pwmss_module_clk_config(BBB_PWMSS pwmss_id)
{
  bool is_valid = true;
  
  if(pwmss_id == BBB_PWMSS0) {
    const uint32_t is_functional = AM335X_CM_PER_EPWMSS0_CLKCTRL_IDLEST_FUNC <<
                         AM335X_CM_PER_EPWMSS0_CLKCTRL_IDLEST_SHIFT;
    const uint32_t clkctrl = AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS0_CLKCTRL;
    const uint32_t idle_bits = AM335X_CM_PER_EPWMSS0_CLKCTRL_IDLEST;
    const uint32_t is_enable = AM335X_CM_PER_EPWMSS0_CLKCTRL_MODULEMODE_ENABLE;
    const uint32_t module_mode = AM335X_CM_PER_EPWMSS0_CLKCTRL_MODULEMODE;
    
    REG(clkctrl) |= is_enable;
    while((REG(clkctrl) & module_mode) != is_enable);
    while((REG(clkctrl) & idle_bits) != is_functional);
    }
    else if(pwmss_id == BBB_PWMSS1) {
    const uint32_t is_functional = AM335X_CM_PER_EPWMSS1_CLKCTRL_IDLEST_FUNC <<
                         AM335X_CM_PER_EPWMSS1_CLKCTRL_IDLEST_SHIFT;
    const uint32_t clkctrl = AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS1_CLKCTRL;
    const uint32_t idle_bits = AM335X_CM_PER_EPWMSS1_CLKCTRL_IDLEST;
    const uint32_t is_enable = AM335X_CM_PER_EPWMSS1_CLKCTRL_MODULEMODE_ENABLE;
    const uint32_t module_mode = AM335X_CM_PER_EPWMSS1_CLKCTRL_MODULEMODE;

    REG(clkctrl) |= is_enable;
    while((REG(clkctrl) & module_mode) != is_enable);
    while((REG(clkctrl) & idle_bits) != is_functional);
    } else if(pwmss_id == BBB_PWMSS2) {
    const uint32_t is_functional = AM335X_CM_PER_EPWMSS2_CLKCTRL_IDLEST_FUNC <<
                         AM335X_CM_PER_EPWMSS2_CLKCTRL_IDLEST_SHIFT;
    const uint32_t clkctrl = AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS2_CLKCTRL;
    const uint32_t idle_bits = AM335X_CM_PER_EPWMSS2_CLKCTRL_IDLEST;
    const uint32_t is_enable = AM335X_CM_PER_EPWMSS2_CLKCTRL_MODULEMODE_ENABLE;
    const uint32_t module_mode = AM335X_CM_PER_EPWMSS2_CLKCTRL_MODULEMODE;

    REG(clkctrl) |= is_enable;
    while((REG(clkctrl) & module_mode) != is_enable);
    while((REG(clkctrl) & idle_bits) != is_functional);
    } else 
	is_valid = false;
  return is_valid;
}

bool beagle_pwm_init(BBB_PWMSS pwmss_id)
{
  const bool id_is_valid = pwmss_id < BBB_PWMSS_COUNT;
  bool status = true;
  
  if(id_is_valid) {
    pwmss_module_clk_config(pwmss_id);
    pwm_clock_enable(pwmss_id);	
    pwmss_tbclk_enable(pwmss_id);
  } else {
      status =false;
  }
  return status;
}

int beagle_pwm_configure(BBB_PWMSS pwm_id, float pwm_freq, float duty_a, float duty_b)
{
  uint32_t baseAddr;
  int status = 1;
  float cycle = 0.0f,divisor = 0;
  unsigned int i,j;
  const float CLKDIV_div[] = {1.0,2.0,4.0,8.0,16.0,32.0,64.0,128.0};
  const float HSPCLKDIV_div[] = {1.0, 2.0, 4.0, 6.0, 8.0, 10.0,12.0, 14.0};
  int NearCLKDIV =7,NearHSPCLKDIV =7,NearTBPRD =0;
 
  if (pwm_freq <= BBB_PWM_FREQ_THRESHOLD) {
    status =0;
  }
  
  if (duty_a < 0.0f || duty_a > 100.0f || duty_b < 0.0f || duty_b > 100.0f) {
    status = 0;
  }
  duty_a /= 100.0f;
  duty_b /= 100.0f;
  
  /** 10^9 /Hz compute time per cycle (ns) */
  cycle = 1000000000.0f / pwm_freq;

  /** am335x provide (128* 14) divider and per TBPRD means 10ns when divider 
    * and max TBPRD is 65535 so max cycle is 128 * 8 * 14 * 65535 * 10ns */
  divisor = (cycle / 655350.0f);
  if (divisor > (128 * 14)) {
    return 0;
  }
  else {
    for (i=0;i<8;i++) {
      for(j=0 ; j<8; j++) {
        if((CLKDIV_div[i] * HSPCLKDIV_div[j]) < (CLKDIV_div[NearCLKDIV]
                            * HSPCLKDIV_div[NearHSPCLKDIV]) && (CLKDIV_div[i] * HSPCLKDIV_div[j] > divisor)) {
          NearCLKDIV = i;
          NearHSPCLKDIV = j;
         }
      }
    }
  
  baseAddr = select_pwm(pwm_id);
  
  REG16(baseAddr + AM335X_EPWM_TBCTL) &= ~(AM335X_TBCTL_CLKDIV_MASK | AM335X_TBCTL_HSPCLKDIV_MASK);
  const uint16_t clkdiv_clear = (REG16(baseAddr + AM335X_EPWM_TBCTL) &
  (~AM335X_EPWM_TBCTL_CLKDIV));
  const uint16_t clkdiv_write = ((NearCLKDIV
  << AM335X_EPWM_TBCTL_CLKDIV_SHIFT) & AM335X_EPWM_TBCTL_CLKDIV);
  REG16(baseAddr + AM335X_EPWM_TBCTL) = clkdiv_clear | clkdiv_write;
  const uint16_t hspclkdiv_clear =  (REG16(baseAddr + AM335X_EPWM_TBCTL) &
  (~AM335X_EPWM_TBCTL_HSPCLKDIV));
  const uint16_t hspclkdiv_write = ((NearHSPCLKDIV <<
  AM335X_EPWM_TBCTL_HSPCLKDIV_SHIFT) & AM335X_EPWM_TBCTL_HSPCLKDIV);
  REG16(baseAddr + AM335X_EPWM_TBCTL) = hspclkdiv_clear | hspclkdiv_write;
  NearTBPRD = (cycle / (10.0 * CLKDIV_div[NearCLKDIV] * HSPCLKDIV_div[NearHSPCLKDIV]));
  const uint16_t shadow_mask =  (REG16(baseAddr + AM335X_EPWM_TBCTL) &
  (~AM335X_EPWM_PRD_LOAD_SHADOW_MASK));
  const uint16_t shadow_disable =  (((bool)AM335X_EPWM_SHADOW_WRITE_DISABLE <<
  AM335X_EPWM_TBCTL_PRDLD_SHIFT) & AM335X_EPWM_PRD_LOAD_SHADOW_MASK);
  REG16(baseAddr + AM335X_EPWM_TBCTL) = shadow_mask | shadow_disable;
  const uint16_t counter_mask = (REG16(baseAddr + AM335X_EPWM_TBCTL) &
  (~AM335X_EPWM_COUNTER_MODE_MASK));
  const uint16_t counter_shift = (((unsigned int)AM335X_EPWM_COUNT_UP <<
  AM335X_TBCTL_CTRMODE_SHIFT) &  AM335X_EPWM_COUNTER_MODE_MASK);
  REG16(baseAddr + AM335X_EPWM_TBCTL) = counter_mask | counter_shift;
  /*setting clock divider and freeze time base*/
  REG16(baseAddr + AM335X_EPWM_CMPB) = (unsigned short)((float)(NearTBPRD) * duty_b);
  REG16(baseAddr + AM335X_EPWM_CMPA) = (unsigned short)((float)(NearTBPRD) * duty_a);
  REG16(baseAddr + AM335X_EPWM_TBPRD) = (unsigned short)NearTBPRD;
  REG16(baseAddr + AM335X_EPWM_TBCNT) = 0;
  }
  return status;
}

bool beagle_pwm_enable(BBB_PWMSS pwmid)
{
  const bool id_is_valid = pwmid < BBB_PWMSS_COUNT;
  bool status = true;
  
  if (id_is_valid)  {
    const uint32_t baseAddr = select_pwm(pwmid);
  /* Initially set EPWMxA o/p high , when increasing counter = CMPA toggle o/p of EPWMxA */
    REG16(baseAddr + AM335X_EPWM_AQCTLA) = AM335X_EPWM_AQCTLA_ZRO_XAHIGH | (AM335X_EPWM_AQCTLA_CAU_EPWMXATOGGLE << AM335X_EPWM_AQCTLA_CAU_SHIFT);
  /* Initially set EPWMxB o/p high , when increasing counter = CMPA toggle o/p of EPWMxB */  
    REG16(baseAddr + AM335X_EPWM_AQCTLB) = AM335X_EPWM_AQCTLB_ZRO_XBHIGH | (AM335X_EPWM_AQCTLB_CBU_EPWMXBTOGGLE << AM335X_EPWM_AQCTLB_CBU_SHIFT);
    REG16(baseAddr + AM335X_EPWM_TBCNT) = 0;
  /* Set counter mode : Up-count mode */
    REG16(baseAddr + AM335X_EPWM_TBCTL) |=  AM335X_TBCTL_FREERUN  | AM335X_TBCTL_CTRMODE_UP;
  }  else  {
       status =false;
  }
  return status;	
}

bool beagle_pwm_disable(BBB_PWMSS pwmid)
{
  const bool id_is_valid = pwmid < BBB_PWMSS_COUNT;
  bool status = true;
  
  if (id_is_valid) {
    const uint32_t baseAddr = select_pwm(pwmid);
    REG16(baseAddr + AM335X_EPWM_TBCTL) = AM335X_EPWM_TBCTL_CTRMODE_STOPFREEZE;
    REG16(baseAddr + AM335X_EPWM_AQCTLA) = AM335X_EPWM_AQCTLA_ZRO_XALOW | (AM335X_EPWM_AQCTLA_CAU_EPWMXATOGGLE << AM335X_EPWM_AQCTLA_CAU_SHIFT);
    REG16(baseAddr + AM335X_EPWM_AQCTLB) = AM335X_EPWM_AQCTLA_ZRO_XBLOW | (AM335X_EPWM_AQCTLB_CBU_EPWMXBTOGGLE << AM335X_EPWM_AQCTLB_CBU_SHIFT);
    REG16(baseAddr + AM335X_EPWM_TBCNT)  = 0;
  }  else  {
 	status = false;
  }
  return status;
}

/**
 * @brief   This functions determines whether time base clock is enabled for EPWMSS
 *
 * @param   pwmss_id  The instance number of ePWMSS whose time base clock need to
 *                    be checked
 *                    
 * @return  returns 4 for PWMSS_ID = 2
 *          returns 2 for PWMSS_ID = 1
 *          returns 1 for PWMSS_ID = 0
 **/ 
static int pwmss_tb_clock_check(unsigned int pwmss_id)
{
  unsigned int reg_value;

  /*control module check*/
  reg_value = REG(AM335X_CONTROL_MODULE + AM335X_PWMSS_CTRL);
  return (reg_value & (1 << pwmss_id));
}

/**
 * @brief   This functions determines whether clock for EPWMSS is enabled or not.
 *
 * @param   It is the Memory address of the PWMSS instance used.
 *
 * @return  
 *
 **/
static unsigned int pwmss_clock_en_status(unsigned int pwmid)
{
  unsigned int status;
  const uint32_t baseAddr = select_pwmss(pwmid);
  
  status = REG(baseAddr + AM335X_PWMSS_CLKSTATUS);
  status = status >> 8 & 0x1;
  return status;
}

bool beagle_pwmss_is_running(unsigned int pwmss_id)
{
  const bool id_is_valid = pwmss_id < BBB_PWMSS_COUNT;
  bool status=true;
  
  if (id_is_valid) {
    status = pwmss_clock_en_status(pwmss_id);
    if(status){
    status = pwmss_tb_clock_check(pwmss_id);
    } else {
        status = false;
    }  
  } else {
  status = false;
  }
  return status;
}

#endif

/* For support of BeagleboardxM */
#if IS_DM3730

/* Currently this section is just to satisfy
 * GPIO API and to make the build successful.
 * Later on support can be added here.
 */
bool beagle_pwm_init(BBB_PWMSS pwmss_id)
{
  return false;
}
bool beagle_pwm_disable(BBB_PWMSS pwmid)
{
  return false;
}
bool beagle_pwm_enable(BBB_PWMSS pwmid)
{
  return false;
}
int beagle_pwm_configure(BBB_PWMSS pwm_id, float pwm_freq, float duty_a, float duty_b)
{
  return -1;
}
bool beagle_pwm_pinmux_setup(bbb_pwm_pin_t pin_no, BBB_PWMSS pwm_id)
{
  return false;
}
bool beagle_pwmss_is_running(unsigned int pwmss_id)
{
return false;
}
#endif
