/*
 * Copyright (c) 2014 Chris Nott.  All rights reserved.
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

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_ADC_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_ADC_H

#include <bsp/utility.h>

struct stm32f4_adc_chan_s {
	uint32_t sr;	// 0x00: Status register
#define STM32F4_ADC_SR_OVR			BSP_BIT32(5)	// Overrun
#define STM32F4_ADC_SR_STRT			BSP_BIT32(4)	// Regular channel start flag
#define STM32F4_ADC_SR_JSTRT		BSP_BIT32(3)	// Injected channel start flag
#define STM32F4_ADC_SR_JEOC			BSP_BIT32(2)	// Injected channel end of conversion
#define STM32F4_ADC_SR_EOC			BSP_BIT32(1)	// Regular channel end of conversion
#define STM32F4_ADC_SR_AWD			BSP_BIT32(0)	// Analog watchdog flag

	uint32_t cr1;	// 0x04: Control register 1
#define STM32F4_ADC_CR1_OVRIE		BSP_BIT32(26)	// Overrun interrupt enable
#define STM32F4_ADC_CR1_RES(val)	BSP_FLD32(val, 24, 25)	// Resolution
#define STM32F4_ADC_CR1_RES_GET(reg)	BSP_FLD32GET(reg, 24, 25)
#define STM32F4_ADC_CR1_RES_SET(reg, val)	BSP_FLD32SET(reg, val, 24, 25)
#define ADC_CR1_RES_12BIT	0
#define ADC_CR1_RES_10BIT	1
#define ADC_CR1_RES_8BIT	2
#define ADC_CR1_RES_6BIT	3
#define STM32F4_ADC_CR1_AWDEN		BSP_BIT32(23)	// Analog watchdog enable on regular channels
#define STM32F4_ADC_CR1_JAWDEN		BSP_BIT32(22)	// Analog watchdog enable on injected channels
#define STM32F4_ADC_CR1_DISCNUM(val)	BSP_FLD32(val, 13, 15)	// Discontinuous mode channel count
#define STM32F4_ADC_CR1_DISCNUM_GET(reg)	BSP_FLD32GET(reg, 13, 15)
#define STM32F4_ADC_CR1_DISCNUM_SET(reg, val)	BSP_FLD32SET(reg, val, 13, 15)
#define STM32F4_ADC_CR1_JDISCEN		BSP_BIT32(12)	// Discontinous mode on injected channels
#define STM32F4_ADC_CR1_DISCEN		BSP_BIT32(11)	// Discontinous mode on regular channels
#define STM32F4_ADC_CR1_JAUTO		BSP_BIT32(10)	// Automated injected group conversion
#define STM32F4_ADC_CR1_AWDSGL		BSP_BIT32(9)	// Enable watchdog on single channel in scan mode
#define STM32F4_ADC_CR1_SCAN		BSP_BIT32(8)	// Scan mode
#define STM32F4_ADC_CR1_JEOCIE		BSP_BIT32(7)	// Interrupt enable for injected channels
#define STM32F4_ADC_CR1_AWDIE		BSP_BIT32(6)	// Analog watchdog interrupt enable
#define STM32F4_ADC_CR1_EOCIE		BSP_BIT32(5)	// Interrupt enable for EOC
#define STM32F4_ADC_CR1_AWDCH(val)	BSP_FLD32(val, 0, 4)	// Analog watchdog channel select bits
#define STM32F4_ADC_CR1_AWDCH_GET(reg)	BSP_FLD32GET(reg, 0, 4)
#define STM32F4_ADC_CR1_AWDCH_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 4)

	uint32_t cr2;	// 0x08: Control register 2
#define STM32F4_ADC_CR2_SWSTART		BSP_BIT32(30)	// Start conversion of regular channels
#define STM32F4_ADC_CR2_EXTEN(val)	BSP_FLD32(val, 28, 29)	// External trigger enable for regular channels
#define STM32F4_ADC_CR2_EXTEN_GET(reg)	BSP_FLD32GET(reg, 28, 29)
#define STM32F4_ADC_CR2_EXTEN_SET(reg, val)	BSP_FLD32SET(reg, val, 28, 29)
#define STM32F4_ADC_CR2_JEXTEN(val)	BSP_FLD32(val, 20, 21)	// External trigger enable for injected channels
#define STM32F4_ADC_CR2_JEXTEN_GET(reg)	BSP_FLD32GET(reg, 20, 21)
#define STM32F4_ADC_CR2_JEXTEN_SET(reg, val)	BSP_FLD32SET(reg, val, 20, 21)
#define ADC_CR2_TRIGGER_DISABLE 0
#define ADC_CR2_TRIGGER_RISING 1
#define ADC_CR2_TRIGGER_FALLING 2
#define ADC_CR2_TRIGGER_BOTH 3
#define STM32F4_ADC_CR2_EXTSEL(val)	BSP_FLD32(val, 24, 27)	// External event select for regular group
#define STM32F4_ADC_CR2_EXTSEL_GET(reg)	BSP_FLD32GET(reg, 24, 27)
#define STM32F4_ADC_CR2_EXTSEL_SET(reg, val)	BSP_FLD32SET(reg, val, 24, 27)
#define ADC_CR2_EVT_TIMER1_CC1 0x0
#define ADC_CR2_EVT_TIMER1_CC2 0x1
#define ADC_CR2_EVT_TIMER1_CC3 0x2
#define ADC_CR2_EVT_TIMER2_CC2 0x3
#define ADC_CR2_EVT_TIMER2_CC3 0x4
#define ADC_CR2_EVT_TIMER2_CC4 0x5
#define ADC_CR2_EVT_TIMER2_TRGO 0x6
#define ADC_CR2_EVT_TIMER3_CC1 0x7
#define ADC_CR2_EVT_TIMER3_TRGO 0x8
#define ADC_CR2_EVT_TIMER4_CC1 0x9
#define ADC_CR2_EVT_TIMER5_CC1 0xa
#define ADC_CR2_EVT_TIMER5_CC2 0xb
#define ADC_CR2_EVT_TIMER5_CC3 0xc
#define ADC_CR2_EVT_TIMER8_CC1 0xd
#define ADC_CR2_EVT_TIMER8_TRGO 0xe
#define ADC_CR2_EVT_EXTI_11 0xf
#define STM32F4_ADC_CR2_JSWSTART	BSP_BIT32(22)	// Start conversion of injected channels
#define STM32F4_ADC_CR2_JEXTSEL(val)	BSP_FLD32(val, 16, 19)	// External event select for injected group
#define STM32F4_ADC_CR2_JEXTSEL_GET(reg)	BSP_FLD32GET(reg, 16, 19)
#define STM32F4_ADC_CR2_JEXTSEL_SET(reg, val)	BSP_FLD32SET(reg, val, 16, 19)
#define ADC_CR2_JEVT_TIMER1_CC4 0x0
#define ADC_CR2_JEVT_TIMER1_TRGO 0x1
#define ADC_CR2_JEVT_TIMER2_CC1 0x2
#define ADC_CR2_JEVT_TIMER2_TRGO 0x3
#define ADC_CR2_JEVT_TIMER3_CC2 0x4
#define ADC_CR2_JEVT_TIMER3_CC4 0x5
#define ADC_CR2_JEVT_TIMER4_CC1 0x6
#define ADC_CR2_JEVT_TIMER4_CC2 0x7
#define ADC_CR2_JEVT_TIMER4_CC3 0x8
#define ADC_CR2_JEVT_TIMER4_TRGO 0x9
#define ADC_CR2_JEVT_TIMER5_CC4 0xa
#define ADC_CR2_JEVT_TIMER5_TRGO 0xb
#define ADC_CR2_JEVT_TIMER8_CC2 0xc
#define ADC_CR2_JEVT_TIMER8_CC3 0xd
#define ADC_CR2_JEVT_TIMER8_CC4 0xe
#define ADC_CR2_JEVT_EXTI_15 0xf
#define STM32F4_ADC_CR2_ALIGN	BSP_BIT32(11)	// Data alignment
#define STM32F4_ADC_CR2_ALIGN_RIGHT		0
#define STM32F4_ADC_CR2_ALIGN_LEFT		STM32F4_ADC_CR2_ALIGN
#define STM32F4_ADC_CR2_EOCS	BSP_BIT32(10)	// End of conversion selection
#define STM32F4_ADC_CR2_DDS		BSP_BIT32(9)	// DMA disable selection (single ADC mode)
#define STM32F4_ADC_CR2_DMA		BSP_BIT32(8)	// DMA access mode (single ADC)
#define STM32F4_ADC_CR2_CONT	BSP_BIT32(1)	// Continuous conversion
#define STM32F4_ADC_CR2_ADON	BSP_BIT32(0)	// A/D converter ON

	uint32_t smpr1;	// 0x0C: Sample time register 1
#define ADC_SAMPLE_3CYCLE	0
#define ADC_SAMPLE_15CYCLE	1
#define ADC_SAMPLE_28CYCLE	2
#define ADC_SAMPLE_56CYCLE	3
#define ADC_SAMPLE_84CYCLE	4
#define ADC_SAMPLE_112CYCLE	5
#define ADC_SAMPLE_144CYCLE	6
#define ADC_SAMPLE_480CYCLE	7
#define STM32F4_ADC_SMP18(val)	BSP_FLD32(val, 24, 26)	// Channel 18 sampling time selection
#define STM32F4_ADC_SMP18_GET(reg)	BSP_FLD32GET(reg, 24, 26)
#define STM32F4_ADC_SMP18_SET(reg, val)	BSP_FLD32SET(reg, val, 24, 26)
#define STM32F4_ADC_SMP17(val)	BSP_FLD32(val, 21, 23)	// Channel 17 sampling time selection
#define STM32F4_ADC_SMP17_GET(reg)	BSP_FLD32GET(reg, 21, 23)
#define STM32F4_ADC_SMP17_SET(reg, val)	BSP_FLD32SET(reg, val, 21, 23)
#define STM32F4_ADC_SMP16(val)	BSP_FLD32(val, 18, 20)	// Channel 16 sampling time selection
#define STM32F4_ADC_SMP16_GET(reg)	BSP_FLD32GET(reg, 18, 20)
#define STM32F4_ADC_SMP16_SET(reg, val)	BSP_FLD32SET(reg, val, 18, 20)
#define STM32F4_ADC_SMP15(val)	BSP_FLD32(val, 15, 17)	// Channel 15 sampling time selection
#define STM32F4_ADC_SMP15_GET(reg)	BSP_FLD32GET(reg, 15, 17)
#define STM32F4_ADC_SMP15_SET(reg, val)	BSP_FLD32SET(reg, val, 15, 17)
#define STM32F4_ADC_SMP14(val)	BSP_FLD32(val, 12, 14)	// Channel 14 sampling time selection
#define STM32F4_ADC_SMP14_GET(reg)	BSP_FLD32GET(reg, 12, 14)
#define STM32F4_ADC_SMP14_SET(reg, val)	BSP_FLD32SET(reg, val, 12, 14)
#define STM32F4_ADC_SMP13(val)	BSP_FLD32(val, 9, 11)	// Channel 13 sampling time selection
#define STM32F4_ADC_SMP13_GET(reg)	BSP_FLD32GET(reg, 9, 11)
#define STM32F4_ADC_SMP13_SET(reg, val)	BSP_FLD32SET(reg, val, 9, 11)
#define STM32F4_ADC_SMP12(val)	BSP_FLD32(val, 6, 8)	// Channel 12 sampling time selection
#define STM32F4_ADC_SMP12_GET(reg)	BSP_FLD32GET(reg, 6, 8)
#define STM32F4_ADC_SMP12_SET(reg, val)	BSP_FLD32SET(reg, val, 6, 8)
#define STM32F4_ADC_SMP11(val)	BSP_FLD32(val, 3, 5)	// Channel 11 sampling time selection
#define STM32F4_ADC_SMP11_GET(reg)	BSP_FLD32GET(reg, 3, 5)
#define STM32F4_ADC_SMP11_SET(reg, val)	BSP_FLD32SET(reg, val, 3, 5)
#define STM32F4_ADC_SMP10(val)	BSP_FLD32(val, 0, 2)	// Channel 10 sampling time selection
#define STM32F4_ADC_SMP10_GET(reg)	BSP_FLD32GET(reg, 0, 2)
#define STM32F4_ADC_SMP10_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 2)

	uint32_t smpr2;		// 0x10: Sample time register 2
#define STM32F4_ADC_SMP9(val)	BSP_FLD32(val, 27, 29)	// Channel 9 sampling time selection
#define STM32F4_ADC_SMP9_GET(reg)	BSP_FLD32GET(reg, 27, 29)
#define STM32F4_ADC_SMP9_SET(reg, val)	BSP_FLD32SET(reg, val, 27, 29)
#define STM32F4_ADC_SMP8(val)	BSP_FLD32(val, 24, 26)	// Channel 8 sampling time selection
#define STM32F4_ADC_SMP8_GET(reg)	BSP_FLD32GET(reg, 24, 26)
#define STM32F4_ADC_SMP8_SET(reg, val)	BSP_FLD32SET(reg, val, 24, 26)
#define STM32F4_ADC_SMP7(val)	BSP_FLD32(val, 21, 23)	// Channel 7 sampling time selection
#define STM32F4_ADC_SMP7_GET(reg)	BSP_FLD32GET(reg, 21, 23)
#define STM32F4_ADC_SMP7_SET(reg, val)	BSP_FLD32SET(reg, val, 21, 23)
#define STM32F4_ADC_SMP6(val)	BSP_FLD32(val, 18, 20)	// Channel 6 sampling time selection
#define STM32F4_ADC_SMP6_GET(reg)	BSP_FLD32GET(reg, 18, 20)
#define STM32F4_ADC_SMP6_SET(reg, val)	BSP_FLD32SET(reg, val, 18, 20)
#define STM32F4_ADC_SMP5(val)	BSP_FLD32(val, 15, 17)	// Channel 5 sampling time selection
#define STM32F4_ADC_SMP5_GET(reg)	BSP_FLD32GET(reg, 15, 17)
#define STM32F4_ADC_SMP5_SET(reg, val)	BSP_FLD32SET(reg, val, 15, 17)
#define STM32F4_ADC_SMP4(val)	BSP_FLD32(val, 12, 14)	// Channel 4 sampling time selection
#define STM32F4_ADC_SMP4_GET(reg)	BSP_FLD32GET(reg, 12, 14)
#define STM32F4_ADC_SMP4_SET(reg, val)	BSP_FLD32SET(reg, val, 12, 14)
#define STM32F4_ADC_SMP3(val)	BSP_FLD32(val, 9, 11)	// Channel 3 sampling time selection
#define STM32F4_ADC_SMP3_GET(reg)	BSP_FLD32GET(reg, 9, 11)
#define STM32F4_ADC_SMP3_SET(reg, val)	BSP_FLD32SET(reg, val, 9, 11)
#define STM32F4_ADC_SMP2(val)	BSP_FLD32(val, 6, 8)	// Channel 2 sampling time selection
#define STM32F4_ADC_SMP2_GET(reg)	BSP_FLD32GET(reg, 6, 8)
#define STM32F4_ADC_SMP2_SET(reg, val)	BSP_FLD32SET(reg, val, 6, 8)
#define STM32F4_ADC_SMP1(val)	BSP_FLD32(val, 3, 5)	// Channel 1 sampling time selection
#define STM32F4_ADC_SMP1_GET(reg)	BSP_FLD32GET(reg, 3, 5)
#define STM32F4_ADC_SMP1_SET(reg, val)	BSP_FLD32SET(reg, val, 3, 5)
#define STM32F4_ADC_SMP0(val)	BSP_FLD32(val, 0, 2)	// Channel 0 sampling time selection
#define STM32F4_ADC_SMP0_GET(reg)	BSP_FLD32GET(reg, 0, 2)
#define STM32F4_ADC_SMP0_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 2)

	uint32_t jofr[4];	// 0x14-0x20: Injected channel data offset registers
#define STM32F4_ADC_JOFFSET(val)	BSP_FLD32(val, 0, 11)	// Data offset for injected channel
#define STM32F4_ADC_JOFFSET_GET(reg)	BSP_FLD32GET(reg, 0, 11)
#define STM32F4_ADC_JOFFSET_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 11)

	uint32_t htr;		// 0x24: Watchdog higher threshold register
#define STM32F4_ADC_HT(val)	BSP_FLD32(val, 0, 11)	// Analog watchdog higher threshold
#define STM32F4_ADC_HT_GET(reg)	BSP_FLD32GET(reg, 0, 11)
#define STM32F4_ADC_HT_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 11)

	uint32_t ltr; 		// 0x28: Watchdog lower threshold register
#define STM32F4_ADC_LT(val)	BSP_FLD32(val, 0, 11)	// Analog watchdog lower threshold
#define STM32F4_ADC_LT_GET(reg)	BSP_FLD32GET(reg, 0, 11)
#define STM32F4_ADC_LT_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 11)

	uint32_t sqr[3];	// 0x2c-0x34: Regular sequence registers
#define STM32F4_ADC_SQR_L(val)	BSP_FLD32(val, 20, 23)	// Regular channel sequence length
#define STM32F4_ADC_SQR_L_GET(reg)	BSP_FLD32GET(reg, 20, 23)
#define STM32F4_ADC_SQR_L_SET(reg, val)	BSP_FLD32SET(reg, val, 20, 23)

	uint32_t jsqr; 		// 0x38: Injected sequence register
#define STM32F4_ADC_JSQR_JL(val)	BSP_FLD32(val, 20, 21)	// Injected sequence length
#define STM32F4_ADC_JSQR_JL_GET(reg)	BSP_FLD32GET(reg, 20, 21)
#define STM32F4_ADC_JSQR_JL_SET(reg, val)	BSP_FLD32SET(reg, val, 20, 21)
#define STM32F4_ADC_JSQR_JSQ4(val)	BSP_FLD32(val, 15, 19)	// 4th conversion in injected sequence
#define STM32F4_ADC_JSQR_JSQ4_GET(reg)	BSP_FLD32GET(reg, 15, 19)
#define STM32F4_ADC_JSQR_JSQ4_SET(reg, val)	BSP_FLD32SET(reg, val, 15, 19)
#define STM32F4_ADC_JSQR_JSQ3(val)	BSP_FLD32(val, 10, 14)	// 3rd conversion in injected sequence
#define STM32F4_ADC_JSQR_JSQ3_GET(reg)	BSP_FLD32GET(reg, 10, 14)
#define STM32F4_ADC_JSQR_JSQ3_SET(reg, val)	BSP_FLD32SET(reg, val, 10, 14)
#define STM32F4_ADC_JSQR_JSQ2(val)	BSP_FLD32(val, 5, 9)	// 2nd conversion in injected sequence
#define STM32F4_ADC_JSQR_JSQ2_GET(reg)	BSP_FLD32GET(reg, 5, 9)
#define STM32F4_ADC_JSQR_JSQ2_SET(reg, val)	BSP_FLD32SET(reg, val, 5, 9)
#define STM32F4_ADC_JSQR_JSQ1(val)	BSP_FLD32(val, 0, 4)	// 1st conversion in injected sequence
#define STM32F4_ADC_JSQR_JSQ1_GET(reg)	BSP_FLD32GET(reg, 0, 4)
#define STM32F4_ADC_JSQR_JSQ1_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 4)

	uint32_t jdr[4];	// 0x3c-0x48: Injected data registers
#define STM32F4_ADC_JDATA(val)	BSP_FLD32(val, 0, 15)	// Injected data
#define STM32F4_ADC_JDATA_GET(reg)	BSP_FLD32GET(reg, 0, 15)
#define STM32F4_ADC_JDATA_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 15)

	uint32_t dr;		// 0x4c: Regular data register
#define STM32F4_ADC_DATA(val)	BSP_FLD32(val, 0, 15)	// Regular data
#define STM32F4_ADC_DATA_GET(reg)	BSP_FLD32GET(reg, 0, 15)
#define STM32F4_ADC_DATA_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 15)

} __attribute__ ((packed));
typedef struct stm32f4_adc_chan_s stm32f4_adc_chan;

struct stm32f4_adc_com_s {
	uint32_t csr; 		// 0x00: Common status register
#define STM32F4_ADC_CSR_OVR3	BSP_BIT32(21)	// Overrun flag ADC3
#define STM32F4_ADC_CSR_STRT3	BSP_BIT32(20)	// Regular start flag ADC3
#define STM32F4_ADC_CSR_JSTRT3	BSP_BIT32(19)	// Injected start flag ADC3
#define STM32F4_ADC_CSR_JEOC3	BSP_BIT32(18)	// Injected channel end of conversion flag ADC3
#define STM32F4_ADC_CSR_EOC3	BSP_BIT32(17)	// Channel end of conversion flag ADC3
#define STM32F4_ADC_CSR_AWD3	BSP_BIT32(16)	// Analog watchdog flag ADC3
#define STM32F4_ADC_CSR_OVR2	BSP_BIT32(13)	// Overrun flag ADC2
#define STM32F4_ADC_CSR_STRT2	BSP_BIT32(12)	// Regular start flag ADC2
#define STM32F4_ADC_CSR_JSTRT2	BSP_BIT32(11)	// Injected start flag ADC2
#define STM32F4_ADC_CSR_JEOC2	BSP_BIT32(10)	// Injected channel end of conversion flag ADC2
#define STM32F4_ADC_CSR_EOC2	BSP_BIT32(9)	// Channel end of conversion flag ADC2
#define STM32F4_ADC_CSR_AWD2	BSP_BIT32(8)	// Analog watchdog flag ADC2
#define STM32F4_ADC_CSR_OVR1	BSP_BIT32(5)	// Overrun flag ADC1
#define STM32F4_ADC_CSR_STRT1	BSP_BIT32(4)	// Regular start flag ADC1
#define STM32F4_ADC_CSR_JSTRT1	BSP_BIT32(3)	// Injected start flag ADC1
#define STM32F4_ADC_CSR_JEOC1	BSP_BIT32(2)	// Injected channel end of conversion flag ADC1
#define STM32F4_ADC_CSR_EOC1	BSP_BIT32(1)	// Channel end of conversion flag ADC1
#define STM32F4_ADC_CSR_AWD1	BSP_BIT32(0)	// Analog watchdog flag ADC1

	uint32_t ccr; 		// 0x00: Common control register
#define STM32F4_ADC_CCR_TSVREFE	BSP_BIT32(23)	// Temp sensor and Vrefint enable
#define STM32F4_ADC_CCR_VBATE	BSP_BIT32(22)	// Vbat enable
#define STM32F4_ADC_CCR_ADCPRE(val)	BSP_FLD32(val, 16, 17)	// ADC prescalar
#define STM32F4_ADC_CCR_ADCPRE_GET(reg)	BSP_FLD32GET(reg, 16, 17)
#define STM32F4_ADC_CCR_ADCPRE_SET(reg, val)	BSP_FLD32SET(reg, val, 16, 17)
#define ADC_ADCPRE_PCLK2_2 0
#define ADC_ADCPRE_PCLK2_4 1
#define ADC_ADCPRE_PCLK2_6 2
#define ADC_ADCPRE_PCLK2_8 3
#define STM32F4_ADC_CCR_DMA(val)	BSP_FLD32(val, 14, 15)	// DMA access mode for multi ADC
#define STM32F4_ADC_CCR_DMA_GET(reg)	BSP_FLD32GET(reg, 14, 15)
#define STM32F4_ADC_CCR_DMA_SET(reg, val)	BSP_FLD32SET(reg, val, 14, 15)
#define ADC_DMA_DISABLE 0
#define ADC_DMA_MODE1 1
#define ADC_DMA_MODE2 2
#define ADC_DMA_MODE3 3
#define STM32F4_ADC_CCR_DDS	BSP_BIT32(13)	// DMA disable selection
#define STM32F4_ADC_CCR_DELAY(val)	BSP_FLD32(val, 8, 11)	// Delay between sampling phases
#define STM32F4_ADC_CCR_DELAY_GET(reg)	BSP_FLD32GET(reg, 8, 11)
#define STM32F4_ADC_CCR_DELAY_SET(reg, val)	BSP_FLD32SET(reg, val, 8, 11)
#define ADC_DELAY_5T 0
#define ADC_DELAY_6T 1
#define ADC_DELAY_7T 2
#define ADC_DELAY_8T 3
#define ADC_DELAY_9T 4
#define ADC_DELAY_10T 5
#define ADC_DELAY_11T 6
#define ADC_DELAY_12T 7
#define ADC_DELAY_13T 8
#define ADC_DELAY_14T 9
#define ADC_DELAY_15T 10
#define ADC_DELAY_16T 11
#define ADC_DELAY_17T 12
#define ADC_DELAY_18T 13
#define ADC_DELAY_19T 14
#define ADC_DELAY_20T 15
#define STM32F4_ADC_CCR_MULTI(val)	BSP_FLD32(val, 0, 4)	// Multi ADC mode
#define STM32F4_ADC_CCR_MULTI_GET(reg)	BSP_FLD32GET(reg, 0, 4)
#define STM32F4_ADC_CCR_MULTI_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 4)
#define ADC_MULTI_INDEPENDENT		0x00
#define ADC_MULTI_DUAL_REG_INJ		0x01
#define ADC_MULTI_DUAL_REG_ALT		0x02
#define ADC_MULTI_DUAL_INJ			0x05
#define ADC_MULTI_DUAL_REG			0x06
#define ADC_MULTI_DUAL_INTRL		0x07
#define ADC_MULTI_DUAL_ALT_TRIG		0x09
#define ADC_MULTI_TRIPLE_REG_INJ	0x11
#define ADC_MULTI_TRIPLE_REG_ALT	0x12
#define ADC_MULTI_TRIPLE_INJ		0x15
#define ADC_MULTI_TRIPLE_REG		0x16
#define ADC_MULTI_TRIPLE_INTRL		0x17
#define ADC_MULTI_TRIPLE_ALT_TRIG	0x19

	uint32_t cdr; 		// 0x00: Common regular data register
#define STM32F4_ADC_CDR_DATA2(val)	BSP_FLD32(val, 16, 31)	// 2nd data item
#define STM32F4_ADC_CDR_DATA2_GET(reg)	BSP_FLD32GET(reg, 16, 31)
#define STM32F4_ADC_CDR_DATA2_SET(reg, val)	BSP_FLD32SET(reg, val, 16, 31)
#define STM32F4_ADC_CDR_DATA1(val)	BSP_FLD32(val, 0, 15)	// 1st data item
#define STM32F4_ADC_CDR_DATA1_GET(reg)	BSP_FLD32GET(reg, 0, 15)
#define STM32F4_ADC_CDR_DATA1_SET(reg, val)	BSP_FLD32SET(reg, val, 0, 15)

} __attribute__ ((packed));
typedef struct stm32f4_adc_com_s stm32f4_adc_com;

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_ADC_H */
