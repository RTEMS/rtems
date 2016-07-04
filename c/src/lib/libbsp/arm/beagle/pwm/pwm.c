/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief Support for PWM for the BeagleBone Black.
 */

/**
 * Copyright (c) 2016 Punit Vara <punitvara at gmail.com>
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

/* Currently these definitions are for BeagleBone Black board only
 * Later on Beagle-xM board support can be added in this code.
 * After support gets added if condition should be removed
 */
#if IS_AM335X

/*
 * @brief This function select PWM module to be enabled
 * 
 * @param pwm_id It is the instance number of EPWM of pwm sub system.
 * 
 * @return Base Address of respective pwm instant.
*/
static uint32_t select_pwmss(uint32_t pwm_id)
{
uint32_t baseAddr=0;
   if (pwm_id == BBB_PWMSS0)
   {
   baseAddr = AM335X_EPWM_0_REGS;
   return baseAddr;
   }
   else if (pwm_id == BBB_PWMSS1)
   {
   baseAddr = AM335X_EPWM_1_REGS;
   return baseAddr;
   } 
   else if (pwm_id == BBB_PWMSS2)
   {
   baseAddr = AM335X_EPWM_2_REGS;
   return baseAddr;
   }
   else 
   {
   printf("Invalid PWM Id\n");
   return 0;   
   }
}

bool beagle_epwm_pinmux_setup(uint32_t pin_no, uint32_t pwm_id)
{
  switch(pwm_id)  {
   case BBB_PWMSS2:
       switch(pin_no) { 
           case BBB_P8_13_2B:
               REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_GPMC_AD(9)) = BBB_MUXMODE(BBB_MUX4);
               break;
           case BBB_P8_19_2A:
               REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_GPMC_AD(8)) = BBB_MUXMODE(BBB_MUX4);
               break;
           case BBB_P8_45_2A:
               REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_LCD_DATA(0)) = BBB_MUXMODE(BBB_MUX3);
               break;
           case BBB_P8_46_2B:
               REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_LCD_DATA(1)) = BBB_MUXMODE(BBB_MUX3);
               break;
           default :
               printf("Invalid pin for module 2\n");
               return false;
       }
       break;
   case BBB_PWMSS1:
       switch(pin_no) {
           case BBB_P8_34_1B:
               REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_LCD_DATA(11)) = BBB_MUXMODE(BBB_MUX2);
               break;
           case BBB_P8_36_1A:
               REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_LCD_DATA(10)) = BBB_MUXMODE(BBB_MUX2);
               break;
           case BBB_P9_14_1A:
               REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_GPMC_AD(2)) = BBB_MUXMODE(BBB_MUX6);
               break;
           case BBB_P9_16_1B:
               REG(AM335X_PADCONF_BASE + BBB_CONTROL_CONF_GPMC_AD(3)) = BBB_MUXMODE(BBB_MUX6);
               break;
           default :
               printf("Invalid pin for module 1\n");
               return false;
       }   
       break;
   case BBB_PWMSS0:
       switch(pin_no) {
           case BBB_P9_21_0B:
               REG(AM335X_PADCONF_BASE + AM335X_CONF_SPI0_D0) = BBB_MUXMODE(BBB_MUX3);
               break;
           case BBB_P9_22_0A:
               REG(AM335X_PADCONF_BASE + AM335X_CONF_SPI0_SCLK) = BBB_MUXMODE(BBB_MUX3);
               break;
           case BBB_P9_29_0B:
               REG(AM335X_PADCONF_BASE + AM335X_CONF_MCASP0_FSX) = BBB_MUXMODE(BBB_MUX1);
               break;
           case BBB_P9_31_0A:
               REG(AM335X_PADCONF_BASE + AM335X_CONF_MCASP0_ACLKX) = BBB_MUXMODE(BBB_MUX1);
               break;
           default:
               printf("Invalid pin for module 0\n");
               return false;
       }
       break;
   
   default:
       printf("Invalid PWM sub system\n");
       return false;
}
}

/**
 * @brief   This function Enables TBCLK(Time Base Clock) for specific
 *          EPWM instance of pwmsubsystem.
 *
 * @param   instance  It is the instance number of EPWM of pwmsubsystem.
 *
 * @return  true if successful
 **/
static bool pwmss_tbclk_enable(unsigned int instance)
{
uint32_t enable_bit;
bool is_valid = true;
  
  if (instance == BBB_PWMSS0)
  {
       enable_bit = AM335X_PWMSS_CTRL_PWMSS0_TBCLKEN;
  }
  else if (instance == BBB_PWMSS1)
  {
       enable_bit = AM335X_PWMSS_CTRL_PWMSS1_TBCLKEN;
  }
  else if (instance == BBB_PWMSS2)
  {
       enable_bit = AM335X_PWMSS_CTRL_PWMSS2_TBCLKEN;
  }
  else
  {
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
 * @return  None.
 *
 **/
static void epwm_clock_enable(uint32_t pwm_id)
{  
   if((pwm_id <3) && (pwm_id >=0)) {
       uint32_t baseAddr;
       baseAddr = select_pwmss(pwm_id);
               REG(baseAddr - AM335X_EPWM_REGS + AM335X_PWMSS_CLKCONFIG) |= AM335X_PWMSS_CLK_EN_ACK;
        } else {
       printf("Invalid pwm_id\n");
   }
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
 * @return  None.
 *
 */
static void module_clk_config(uint32_t pwmss_id)
{
        if(pwmss_id == 0)
        {
                REG(AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS0_CLKCTRL) |=
                        AM335X_CM_PER_EPWMSS0_CLKCTRL_MODULEMODE_ENABLE;
   
    while(AM335X_CM_PER_EPWMSS0_CLKCTRL_MODULEMODE_ENABLE !=
              (REG(AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS0_CLKCTRL) &
               AM335X_CM_PER_EPWMSS0_CLKCTRL_MODULEMODE));

    while((AM335X_CM_PER_EPWMSS0_CLKCTRL_IDLEST_FUNC <<
               AM335X_CM_PER_EPWMSS0_CLKCTRL_IDLEST_SHIFT) !=
              (REG(AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS0_CLKCTRL) &
               AM335X_CM_PER_EPWMSS0_CLKCTRL_IDLEST));
        }
        else if(pwmss_id == 1)
        {
                REG(AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS1_CLKCTRL) |=
                        AM335X_CM_PER_EPWMSS1_CLKCTRL_MODULEMODE_ENABLE;
   while(AM335X_CM_PER_EPWMSS1_CLKCTRL_MODULEMODE_ENABLE !=
              (REG(AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS1_CLKCTRL) &
               AM335X_CM_PER_EPWMSS1_CLKCTRL_MODULEMODE));

        while((AM335X_CM_PER_EPWMSS1_CLKCTRL_IDLEST_FUNC <<
               AM335X_CM_PER_EPWMSS1_CLKCTRL_IDLEST_SHIFT) !=
               (REG(AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS1_CLKCTRL) &
               AM335X_CM_PER_EPWMSS1_CLKCTRL_IDLEST));
        }
        else if(pwmss_id == 2)
        {
                REG(AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS2_CLKCTRL) |=
                        AM335X_CM_PER_EPWMSS2_CLKCTRL_MODULEMODE_ENABLE;
   while(AM335X_CM_PER_EPWMSS2_CLKCTRL_MODULEMODE_ENABLE !=
              (REG(AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS2_CLKCTRL) &
               AM335X_CM_PER_EPWMSS2_CLKCTRL_MODULEMODE));

        while((AM335X_CM_PER_EPWMSS2_CLKCTRL_IDLEST_FUNC <<
               AM335X_CM_PER_EPWMSS2_CLKCTRL_IDLEST_SHIFT) !=
               (REG(AM335X_CM_PER_ADDR + AM335X_CM_PER_EPWMSS2_CLKCTRL) &
                AM335X_CM_PER_EPWMSS2_CLKCTRL_IDLEST));
        }
        else
        {
       printf("Please enter valid pwm Id \n");
        }
}

bool beagle_pwm_init(uint32_t pwmss_id)
{
  bool status = true;
  if((pwmss_id <3) && (pwmss_id >=0)) 
  {
  module_clk_config(pwmss_id);
  epwm_clock_enable(pwmss_id); 
  pwmss_tbclk_enable(pwmss_id);
  return status;
  }
  else {
   status =false;
  return status;
  }
}

int beagle_pwmss_setting(uint32_t pwm_id, float pwm_freq, float dutyA, float dutyB)
{  
  uint32_t baseAddr;
  int status = 1;
   
  if(pwm_freq <= 0.5) {
   status =0;
   return status;
  }
  if(dutyA < 0.0f || dutyA > 100.0f || dutyB < 0.0f || dutyB > 100.0f) {
   status = 0;
   return status;
  }
  dutyA /= 100.0f;
  dutyB /= 100.0f;

  /*Compute necessary TBPRD*/
  float Cyclens = 0.0f;
  float Divisor =0;
  int i,j;
  const float CLKDIV_div[] = {1.0,2.0,4.0,8.0,16.0,32.0,64.0,128.0};
  const float HSPCLKDIV_div[] = {1.0, 2.0, 4.0, 6.0, 8.0, 10.0,12.0, 14.0};
  int NearCLKDIV =7;
  int NearHSPCLKDIV =7;
  int NearTBPRD =0;

  /** 10^9 /Hz compute time per cycle (ns) */
  Cyclens = 1000000000.0f / pwm_freq;

  /** am335x provide (128* 14) divider and per TBPRD means 10ns when divider 
    * and max TBPRD is 65535 so max cycle is 128 * 8 * 14 * 65535 * 10ns */
  Divisor = (Cyclens / 655350.0f);
   
  if(Divisor > (128 * 14)) {
   printf("Can't generate %f HZ",pwm_freq);
   return 0;
  }
  else {
   for (i=0;i<8;i++) {
       for(j=0 ; j<8; j++) {
           if((CLKDIV_div[i] * HSPCLKDIV_div[j]) < (CLKDIV_div[NearCLKDIV] 
                       * HSPCLKDIV_div[NearHSPCLKDIV]) && (CLKDIV_div[i] * HSPCLKDIV_div[j] > Divisor)) {
               NearCLKDIV = i;
               NearHSPCLKDIV = j;
           }
       }
   }
  baseAddr = select_pwmss(pwm_id); 
  REG16(baseAddr + AM335X_EPWM_TBCTL) &= ~(AM335X_TBCTL_CLKDIV_MASK | AM335X_TBCTL_HSPCLKDIV_MASK);
           
  REG16(baseAddr + AM335X_EPWM_TBCTL) = (REG16(baseAddr + AM335X_EPWM_TBCTL) &
  (~AM335X_EPWM_TBCTL_CLKDIV)) | ((NearCLKDIV 
  << AM335X_EPWM_TBCTL_CLKDIV_SHIFT) & AM335X_EPWM_TBCTL_CLKDIV);

  REG16(baseAddr + AM335X_EPWM_TBCTL) = (REG16(baseAddr + AM335X_EPWM_TBCTL) &
  (~AM335X_EPWM_TBCTL_HSPCLKDIV)) | ((NearHSPCLKDIV << 
  AM335X_EPWM_TBCTL_HSPCLKDIV_SHIFT) & AM335X_EPWM_TBCTL_HSPCLKDIV);

  NearTBPRD = (Cyclens / (10.0 * CLKDIV_div[NearCLKDIV] * HSPCLKDIV_div[NearHSPCLKDIV]));
       
  REG16(baseAddr + AM335X_EPWM_TBCTL) = (REG16(baseAddr + AM335X_EPWM_TBCTL) &
  (~AM335X_EPWM_PRD_LOAD_SHADOW_MASK)) | (((bool)AM335X_EPWM_SHADOW_WRITE_DISABLE <<
  AM335X_EPWM_TBCTL_PRDLD_SHIFT) & AM335X_EPWM_PRD_LOAD_SHADOW_MASK);

  REG16(baseAddr + AM335X_EPWM_TBCTL) = (REG16(baseAddr + AM335X_EPWM_TBCTL) &
  (~AM335X_EPWM_COUNTER_MODE_MASK)) | (((unsigned int)AM335X_EPWM_COUNT_UP <<
  AM335X_TBCTL_CTRMODE_SHIFT) &  AM335X_EPWM_COUNTER_MODE_MASK);

  /*setting clock divider and freeze time base*/
  REG16(baseAddr + AM335X_EPWM_CMPB) = (unsigned short)((float)(NearTBPRD) * dutyB);
  REG16(baseAddr + AM335X_EPWM_CMPA) = (unsigned short)((float)(NearTBPRD) * dutyA);
  REG16(baseAddr + AM335X_EPWM_TBPRD) = (unsigned short)NearTBPRD;
  REG16(baseAddr + AM335X_EPWM_TBCNT) = 0;
  }
  return status;
}

bool beagle_ehrpwm_enable(uint32_t pwmid)
{
  bool status = true;
  uint32_t baseAddr;
  if((pwmid<3) && (pwmid >=0)) {
  baseAddr = select_pwmss(pwmid);
  REG16(baseAddr + AM335X_EPWM_AQCTLA) = AM335X_EPWM_AQCTLA_ZRO_XAHIGH | (AM335X_EPWM_AQCTLA_CAU_EPWMXATOGGLE << AM335X_EPWM_AQCTLA_CAU_SHIFT);
  REG16(baseAddr + AM335X_EPWM_AQCTLB) = AM335X_EPWM_AQCTLB_ZRO_XBHIGH | (AM335X_EPWM_AQCTLB_CBU_EPWMXBTOGGLE << AM335X_EPWM_AQCTLB_CBU_SHIFT);
  REG16(baseAddr + AM335X_EPWM_TBCNT) = 0;
  REG16(baseAddr + AM335X_EPWM_TBCTL) |=  AM335X_TBCTL_FREERUN  | AM335X_TBCTL_CTRMODE_UP;
  return status;
  }
  else {
   status =false;
   return status;
  }
}

bool beagle_ehrpwm_disable(uint32_t pwmid)
{
  bool status = true;
  uint32_t baseAddr;
  if((pwmid<3) && (pwmid >=0)) {
  baseAddr = select_pwmss(pwmid);
  REG16(baseAddr + AM335X_EPWM_TBCTL) = AM335X_EPWM_TBCTL_CTRMODE_STOPFREEZE;
  REG16(baseAddr + AM335X_EPWM_AQCTLA) = AM335X_EPWM_AQCTLA_ZRO_XALOW | (AM335X_EPWM_AQCTLA_CAU_EPWMXATOGGLE << AM335X_EPWM_AQCTLA_CAU_SHIFT);
  REG16(baseAddr + AM335X_EPWM_AQCTLB) = AM335X_EPWM_AQCTLA_ZRO_XBLOW | (AM335X_EPWM_AQCTLB_CBU_EPWMXBTOGGLE << AM335X_EPWM_AQCTLB_CBU_SHIFT);
  REG16(baseAddr + AM335X_EPWM_TBCNT)  = 0;
  return status;
  }
  else {
   status = false;
   return status;
  }
}

#endif

/* For support of BeagleboardxM */
#if IS_DM3730

/* Currently this section is just to satisfy
 * GPIO API and to make the build successful.
 * Later on support can be added here.
 */
uint32_t select_pwmss(uint32_t pwm_id)
{
return -1;
}
bool pwmss_tbclk_enable(unsigned int instance)
{
return false;
}
bool beagle_pwm_init(uint32_t pwmss_id)
{
return false;
}
bool beagle_ehrpwm_disable(uint32_t pwmid)
{
return false;
}
bool beagle_ehrpwm_enable(uint32_t pwmid)
{
return false;
}
int beagle_pwmss_setting(uint32_t pwm_id, float pwm_freq, float dutyA, float dutyB)
{
return -1;
}
bool beagle_epwm_pinmux_setup(uint32_t pin_no, uint32_t pwm_id)
{
return false;
}

#endif
