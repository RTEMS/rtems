/*
 * Copyright (c) 2013 Chris Nott.  All rights reserved.
 *
 *  Virtual Logic
 *  21-25 King St.
 *  Rockdale NSW 2216
 *  Australia
 *  <rtems@vl.com.au>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_TIM_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_TIM_H

#include <bsp/utility.h>

struct stm32f4_tim_s {
  uint16_t cr1; // Control register 1
#define STM32F4_TIMER_CR1_CKD_DIV           0x0300
#define STM32F4_TIMER_CR1_CKD_DIV1          0x0000
#define STM32F4_TIMER_CR1_CKD_DIV2          0x0100
#define STM32F4_TIMER_CR1_CKD_DIV3          0x0200
#define STM32F4_TIMER_CR1_ARPE              BSP_BIT16(7)
#define STM32F4_TIMER_CR1_CMS               0x0060
#define STM32F4_TIMER_CR1_CMS_EDGE          0x0000
#define STM32F4_TIMER_CR1_CMS_CENTER1       0x0020
#define STM32F4_TIMER_CR1_CMS_CENTER2       0x0040
#define STM32F4_TIMER_CR1_CMS_CENTER3       0x0060
#define STM32F4_TIMER_CR1_DIR               BSP_BIT16(4)
#define STM32F4_TIMER_CR1_DIR_UP            0x0000
#define STM32F4_TIMER_CR1_DIR_DOWN          0x0010
#define STM32F4_TIMER_CR1_DIR_OPM           0x0008
#define STM32F4_TIMER_CR1_DIR_OPM_CONT      0x0000
#define STM32F4_TIMER_CR1_DIR_OPM_STOP      0x0008
#define STM32F4_TIMER_CR1_DIR_URS           0x0004
#define STM32F4_TIMER_CR1_DIR_UDIS          0x0002
#define STM32F4_TIMER_CR1_DIR_UDIS_EN       0x0000
#define STM32F4_TIMER_CR1_DIR_UDIS_DIS      0x0002
#define STM32F4_TIMER_CR1_CEN               0x0001
  uint16_t reserved_02;
  uint16_t cr2; // Control register 2
  uint16_t reserved_06;
  uint16_t smcr;  // Slave mode control register
  uint16_t reserved_0a;
  uint16_t dier;  // DMA / interrupt enable register
#define STM32F4_TIMER_DIER_TDE              BSP_BIT16(14) // Trigger DMA request enable
#define STM32F4_TIMER_DIER_CC4DE            BSP_BIT16(12) // Capture/compare 4 DMA request enable
#define STM32F4_TIMER_DIER_CC3DE            BSP_BIT16(11) // Capture/compare 3 DMA request enable
#define STM32F4_TIMER_DIER_CC2DE            BSP_BIT16(10) // Capture/compare 2 DMA request enable
#define STM32F4_TIMER_DIER_CC1DE            BSP_BIT16(9)  // Capture/compare 1 DMA request enable
#define STM32F4_TIMER_DIER_UDE              BSP_BIT16(8)  // Update DMA request enable
#define STM32F4_TIMER_DIER_TIE              BSP_BIT16(6)  // Trigger interrupt enable
#define STM32F4_TIMER_DIER_CC4IE            BSP_BIT16(4)  // Capture/compare 4 interrupt request enable
#define STM32F4_TIMER_DIER_CC3IE            BSP_BIT16(3)  // Capture/compare 3 interrupt request enable
#define STM32F4_TIMER_DIER_CC2IE            BSP_BIT16(2)  // Capture/compare 2 interrupt request enable
#define STM32F4_TIMER_DIER_CC1IE            BSP_BIT16(1)  // Capture/compare 1 interrupt request enable
#define STM32F4_TIMER_DIER_UIE              BSP_BIT16(0)  // Update interrupt request enable

  uint16_t reserved_0e;
  uint16_t sr;  // Status register
#define STM32F4_TIMER_SR_CC4OF              BSP_BIT16(12) // Capture/compare 4 overcapture flag
#define STM32F4_TIMER_SR_CC3OF              BSP_BIT16(11) // Capture/compare 3 overcapture flag
#define STM32F4_TIMER_SR_CC2OF              BSP_BIT16(10) // Capture/compare 2 overcapture flag
#define STM32F4_TIMER_SR_CC1OF              BSP_BIT16(9)  // Capture/compare 1 overcapture flag
#define STM32F4_TIMER_SR_TIF                BSP_BIT16(6)  // Trigger interrupt flag
#define STM32F4_TIMER_SR_CC4IF              BSP_BIT16(4)  // Capture/compare 4 interrupt flag
#define STM32F4_TIMER_SR_CC3IF              BSP_BIT16(3)  // Capture/compare 3 interrupt flag
#define STM32F4_TIMER_SR_CC2IF              BSP_BIT16(2)  // Capture/compare 2 interrupt flag
#define STM32F4_TIMER_SR_CC1IF              BSP_BIT16(1)  // Capture/compare 1 interrupt flag
#define STM32F4_TIMER_SR_UIF                BSP_BIT16(0)  // Update interrupt flag
  uint16_t reserved_12;
  uint16_t egr; // Event generation register
#define STM32F4_TIMER_EGR_TG                BSP_BIT16(6)  // Trigger event
#define STM32F4_TIMER_EGR_CC4G              BSP_BIT16(4)  // Capture/compare 4 event
#define STM32F4_TIMER_EGR_CC3G              BSP_BIT16(3)  // Capture/compare 3 generation)
#define STM32F4_TIMER_EGR_CC2G              BSP_BIT16(2)  // Capture/compare 2 generation)
#define STM32F4_TIMER_EGR_CC1G              BSP_BIT16(1)  // Capture/compare 1 generation)
#define STM32F4_TIMER_EGR_UG                BSP_BIT16(0)  // Update event
  uint16_t reserved_16;
  uint16_t ccmr1; // Capture / compare mode register 1
#define STM32F4_TIMER_CCMR1_OC2CE           BSP_BIT16(15) // Output compare 2 clear enable
#define STM32F4_TIMER_CCMR1_OC2M(val)       BSP_FLD16(val, 12, 14)
#define STM32F4_TIMER_CCMR1_OC2M_GET(reg)   BSP_FLD16GET(reg, 12, 14)
#define STM32F4_TIMER_CCMR1_OC2M_SET(reg, val) BSP_FLD16SET(reg, val, 12, 14)
#define STM32F4_TIMER_CCMR1_OC2M_FROZEN     STM32F4_TIMER_CCMR1_OC2M(0)
#define STM32F4_TIMER_CCMR1_OC2M_ACTIVE     STM32F4_TIMER_CCMR1_OC2M(1)
#define STM32F4_TIMER_CCMR1_OC2M_INACTIVE   STM32F4_TIMER_CCMR1_OC2M(2)
#define STM32F4_TIMER_CCMR1_OC2M_TOGGLE     STM32F4_TIMER_CCMR1_OC2M(3)
#define STM32F4_TIMER_CCMR1_OC2M_FORCE_LOW  STM32F4_TIMER_CCMR1_OC2M(4)
#define STM32F4_TIMER_CCMR1_OC2M_FORCE_HIGH STM32F4_TIMER_CCMR1_OC2M(5)
#define STM32F4_TIMER_CCMR1_OC2M_PWM1       STM32F4_TIMER_CCMR1_OC2M(6)
#define STM32F4_TIMER_CCMR1_OC2M_PWM2       STM32F4_TIMER_CCMR1_OC2M(7)
#define STM32F4_TIMER_CCMR1_OC2PE           BSP_BIT16(11) // Output compare 2 preload enable
#define STM32F4_TIMER_CCMR1_OC2FE           BSP_BIT16(10) // Output compare 2 fast enable
#define STM32F4_TIMER_CCMR1_CC2S(val)       BSP_FLD16(val, 8, 9)
#define STM32F4_TIMER_CCMR1_CC2S_GET(reg)   BSP_FLD16GET(reg, 8, 9)
#define STM32F4_TIMER_CCMR1_CC2S_SET(reg, val) BSP_FLD16SET(reg, val, 8, 9)
#define STM32F4_TIMER_CCMR1_CC2S_OUTPUT     STM32F4_TIMER_CCMR1_OC2S(0)
#define STM32F4_TIMER_CCMR1_CC2S_TI2        STM32F4_TIMER_CCMR1_OC2S(1)
#define STM32F4_TIMER_CCMR1_CC2S_TI1        STM32F4_TIMER_CCMR1_OC2S(2)
#define STM32F4_TIMER_CCMR1_CC2S_TRC        STM32F4_TIMER_CCMR1_OC2S(3)
#define STM32F4_TIMER_CCMR1_OC1CE           BSP_BIT16(7)  // Output compare 1 clear enable
#define STM32F4_TIMER_CCMR1_OC1M(val)       BSP_FLD16(val, 4, 6)
#define STM32F4_TIMER_CCMR1_OC1M_GET(reg)   BSP_FLD16GET(reg, 4, 6)
#define STM32F4_TIMER_CCMR1_OC1M_SET(reg, val) BSP_FLD16SET(reg, val, 4, 6)
#define STM32F4_TIMER_CCMR1_OC1M_FROZEN     STM32F4_TIMER_CCMR1_OC1M(0)
#define STM32F4_TIMER_CCMR1_OC1M_ACTIVE     STM32F4_TIMER_CCMR1_OC1M(1)
#define STM32F4_TIMER_CCMR1_OC1M_INACTIVE   STM32F4_TIMER_CCMR1_OC1M(2)
#define STM32F4_TIMER_CCMR1_OC1M_TOGGLE     STM32F4_TIMER_CCMR1_OC1M(3)
#define STM32F4_TIMER_CCMR1_OC1M_FORCE_LOW  STM32F4_TIMER_CCMR1_OC1M(4)
#define STM32F4_TIMER_CCMR1_OC1M_FORCE_HIGH STM32F4_TIMER_CCMR1_OC1M(5)
#define STM32F4_TIMER_CCMR1_OC1M_PWM1       STM32F4_TIMER_CCMR1_OC1M(6)
#define STM32F4_TIMER_CCMR1_OC1M_PWM2       STM32F4_TIMER_CCMR1_OC1M(7)
#define STM32F4_TIMER_CCMR1_OC1PE           BSP_BIT16(3)  // Output compare 1 preload enable
#define STM32F4_TIMER_CCMR1_OC1FE           BSP_BIT16(2)  // Output compare 1 fast enable
#define STM32F4_TIMER_CCMR1_CC1S(val)       BSP_FLD16(val, 0, 1)
#define STM32F4_TIMER_CCMR1_CC1S_GET(reg)   BSP_FLD16GET(reg, 0, 1)
#define STM32F4_TIMER_CCMR1_CC1S_SET(reg, val) BSP_FLD16SET(reg, val, 0, 1)
#define STM32F4_TIMER_CCMR1_CC1S_OUTPUT     STM32F4_TIMER_CCMR1_OC1S(0)
#define STM32F4_TIMER_CCMR1_CC1S_TI2        STM32F4_TIMER_CCMR1_OC1S(1)
#define STM32F4_TIMER_CCMR1_CC1S_TI1        STM32F4_TIMER_CCMR1_OC1S(2)
#define STM32F4_TIMER_CCMR1_CC1S_TRC        STM32F4_TIMER_CCMR1_OC1S(3)
  uint16_t reserved_1a;
  uint16_t ccmr2; // Capture / compare mode register 2
#define STM32F4_TIMER_CCMR2_OC4CE           BSP_BIT16(15) // Output compare 4 clear enable
#define STM32F4_TIMER_CCMR2_OC4M(val)       BSP_FLD16(val, 12, 14)
#define STM32F4_TIMER_CCMR2_OC4M_GET(reg)   BSP_FLD16GET(reg, 12, 14)
#define STM32F4_TIMER_CCMR2_OC4M_SET(reg, val) BSP_FLD16SET(reg, val, 12, 14)
#define STM32F4_TIMER_CCMR2_OC4M_FROZEN     STM32F4_TIMER_CCMR2_OC4M(0)
#define STM32F4_TIMER_CCMR2_OC4M_ACTIVE     STM32F4_TIMER_CCMR2_OC4M(1)
#define STM32F4_TIMER_CCMR2_OC4M_INACTIVE   STM32F4_TIMER_CCMR2_OC4M(2)
#define STM32F4_TIMER_CCMR2_OC4M_TOGGLE     STM32F4_TIMER_CCMR2_OC4M(3)
#define STM32F4_TIMER_CCMR2_OC4M_FORCE_LOW  STM32F4_TIMER_CCMR2_OC4M(4)
#define STM32F4_TIMER_CCMR2_OC4M_FORCE_HIGH STM32F4_TIMER_CCMR2_OC4M(5)
#define STM32F4_TIMER_CCMR2_OC4M_PWM1       STM32F4_TIMER_CCMR2_OC4M(6)
#define STM32F4_TIMER_CCMR2_OC4M_PWM2       STM32F4_TIMER_CCMR2_OC4M(7)
#define STM32F4_TIMER_CCMR2_OC4PE           BSP_BIT16(11) // Output compare 4 preload enable
#define STM32F4_TIMER_CCMR2_OC4FE           BSP_BIT16(10) // Output compare 4 fast enable
#define STM32F4_TIMER_CCMR2_CC4S(val)       BSP_FLD16(val, 8, 9)
#define STM32F4_TIMER_CCMR2_CC4S_GET(reg)   BSP_FLD16GET(reg, 8, 9)
#define STM32F4_TIMER_CCMR2_CC4S_SET(reg, val) BSP_FLD16SET(reg, val, 8, 9)
#define STM32F4_TIMER_CCMR2_CC4S_OUTPUT     STM32F4_TIMER_CCMR2_OC4S(0)
#define STM32F4_TIMER_CCMR2_CC4S_TI2        STM32F4_TIMER_CCMR2_OC4S(1)
#define STM32F4_TIMER_CCMR2_CC4S_TI1        STM32F4_TIMER_CCMR2_OC4S(2)
#define STM32F4_TIMER_CCMR2_CC4S_TRC        STM32F4_TIMER_CCMR2_OC4S(3)
#define STM32F4_TIMER_CCMR2_OC3CE           BSP_BIT16(7)  // Output compare 3 clear enable
#define STM32F4_TIMER_CCMR2_OC3M(val)       BSP_FLD16(val, 4, 6)
#define STM32F4_TIMER_CCMR2_OC3M_GET(reg)   BSP_FLD16GET(reg, 4, 6)
#define STM32F4_TIMER_CCMR2_OC3M_SET(reg, val) BSP_FLD16SET(reg, val, 4, 6)
#define STM32F4_TIMER_CCMR2_OC3M_FROZEN     STM32F4_TIMER_CCMR2_OC3M(0)
#define STM32F4_TIMER_CCMR2_OC3M_ACTIVE     STM32F4_TIMER_CCMR2_OC3M(1)
#define STM32F4_TIMER_CCMR2_OC3M_INACTIVE   STM32F4_TIMER_CCMR2_OC3M(2)
#define STM32F4_TIMER_CCMR2_OC3M_TOGGLE     STM32F4_TIMER_CCMR2_OC3M(3)
#define STM32F4_TIMER_CCMR2_OC3M_FORCE_LOW  STM32F4_TIMER_CCMR2_OC3M(4)
#define STM32F4_TIMER_CCMR2_OC3M_FORCE_HIGH STM32F4_TIMER_CCMR2_OC3M(5)
#define STM32F4_TIMER_CCMR2_OC3M_PWM1       STM32F4_TIMER_CCMR2_OC3M(6)
#define STM32F4_TIMER_CCMR2_OC3M_PWM2       STM32F4_TIMER_CCMR2_OC3M(7)
#define STM32F4_TIMER_CCMR2_OC3PE           BSP_BIT16(3)  // Output compare 3 preload enable
#define STM32F4_TIMER_CCMR2_OC3FE           BSP_BIT16(2)  // Output compare 3 fast enable
#define STM32F4_TIMER_CCMR2_CC3S(val)       BSP_FLD16(val, 0, 1)
#define STM32F4_TIMER_CCMR2_CC3S_GET(reg)   BSP_FLD16GET(reg, 0, 1)
#define STM32F4_TIMER_CCMR2_CC3S_SET(reg, val) BSP_FLD16SET(reg, val, 0, 1)
#define STM32F4_TIMER_CCMR2_CC3S_OUTPUT     STM32F4_TIMER_CCMR2_OC3S(0)
#define STM32F4_TIMER_CCMR2_CC3S_TI2        STM32F4_TIMER_CCMR2_OC3S(1)
#define STM32F4_TIMER_CCMR2_CC3S_TI1        STM32F4_TIMER_CCMR2_OC3S(2)
#define STM32F4_TIMER_CCMR2_CC3S_TRC        STM32F4_TIMER_CCMR2_OC3S(3)
  uint16_t reserved_1e;
  uint16_t ccer;  // Capture / compare enable register
#define STM32F4_TIMER_CCER_CC4NP            BSP_BIT16(15) // Capture / compare 4 output polarity
#define STM32F4_TIMER_CCER_CC4P             BSP_BIT16(13) // Capture / compare 4 output polarity
#define STM32F4_TIMER_CCER_CC4E             BSP_BIT16(12) // Capture / compare 4 output enable
#define STM32F4_TIMER_CCER_CC3NP            BSP_BIT16(11) // Capture / compare 3 output polarity
#define STM32F4_TIMER_CCER_CC3P             BSP_BIT16(9)  // Capture / compare 3 output polarity
#define STM32F4_TIMER_CCER_CC3E             BSP_BIT16(8)  // Capture / compare 3 output enable
#define STM32F4_TIMER_CCER_CC2NP            BSP_BIT16(7)  // Capture / compare 2 output polarity
#define STM32F4_TIMER_CCER_CC2P             BSP_BIT16(5)  // Capture / compare 2 output polarity
#define STM32F4_TIMER_CCER_CC2E             BSP_BIT16(4)  // Capture / compare 2 output enable
#define STM32F4_TIMER_CCER_CC1NP            BSP_BIT16(3)  // Capture / compare 1 output polarity
#define STM32F4_TIMER_CCER_CC1P             BSP_BIT16(1)  // Capture / compare 1 output polarity
#define STM32F4_TIMER_CCER_CC1E             BSP_BIT16(0)  // Capture / compare 1 output enable
  uint16_t reserved_22;
  uint32_t cnt; // Counter register
#define STM32F4_TIMER_DR(val) BSP_FLD32(val, 0, 31)
#define STM32F4_TIMER_DR_GET(reg) BSP_FLD32GET(reg, 0, 31)
#define STM32F4_TIMER_DR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 31)
  uint16_t psc; // Prescalar
  uint16_t reserved_2a;
  uint32_t arr; // Auto-reload register
  uint16_t rcr; // Repetition counter register
  uint16_t rserved_32;
  uint32_t ccr[4];// Capture / compare registers
  uint16_t bdtr;  // Break and dead-time register
  uint16_t reserved_46;
  uint16_t dcr; // DMA control register
  uint16_t reserved_4a;
  uint16_t dmar;  // DMA address for full transfer
  uint16_t reserved_4e;
  uint16_t or;  // Option register
  uint16_t reserved_52;
} __attribute__ ((packed));
typedef struct stm32f4_tim_s stm32f4_tim;

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_TIM_H */
